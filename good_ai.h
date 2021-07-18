#pragma once
#include "ai.h"
#include "match_pattern.h"
#include <nlohmann/json.hpp>

struct compressed_board {
	std::array<uint32_t, 10> board = {};

	static compressed_board from_board(const tetris_board& b) {
		compressed_board ret;
		//const __m256i compare_to = _mm256_set1_epi8(0);
		for (int x = 0; x < 10; ++x) {

			const auto things = _mm256_load_si256((const __m256i*)&b.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			ret.board[x] = ~(unsigned)_mm256_movemask_epi8(c);
			/*
			for (int y = 0; y < b.minos[x].size(); ++y) {

				if (b.minos[x][y] != tetris_block::empty) {
					ret.board[x] |= 1 << y;
				}
			}
			*/
		}
		return ret;
	}

	bool operator==(const compressed_board&) const = default;
};


template<>
struct std::hash<compressed_board> {
	uint64_t operator()(const compressed_board& board) const noexcept {
		uint64_t ret = 0;
		const uint64_t* watland = (const uint64_t*)board.board.data();
		for (int i = 0; i < 5; ++i) {
			ret ^= std::hash<uint64_t>()(watland[i]) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
		}
		return ret;
	}
};

inline int max_allowed_extra_covered_sections(tetris_piece piece, int orientation) {
	if (piece == tetris_piece::I) {
		return 1;
	} else if (piece == tetris_piece::Z || piece == tetris_piece::S) {
		return 1;
	} else if (piece == tetris_piece::L || piece == tetris_piece::J) {
		if (orientation == 1 || orientation == 3) {
			return 1;
		} else {
			return 1;
		}
	} else if (piece == tetris_piece::O) {
		return 1;
	} else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return 0;
		}
		return 1;
	} else {
		return 0;
	}
}

inline std::array<int8_t, 8> disallowed_extra_thingys(tetris_piece piece, int orientation, int extra_sections, bool has_tspin) {
	if (piece == tetris_piece::I) {
		if (extra_sections == 2) {
			return {4, 5, 6, 7, 8, 9, 3, 10};
		}
		return {4, 5, 6, 7, 8, 9, 1 * has_tspin, 0};
	} else if (piece == tetris_piece::Z || piece == tetris_piece::S) {
		//if()
		return {3, 5, 6, 7, 8, 4 * has_tspin, 2 * has_tspin, 0};
	} else if (piece == tetris_piece::L || piece == tetris_piece::J) {
		if (orientation == 1 || orientation == 3) {
			return {2, 3, 5, 6, 7, 8, 4 * has_tspin, 0};
		} else {
			return {6, 7, 8, 4, 0, 0, 0, 0};
		}
	} else if (piece == tetris_piece::O) {
		return {0, 0, 8, 7, 6, 5, 2, 4 * has_tspin};
	} else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return {3, 4, 0, 0, 0, 0, 0, 0};
		}
		if (orientation == 2) {
			return {5, 4, 3, 0, 0, 0, 0, 0};
		}
		return {5, 4, 2, 3, 6, 1 * has_tspin, 0, 0};//no 2
	} else {
		return {0, 0, 0, 0, 0, 0, 0, 0};
	}
}

struct flatstacking_ai {
	static constexpr bool print_stuff = true;

	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		static uint64_t number_of_things_filtered_1 = 0;
		static uint64_t number_of_things_filtered_2 = 0;
		static uint64_t number_of_things_filtered_3 = 0;

		uint64_t number_of_things_filtered_1_this_iter = 0;
		uint64_t number_of_things_filtered_2_this_iter = 0;
		uint64_t number_of_things_filtered_3_this_iter = 0;
		uint64_t number_of_things_filtered_4_this_iter = 0;
		uint64_t number_of_things_filtered_5_this_iter = 0;

		uint64_t total_ply_depth_filter1 = 0;

		uint64_t total_size = 0;
		uint64_t branches_count = 0;


		const auto original_heights = col_heights(game.board.minos);
		const auto covered_sections_count0 = covered_sections(game.board);
		const auto covered_slots0 = covered_slots_raw(game.board);
		const auto start_time = std::chrono::steady_clock::now();
		int watland = 0;
		static int potatoland = 0;

		ska::bytell_hash_set<compressed_board> seen_boards;
		seen_boards.reserve(15000);

		auto res = iterate_board1(
			5, game,
			[&](const tetris_game& game, garbage_calculator calc, int depth, int total_lines_cleared, int total_lines_sent) {//next

				if constexpr (print_stuff) {
					++watland;
					if (watland % 100 == 0) {
						if (potatoland <= 3000 && potatoland >= 2800) {
							std::ofstream file("bonkland" + std::to_string(potatoland) + ".txt", std::ios::out);
							for (int y = 31; y >= 0; --y) {
								for (int x = 0; x < 10; ++x) {
									file << (int)game.board.minos[x][y] << ' ';
								}
								file << '\n';
							}
							file << (int)game.current_piece;
							file.close();
						}
						++potatoland;
						watland = 0;
					}
				}

				const auto prev_heights = col_heights(game.board.minos);
				const auto [min_h_it, max_h_it] = std::ranges::minmax_element(prev_heights);
				const auto min_height = *min_h_it;
				const auto max_height = *max_h_it;
				std::array<int8_t, 10> height_order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
				std::ranges::sort(height_order, std::less(), [&](int8_t a) { return prev_heights[a]; });

				const auto covered_sections_count = covered_sections(game.board);
				const auto covered_slots = covered_slots_raw(game.board);
				const auto just_placed_piece = game.current_piece;
				const auto dont_place_spots = do_not_place_spots(just_placed_piece, prev_heights, game, height_order);

				const auto number_of_Is = std::ranges::count_if(
					game.preview_pieces | ranges::views::take(std::min(depth + 1, 5)),
					[](auto a) { return a == tetris_piece::I; }
				) + (game.held_piece == tetris_piece::I);


				std::vector<next_move_thing> ret;
				ret.reserve(possible_hard_drop_spots[(int)game.current_piece].size());
				bool has_at_least_1_tspin = false;

				if (do_tspins) {
					for (const auto& tspin_matcher : tspindouble_matches) {
						const auto a = tspin_matcher.match(game.board, 0, 10, min_height, max_height);
						if (a) {
							has_at_least_1_tspin = true;
							if (just_placed_piece == tetris_piece::T) {
								const auto [x, y, o] = a.value();
								next_move_thing thing;
								thing.garb_state = calc;
								thing.game = game;
								thing.rotate_stuff = {true, true, false};
								thing.lines_cleared = thing.game.place_current_piece(x, y, o).value();
								if (thing.lines_cleared &&
									prev_heights[x - 1] > prev_heights[x] &&
									prev_heights[x + 1] > prev_heights[x] &&
									std::abs(prev_heights[x - 1] - prev_heights[x + 1]) > 1 &&
									prev_heights[x] < y &&
									(prev_heights[x - 1] <= y || prev_heights[x + 1] <= y)
								) {
									thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, thing.game.board.is_empty());
									ret.emplace_back(std::move(thing));
								}
							}
						}
					}
				}
				for (const auto& a : possible_hard_drop_spots[(int)game.current_piece]) {

					if (max_height >= 20 &&
						!game.board.can_place_piece_on_board(a.x, 20, piece_offsets[(int)game.current_piece][a.orientation])) {

						continue;
					}

					if (max_height <= 16 && ranges::contains(dont_place_spots, a)) {
						if constexpr (print_stuff) {
							++number_of_things_filtered_5_this_iter;
						}
						continue;
					}
					next_move_thing thing{.game = game, .garb_state = calc};

					thing.game.piece_center_x = (int)a.x;
					thing.game.orientation = (int)a.orientation;
					thing.rotate_stuff = {true, false, false};
					thing.lines_cleared = thing.game.hard_drop();
					const bool is_pc = thing.lines_cleared && thing.game.board.is_empty();

					thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, is_pc);
					if (is_pc) {
						ret.clear();
						ret.emplace_back(std::move(thing));
						if constexpr (print_stuff) {
							total_size += 1;
							++branches_count;
						}
						return ret;
					}

					const auto covered_sections2 = covered_sections(thing.game.board);
					const auto extra_sections = covered_sections2 - covered_sections_count;
					if (covered_sections2 - covered_sections_count > max_allowed_extra_covered_sections(just_placed_piece, a.orientation) ||
						(covered_sections2 - covered_sections_count0 > std::clamp(4 - depth, 1, 3))
					) {
						if constexpr (print_stuff) {
							++number_of_things_filtered_2;
							++number_of_things_filtered_2_this_iter;
						}
						continue;
					}
					const auto covered_slots2 = covered_slots_raw(thing.game.board);
					const auto extra_covered_slots = covered_slots2 - covered_slots;

					if (extra_sections > 0) {
						const auto disallowed_extra_thingys_ = disallowed_extra_thingys(just_placed_piece, a.orientation, extra_sections, has_at_least_1_tspin);
						if (extra_covered_slots >= 8 || ranges::contains(disallowed_extra_thingys_, extra_covered_slots) || covered_slots2 - covered_slots0 >= 10) {
							if constexpr (print_stuff) {
								++number_of_things_filtered_3;
								++number_of_things_filtered_3_this_iter;
							}
							continue;
						}
					}

					if ((depth > 1 && !seen_boards.insert(compressed_board::from_board(thing.game.board)).second && !false)) {
						if constexpr (print_stuff) {
							++number_of_things_filtered_1;
							++number_of_things_filtered_1_this_iter;
							total_ply_depth_filter1 += depth;
						}
						continue;
					}

					if (depth > 1 && number_of_Is < 3) {
						const auto heights = col_heights(thing.game.board.minos);
						const auto height_diffs_ = height_diffs(heights);
						const auto number_of_diffs_gteq3 = std::ranges::count_if(height_diffs_, [](int a) { return a >= 3; });

						if (number_of_diffs_gteq3 - 1 > number_of_Is) {
							if constexpr (print_stuff) {
								++number_of_things_filtered_4_this_iter;
							}
							continue;
						} else { }
					} else if (depth == 1) {
						//int max_height;
						//const auto heights = col_heights(thing.game.board.minos);
						if (!false &&
							thing.lines_cleared + total_lines_cleared != 0 &&
							total_lines_sent + thing.garbage_sent == 0) {
							if constexpr (print_stuff) {
								++number_of_things_filtered_4_this_iter;
							}
							continue;
						}
					}
					ret.emplace_back(std::move(thing));
				}

				if (!false && depth != 1) {
					if (ret.empty()) {
						return ret;
					}
					const int sizeland = std::max((int)(ret.size() * branch_factor_multiplier[depth]), 1);
					std::vector<double> evals = ret | ranges::views::transform([this](const next_move_thing& wat) {
						return evaluate_board(wat.game, wat.garbage_sent, wat.lines_cleared, wat.garb_state, false);
					}) | ranges::to<std::vector>();

					auto zipped = ranges::views::zip(ret, evals);
					if (true) {
						//method 1
						std::ranges::nth_element(zipped, zipped.begin() + sizeland, std::greater(), [](const auto& a) { return std::get<1>(a); });
						ret.erase(ret.begin() + sizeland, ret.end());
					} else {
						std::ranges::sort(zipped, std::greater(), [](const auto& a) { return std::get<1>(a); });

					}
				}
				if constexpr (print_stuff) {
					total_size += ret.size();
					++branches_count;
				}
				return ret;
			}
		   ,
			[this](const tetris_game& game, garbage_calculator calc, int garb_sent, int lines_cleared) {
				return evaluate_board(game, garb_sent, lines_cleared, calc);
			},
			garbage_state,
			[&](const tetris_game& game, int ply_depth)->bool {
				return true;
				//return seen_boards.insert(compressed_board::from_board(game.board)).second;
				const auto ret = seen_boards.insert(compressed_board::from_board(game.board)).second;
				if (!ret) {
					++number_of_things_filtered_1;
					++number_of_things_filtered_1_this_iter;
					total_ply_depth_filter1 += ply_depth;
				}
				return ret;
			},
			[](const tetris_game& prev_game, const tetris_game& next_game, int garbage_sent, garbage_calculator prev_garb_state, garbage_calculator next_garb_state) {
				return true;
			}
		);
		if constexpr (print_stuff) {
			const auto end_time = std::chrono::steady_clock::now();
			static int total_lines_cleared = 0;
			static int total_lines_sent = 0;
			static uint64_t total_seen = 0;
			static int thingys_placed = 0;

			total_seen += seen_boards.size();
			++thingys_placed;
			total_lines_cleared += res.lines_cleared;
			total_lines_sent += res.garbage_sent;
			const auto time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			static auto total_time_spent = 0ms;
			static auto total_time_spent_squared = 0ms;
			total_time_spent += time_spent;
			total_time_spent_squared += time_spent * time_spent.count();
			std::cout << "placed " << thingys_placed << std::endl;
			std::cout << seen_boards.size() << std::endl;
			std::cout << "avg: " << total_seen / thingys_placed << std::endl;
			std::cout << "this_iter1 " << number_of_things_filtered_1_this_iter << std::endl;
			std::cout << "this_iter2 " << number_of_things_filtered_2_this_iter << std::endl;
			std::cout << "this_iter3 " << number_of_things_filtered_3_this_iter << std::endl;
			std::cout << "this_iter4 " << number_of_things_filtered_4_this_iter << std::endl;
			std::cout << "this_iter5 " << number_of_things_filtered_5_this_iter << std::endl;
			std::cout << "branch factor " << (double)total_size / branches_count << std::endl;
			std::cout << "efficiency " << (double)total_lines_sent / total_lines_cleared << std::endl;
			std::cout << "time " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) << std::endl;
			std::cout << "average_time " << total_time_spent / thingys_placed << std::endl;
			std::cout << "time spent std " << std::sqrt((total_time_spent_squared / thingys_placed).count() -
				((total_time_spent / thingys_placed) * (total_time_spent.count() / thingys_placed)).count()) << std::endl;
		}
		return res;
	}

	sbo_vector<piece_hard_drop_spot, 24> do_not_place_spots(
		tetris_piece next_piece,
		const std::array<int8_t, 10>& heights,
		const tetris_game& game,
		const std::array<int8_t, 10>& height_order//smallest to largest
	) const {
		sbo_vector<piece_hard_drop_spot, 24> ret;

		if (next_piece == tetris_piece::T) {
			for (int i = 0; i < 7; ++i) {
				if (heights[i] == heights[i + 1] && heights[i + 1] == heights[i + 2]) {
					ret.emplace_back(i, 1);
					ret.emplace_back(i + 2, 3);
					ret.emplace_back(i + 1, 2);
				}
				if (heights[i] == heights[i + 2] && heights[i + 1] < heights[i]) {
					ret.emplace_back(i + 1, 0);
				}
			}
			for (const auto& anti_pattern : bad_T_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::L) {
			for (int i = 0; i < 7; ++i) {
				if ((heights[i] == heights[i + 1] && heights[i + 1] == heights[i + 2])
					|| (heights[i] >= heights[i + 2] && heights[i] >= heights[i + 1])) {
					ret.emplace_back(i + 1, 2);
				}
			}
			for (int i = 0; i < 8; ++i) {
				if (heights[i] <= heights[i + 1] ||
					heights[i] >= heights[i + 1] + 5) {
					ret.emplace_back(i + 1, 3);
				}
			}
			for (const auto& anti_pattern : bad_L_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::J) {
			for (int i = 0; i < 7; ++i) {
				if ((heights[i] == heights[i + 1] && heights[i + 1] == heights[i + 2])
					|| (heights[i + 2] >= heights[i] && heights[i + 2] >= heights[i + 1])) {
					ret.emplace_back(i + 1, 2);
				}
			}
			for (int i = 0; i < 8; ++i) {
				if (heights[i] >= heights[i + 1] ||
					heights[i] + 5 <= heights[i + 1]) {
					ret.emplace_back(i, 1);
				}
			}
			for (const auto& anti_pattern : bad_J_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::S) {
			const std::array<int8_t, 9> height_diffs = [&]() {
				std::array<int8_t, 10> h = {};
				ranges::adjacent_difference(heights, h);
				std::array<int8_t, 9> r = {};
				ranges::copy(h | ranges::views::tail, r.begin());
				return r;
			}();
			if (std::ranges::any_of(height_diffs, [](auto a) { return a == 1 || a == -1; })) {
				for (int i = 0; i < 7; ++i) {
					if (heights[i + 2] >= heights[i + 1] && heights[i] <= heights[i + 2]) {
						ret.emplace_back(i + 2, 1);
					}
				}
			}
			for (const auto& anti_pattern : bad_S_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::Z) {
			const std::array<int8_t, 9> height_diffs = [&]() {
				std::array<int8_t, 10> h;
				ranges::adjacent_difference(heights, h);
				std::array<int8_t, 9> r;
				ranges::copy(h | ranges::views::tail, r.begin());
				return r;
			}();
			if (std::ranges::any_of(height_diffs, [](auto a) { return a == 1 || a == -1; })) {
				for (int i = 0; i < 7; ++i) {
					if (heights[i] >= heights[i + 1] && heights[i] >= heights[i + 2]) {
						ret.push_back(piece_hard_drop_spot(i, 1));
					}
				}
			}

			for (const auto& anti_pattern : bad_Z_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::O) {

			const auto has_consecutive_equal = std::ranges::adjacent_find(heights, std::equal_to()) != heights.end();
			if (has_consecutive_equal) {
				for (int i = 0; i < 9; ++i) {
					const auto height_diff = std::abs(heights[i] - heights[i + 1]);
					if (height_diff >= 2) {
						ret.emplace_back(i, 0);
					}
				}
			}

			for (const auto& anti_pattern : bad_O_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		} else if (next_piece == tetris_piece::I) {
			if (!ranges::contains(height_order.begin(), height_order.begin() + 4, 8)) {
				ret.push_back({8, 1});
			}
			if (!ranges::contains(height_order.begin(), height_order.begin() + 4, 1)) {
				ret.push_back({1, 1});
			}
			if (heights[height_order[9]] - heights[height_order[7]] >= 4) {
				ret.emplace_back(height_order[9], 0);
				ret.emplace_back(height_order[9] + 1, 0);
				ret.emplace_back(height_order[9] - 1, 0);
				ret.emplace_back(height_order[9] - 2, 0);
			}

			for (const auto& anti_pattern : bad_I_spots) {
				ranges::push_back(ret, anti_pattern.match(game.board, 0, 10, heights[heights[0]], heights[heights[9]], heights));
			}
		}

		return ret;
	}

	static std::tuple<int, int> bumpiness(const tetris_board& board, int well_idx, std::array<int8_t, 10> heights) {
		std::shift_left(heights.begin() + well_idx, heights.end(), 1);
		int b = 0;
		int bumpiness_squared = 0;
		std::adjacent_difference(heights.begin(), heights.end() - 1, heights.begin());
		for (int height_diff : std::span(heights.begin() + 1, heights.end() - 1)) {
			b += std::abs(height_diff);
			bumpiness_squared += height_diff * height_diff;
		}
		return {b, bumpiness_squared};
	}

	static std::tuple<int, int> covered_slots(const tetris_board& board, const std::array<int8_t, 10>& heights) {
		int covered_slots = 0;
		int covered_square = 0;

		for (int x = 0; x < 10; ++x) {
			//*
			const auto things = _mm256_load_si256((const __m256i*)&board.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			auto b = (unsigned)_mm256_movemask_epi8(c) & ((1 << heights[x]) - 1);

			int i = 0;
			while (b) {
				const auto leading_zeros = std::countr_zero(b);
				b >>= leading_zeros;
				i += leading_zeros;
				const auto slots = std::min(6, heights[x] - i - 1);
				covered_slots += slots;
				covered_square += slots * slots;
				b >>= 1;
				++i;
			}
			//*/
			/*
			for (int y = 0; y < heights[x]; ++y) {
				if (board.minos[x][y] == tetris_block::empty) {
					const auto slots = std::min(6, heights[x] - y - 1);
					covered_slots += slots;
					covered_square += slots * slots;
				}
			}
			*/
		}
		return {covered_slots, covered_square};
	}

	static int covered_sections(const tetris_board& board) {
		int covered_sections = 0;

		for (int x = 0; x < 10; ++x) {
			const auto things = _mm256_load_si256((const __m256i*)&board.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			const auto b = ~(unsigned)_mm256_movemask_epi8(c);

			const auto not_empty_slots1 = b & 0b10101010101010101010101010101010;
			const auto should_be_empty_slots1 = (~b & 0b01010101010101010101010101010101);

			const auto not_empty_slots2 = b & 0b01010101010101010101010101010101;
			const auto should_be_empty_slots2 = (~b & 0b00101010101010101010101010101010);
			covered_sections += std::popcount(((not_empty_slots1 >> 1) & should_be_empty_slots1) | ((not_empty_slots2 >> 1) & should_be_empty_slots2));
		}


		return covered_sections;
	}

	static int covered_slots_raw(const tetris_board& board) {
		int covered_sections = 0;

		for (int x = 0; x < 10; ++x) {
			const auto things = _mm256_load_si256((const __m256i*)&board.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			const auto b = ~(unsigned)_mm256_movemask_epi8(c);
			const auto height = 32 - std::countl_zero(b);
			covered_sections += height - std::popcount(b);
		}
		return covered_sections;
	}

	static int well_depth(std::array<int8_t, 10> heights, int height_idx) {
		if (height_idx == 0) {
			return *ranges::min_element(heights | ranges::views::drop(1)) - heights[0];
		} else if (height_idx == 9) {
			return *ranges::min_element(heights | ranges::views::take(9)) - heights[9];
		} else {
			return std::min(
				*std::min_element(heights.begin(), heights.begin() + height_idx),
				*std::min_element(heights.begin() + height_idx + 1, heights.end())
			) - heights[height_idx];
		}
	}

	static int second_depth(std::array<int8_t, 10> heights, int well_idx) {
		if (well_idx == 0) {
			return *ranges::min_element(heights | ranges::views::drop(1));
		} else if (well_idx == 9) {
			return *ranges::min_element(heights | ranges::views::take(9));
		} else {
			return std::min(
				*std::min_element(heights.begin(), heights.begin() + well_idx),
				*std::min_element(heights.begin() + well_idx + 1, heights.end())
			);
		}
	}

	static std::array<int, 10> height_diffs(const std::array<int8_t, 10>& heights) {
		std::array<int, 10> ret;
		for (int i = 1; i < 9; ++i) {
			ret[i] = std::min(heights[i - 1], heights[i + 1]) - heights[i];
		}

		ret[0] = heights[1] - heights[0];
		ret[9] = heights[8] - heights[9];
		return ret;
	}

	double evaluate_board(const tetris_game& game, int total_garbage_sent, int total_lines_cleared, garbage_calculator garb_state, bool full_eval = true) const {
		const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
			return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
				return game.board.minos[x][y];
			});
		});

		std::array<int8_t, 10> heights = {};
		for (int i = 0; i < heights.size(); ++i) {
			heights[i] = (int8_t)get_col_height(game.board.minos[i]);
		}

		const auto [lowest_height_it, max_height_it] = std::ranges::minmax_element(heights);
		const auto lowest_height_idx = lowest_height_it - heights.begin();
		const auto lowest_height = *lowest_height_it;
		const auto max_height = *max_height_it;
		const auto [covered, covered_squared] = covered_slots(game.board, heights);
		//const auto covered_raw = covered_slots_raw(game.board);
		const auto [bumpines, bumpiness_squared] = bumpiness(game.board, lowest_height_idx, heights);
		//const auto second_deep = second_depth(heights, lowest_height_idx);
		std::array<int8_t, 9> heights_exculding_well = {};
		std::copy(heights.begin(), heights.begin() + lowest_height_idx, heights_exculding_well.begin());
		std::copy(heights.begin() + lowest_height_idx + 1, heights.end(), heights_exculding_well.begin() + lowest_height_idx);
		const auto second_deep = *std::ranges::min_element(heights_exculding_well);
		const auto average_height = ranges::accumulate(heights_exculding_well, 0) / 9.0;
		const auto height_varience =
				ranges::accumulate(heights_exculding_well | ranges::views::transform([](auto a) { return a * a; }), 0) / 9.0 - average_height * average_height;
		const auto height_std = std::sqrt(height_varience);
		const double line_efficency = (double)total_lines_cleared / (double)garbage_sent;

		double ret = total_garbage_sent * total_garbage_sent * (this->garbage_sent_squared)
				+ std::abs(bumpines) * (bumpiness_)
				+ bumpiness_squared * (this->bumpiness_squared - !do_tspins * 2)
				+ std::abs(covered) * (this->covered)
				+ covered_squared * (this->covered_squared - !do_tspins * 1)
				+ total_garbage_sent * (this->garbage_sent)
				+ line_efficency * (this->clear_efficency);

		if (full_eval) {
			if (total_lines_cleared && total_garbage_sent == 0) {
				ret += this->useless_line_clear;
			}

			if (total_lines_cleared >= 2 && total_garbage_sent == 1) {
				ret += inefficient_line_clear;
			}
		} else {
			if (total_lines_cleared && total_garbage_sent == 0) {
				ret -= 6;
			}

			if (total_lines_cleared >= 2 && total_garbage_sent == 1) {
				ret -= 5;
			}
		}

		for (int i = 0; i < 0; ++i) {
			if (i != lowest_height_idx) {
				ret -= (heights[i] - average_height) * height_varience / (std::abs(i - 5) + 1);
			}
		}

		if (total_garbage_sent > total_lines_cleared) {
			ret += efficent_line_clear;
		}

		if (max_height > 15) {
			ret += top_quarter;
		} else if (max_height > 10) {
			ret += top_half;
		}

		ret += height_std * this->height_std;

		const auto depth_diff = max_height - second_deep;
		if (depth_diff >= 6) {
			ret += (depth_diff - 4) * this->depth_diff_above;
		} else {
			ret += depth_diff + this->depth_diff_below;
		}

		ret += garb_state.is_b2b * this->is_b2b;

		const auto well_dep = std::min(lowest_height - second_deep, 6);

		ret += well_dep * this->well_dep;
		ret += (total_lines_cleared - total_garbage_sent) * this->wasted_line_clear;
		ret += (total_lines_cleared - total_garbage_sent) * this->wasted_line_clear_squared * (total_lines_cleared - total_garbage_sent);

		if (average_height >= 5 && average_height <= 9) {
			ret += this->in_optimal_height;
		}

		//ret += std::sqrt(std::abs(max_height - 5)) * this->height_dist_from_optimal;
		ret += well_idx[lowest_height_idx];
		return ret;
	}

	int max_height_diff = 2;
	int avg_height_between_constant = 13;
	int depth_diff_below = 1;
	int depth_diff_above = -4;
	double height_std = -1.5;
	int garbage_sent_squared = 5;
	int garbage_sent = 7;
	int useless_line_clear = -18;
	int inefficient_line_clear = -21;
	int top_quarter = -40;
	int top_half = -10;
	std::array<int, 10> well_idx = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int wasted_line_clear = -4;
	int wasted_line_clear_squared = -0;
	int well_dep = 3;
	int is_b2b = 8;
	int in_optimal_height = 13;
	int height_dist_from_optimal = -2;
	int efficent_line_clear = 31;
	int bumpiness_ = -1;
	int bumpiness_squared = -2;
	int covered = -8;
	int covered_squared = -3;
	int clear_efficency = 1;

	int max_height_diff_inter = 2;
	int avg_height_between_constant_inter = 13;
	int depth_diff_below_inter = 1;
	int depth_diff_above_inter = -4;
	double height_std_inter = 1.5;
	int garbage_sent_squared_inter = 5;
	int garbage_sent_inter = 7;
	int useless_line_clear_inter = -18;
	int inefficient_line_clear_inter = -21;
	int useless_line_clear_temp_inter = -6;
	int inefficient_line_clear_temp_inter = -5;
	int top_quarter_inter = -40;
	int top_half_inter = -10;
	std::array<int, 10> well_idx_inter = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int wasted_line_clear_inter = 2;
	int well_dep_inter = 3;
	int is_b2b_inter = 8;
	int in_optimal_height_inter = 13;
	int height_dist_from_optimal_inter = -2;
	int efficent_line_clear_inter = 31;

	int bumpiness_inter = -1;
	int bumpiness_squared_inter = 1;
	int covered_inter = -8;
	int covered_squared_inter = -8;


	bool do_tspins = true;
	std::vector<double> branch_factor_multiplier = {
		0.7,//
		0.7,//
		0.58,
		0.6,
		0.68,
		0.7,
		0.7
	};
	/*

	std::vector<double> branch_factor_multiplier = {
		0.7,//
		0.7,//
		0.48,
		0.52,
		0.56,
		0.64,
		0.7,
		0.7
	};

	*/

	double evaluate_board1(const tetris_game& game, int total_garbage_sent, int total_lines_cleared, garbage_calculator garb_state) const {
		const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
			return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
				return game.board.minos[x][y];
			});
		});

		std::array<int8_t, 10> heights = {};
		for (int i = 0; i < heights.size(); ++i) {
			heights[i] = (int8_t)get_col_height(game.board.minos[i]);
		}

		const auto [lowest_height_it, max_height_it] = std::ranges::minmax_element(heights);
		const auto lowest_height_idx = lowest_height_it - heights.begin();
		const auto lowest_height = *lowest_height_it;
		const auto max_height = *max_height_it;
		const auto [covered, covered_squared] = covered_slots(game.board, heights);
		//const auto covered_raw = covered_slots_raw(game.board);
		const auto [bumpines, bumpiness_squared] = bumpiness(game.board, lowest_height_idx, heights);
		//const auto second_deep = second_depth(heights, lowest_height_idx);
		std::array<int8_t, 9> heights_exculding_well = {};
		std::copy(heights.begin(), heights.begin() + lowest_height_idx, heights_exculding_well.begin());
		std::copy(heights.begin() + lowest_height_idx, heights.end(), heights_exculding_well.begin() + lowest_height_idx);
		const auto second_deep = *std::ranges::min_element(heights_exculding_well);
		const auto average_height = ranges::accumulate(heights_exculding_well, 0) / 9.0;
		const auto height_varience =
				ranges::accumulate(heights_exculding_well | ranges::views::transform([](auto a) { return a * a; }), 0) / 9.0 - average_height * average_height;
		const auto height_std = std::sqrt(height_varience);

		const double line_efficency = (double)total_lines_cleared / (double)garbage_sent;

		double ret = total_garbage_sent * total_garbage_sent * (5 - 2.5)
				- std::abs(bumpines)
				- bumpiness_squared * (2 + 1 + !do_tspins * 2)
				- std::abs(covered) * (8 - 2.5)
				- covered_squared * (3 + 1 + !do_tspins * 1)
				+ total_garbage_sent * (7 - 3.5);

		if (total_lines_cleared && total_garbage_sent == 0) {
			ret -= 6;
		}

		if (total_lines_cleared >= 2 && total_garbage_sent == 1) {
			ret -= 5;
		}

		for (int i = 0; i < 0; ++i) {
			if (i != lowest_height_idx) {
				ret -= (heights[i] - average_height) * height_varience / (std::abs(i - 5) + 1);
			}
		}

		if (total_garbage_sent > total_lines_cleared) {
			ret += 31;
		}

		if (max_height > 15) {
			ret -= 40;
		} else if (max_height > 10) {
			ret -= 8;
		}

		ret -= height_std * 1.5;

		const auto depth_diff = max_height - second_deep;
		if (depth_diff >= 6) {
			ret -= (depth_diff - 4) * 4;
		} else {
			ret += depth_diff;
		}

		ret += garb_state.is_b2b * 12;

		const auto well_dep = std::min(lowest_height - second_deep, 6);

		ret += well_dep * 3;
		ret += (total_garbage_sent - total_lines_cleared) * 2;

		if (average_height >= 5 && average_height <= 9) {
			ret += 13;
		}

		ret -= std::sqrt(std::abs(max_height - 5)) * 2;

		return ret;
	}


	friend void to_json(nlohmann::json& json, const flatstacking_ai& ai) {
		json["max_height_diff"] = ai.max_height_diff;
		json["avg_height_between_constant"] = ai.avg_height_between_constant;
		json["depth_diff_below"] = ai.depth_diff_below;
		json["depth_diff_above"] = ai.depth_diff_above;
		json["height_std"] = ai.height_std;
		json["garbage_sent_squared"] = ai.garbage_sent_squared;
		json["garbage_sent"] = ai.garbage_sent;
		json["useless_line_clear"] = ai.useless_line_clear;
		json["inefficient_line_clear"] = ai.inefficient_line_clear;
		json["top_quarter"] = ai.top_quarter;
		json["top_half"] = ai.top_half;
		json["well_idx"] = ai.well_idx;
		json["wasted_line_clear"] = ai.wasted_line_clear;
		json["well_dep"] = ai.well_dep;
		json["is_b2b"] = ai.is_b2b;
		json["in_optimal_height"] = ai.in_optimal_height;
		json["height_dist_from_optimal"] = ai.height_dist_from_optimal;
		json["efficent_line_clear"] = ai.efficent_line_clear;
		json["bumpiness_"] = ai.bumpiness_;
		json["bumpiness_squared"] = ai.bumpiness_squared;
		json["covered"] = ai.covered;
		json["covered_squared"] = ai.covered_squared;


		json["do_tspins"] = ai.do_tspins;
		json["branch_factor_multiplier"] = ai.branch_factor_multiplier;

	}

	friend void from_json(const nlohmann::json& json, flatstacking_ai& ai) {
		ai.max_height_diff = json["max_height_diff"].get<int>();
		ai.avg_height_between_constant = json["avg_height_between_constant"].get<int>();
		ai.depth_diff_below = json["depth_diff_below"].get<int>();
		ai.depth_diff_above = json["depth_diff_above"].get<int>();
		ai.height_std = json["height_std"].get<double>();
		ai.garbage_sent_squared = json["garbage_sent_squared"].get<int>();
		ai.garbage_sent = json["garbage_sent"].get<int>();
		ai.useless_line_clear = json["useless_line_clear"].get<int>();
		ai.inefficient_line_clear = json["inefficient_line_clear"].get<int>();
		ai.top_quarter = json["top_quarter"].get<int>();
		ai.top_half = json["top_half"].get<int>();
		ai.well_idx = json["well_idx"].get<std::array<int, 10>>();
		ai.wasted_line_clear = json["wasted_line_clear"].get<int>();
		ai.well_dep = json["well_dep"].get<int>();
		ai.is_b2b = json["is_b2b"].get<int>();
		ai.in_optimal_height = json["in_optimal_height"].get<int>();
		ai.height_dist_from_optimal = json["height_dist_from_optimal"].get<int>();
		ai.efficent_line_clear = json["efficent_line_clear"].get<int>();
		ai.bumpiness_ = json["bumpiness_"].get<int>();
		ai.bumpiness_squared = json["bumpiness_squared"].get<int>();
		ai.covered = json["covered"].get<int>();
		ai.covered_squared = json["covered_squared"].get<int>();


		ai.do_tspins = json["do_tspins"].get<bool>();
		ai.branch_factor_multiplier = json["branch_factor_multiplier"].get<std::vector<double>>();
	}

};
