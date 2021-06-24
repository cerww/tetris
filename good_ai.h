#pragma once
#include "ai.h"
#include "match_pattern.h"

struct compressed_board {
	std::array<uint32_t, 10> board = {};

	static compressed_board from_board(const tetris_board& b) {
		compressed_board ret;
		//const __m256i compare_to = _mm256_set1_epi8(0);
		for (int x = 0; x < 10; ++x) {

			const auto things = _mm256_load_si256((const __m256i*) & b.minos[x]);
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
		return 2;
	}
	else if (piece == tetris_piece::Z || piece == tetris_piece::S) {
		return 1;
	}
	else if (piece == tetris_piece::L || piece == tetris_piece::J) {
		if (orientation == 1 || orientation == 3) {
			return 1;
		}
		else {
			return 1;
		}
	}
	else if (piece == tetris_piece::O) {
		return 1;
	}
	else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return 0;
		}
		return 1;
	}
	else {
		return 0;
	}
}

inline std::array<int8_t, 8> disallowed_extra_thingys(tetris_piece piece, int orientation, int extra_sections, bool has_tspin) {
	if (piece == tetris_piece::I) {
		if (extra_sections == 2) {
			return { 4, 5, 6, 7, 8, 9, 3, 10 };
		}
		return { 4, 5, 6, 7, 8, 9, 1 * has_tspin, 0 };
	}
	else if (piece == tetris_piece::Z || piece == tetris_piece::S) {
		//if()
		return { 3, 5, 6, 7, 8, 4 * has_tspin, 2 * has_tspin, 0 };
	}
	else if (piece == tetris_piece::L || piece == tetris_piece::J) {
		if (orientation == 1 || orientation == 3) {
			return { 2, 3, 5, 6, 7, 8, 4 * has_tspin, 0 };
		}
		else {
			return { 6, 7, 8, 4, 0, 0, 0, 0 };
		}
	}
	else if (piece == tetris_piece::O) {
		return { 0, 0, 8, 7, 6, 5, 2, 4 * has_tspin };
	}
	else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return { 3, 4, 0, 0, 0, 0, 0, 0 };
		}
		if (orientation == 2) {
			return { 5, 4, 3, 0, 0, 0, 0, 0 };
		}
		return { 5, 4, 2, 3, 6, 1 * has_tspin, 0, 0 };//no 2
	}
	else {
		return { 0, 0, 0, 0, 0, 0, 0, 0 };
	}
}

struct flatstacking_ai {
	bool do_tspins = true;
	std::vector<double> branch_factor_multiplier = {
		0.7,
		0.7,
		0.6,
		0.65,
		0.7,
		0.7,
		0.7
	};

	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		static uint64_t number_of_things_filtered_1 = 0;
		static uint64_t number_of_things_filtered_2 = 0;
		static uint64_t number_of_things_filtered_3 = 0;

		uint64_t number_of_things_filtered_1_this_iter = 0;
		uint64_t number_of_things_filtered_2_this_iter = 0;
		uint64_t number_of_things_filtered_3_this_iter = 0;
		uint64_t number_of_things_filtered_4_this_iter = 0;

		uint64_t total_ply_depth_filter1 = 0;

		uint64_t total_size = 0;
		uint64_t branches_count = 0;

		ska::bytell_hash_set<compressed_board> seen_boards;
		seen_boards.reserve(15000);

		const auto original_heights = col_heights(game.board.minos);
		const auto covered_sections_count0 = covered_sections(game.board);
		const auto covered_slots0 = covered_slots_raw(game.board);
		const auto start_time = std::chrono::steady_clock::now();
		auto res = iterate_board1(
			5, game,
			[&](const tetris_game& game, garbage_calculator calc, int depth,int total_lines_cleared,int total_lines_sent) {//next
				const auto prev_heights = col_heights(game.board.minos);
				const auto [min_h_it, max_h_it] = std::ranges::minmax_element(prev_heights);
				const auto min_height = *min_h_it;
				const auto max_height = *max_h_it;
				std::array<int8_t, 10> height_order = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				std::ranges::sort(height_order, std::less(), [&](int8_t a) { return prev_heights[a]; });
				const auto covered_sections_count = covered_sections(game.board);
				const auto covered_slots = covered_slots_raw(game.board);
				const auto just_placed_piece = game.current_piece;


				const auto number_of_Is = std::ranges::count_if(
					game.preview_pieces | ranges::views::take(std::min(depth + 1,5)),
					[](auto a) { return a == tetris_piece::I; }
				) + (game.held_piece == tetris_piece::I);


				std::vector<next_move_thing> ret;
				ret.reserve(possible_hard_drop_spots[(int)game.current_piece].size());
				bool has_at_least_1_tspin = false;
				if (game.current_piece == tetris_piece::T && do_tspins) {
					for (const auto& tspin_matcher : tspindouble_matches) {
						const auto a = tspin_matcher.match(game.board, 0, 10, min_height, max_height);
						if (a) {
							const auto [x, y, o] = a.value();
							next_move_thing thing;
							thing.garb_state = calc;
							thing.game = game;
							thing.rotate_stuff = { true, true, false };
							thing.lines_cleared = thing.game.place_current_piece(x, y, o).value();
							if (thing.lines_cleared &&
								prev_heights[x - 1] > prev_heights[x] &&
								prev_heights[x + 1] > prev_heights[x] &&
								std::abs(prev_heights[x - 1] - prev_heights[x + 1]) > 1 &&
								prev_heights[x] < y &&
								(thing.lines_cleared == 1 || depth != 5 || prev_heights[x-1] == y || prev_heights[x+1] == y)
								) {

								thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, thing.game.board.is_empty());
								ret.emplace_back(std::move(thing));
								has_at_least_1_tspin = true;
							}
						}
					}
				}
				for (const auto& a : possible_hard_drop_spots[(int)game.current_piece]) {

					if (!game.board.can_place_piece_on_board(a.x, 20, piece_offsets[(int)game.current_piece][a.orientation])) {
						continue;
					}

					next_move_thing thing;

					thing.garb_state = calc;
					thing.game = game;

					thing.game.piece_center_x = (int)a.x;
					thing.game.orientation = (int)a.orientation;
					thing.rotate_stuff = { true, false, false };
					thing.lines_cleared = thing.game.hard_drop();
					const bool is_pc = thing.lines_cleared && thing.game.board.is_empty();

					thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, is_pc);
					if (is_pc) {
						ret.clear();
						ret.emplace_back(std::move(thing));
						total_size += 1;
						++branches_count;

						return ret;
					}

					const auto covered_sections2 = covered_sections(thing.game.board);
					const auto extra_sections = covered_sections2 - covered_sections_count;
					if (covered_sections2 - covered_sections_count > max_allowed_extra_covered_sections(just_placed_piece, a.orientation) ||
						(covered_sections2 - covered_sections_count0 > std::clamp(4 - depth, 1, 3))
						) {
						++number_of_things_filtered_2;
						++number_of_things_filtered_2_this_iter;
						continue;
					}
					const auto extra_covered_slots = covered_slots_raw(thing.game.board) - covered_slots;

					if (extra_sections > 0) {
						const auto disallowed_extra_thingys_ = disallowed_extra_thingys(just_placed_piece, a.orientation, extra_sections, has_at_least_1_tspin);
						if (extra_covered_slots >= 8 || ranges::contains(disallowed_extra_thingys_, extra_covered_slots)) {
							++number_of_things_filtered_3;
							++number_of_things_filtered_3_this_iter;
							continue;
						}
					}

					if ((depth > 1 && !seen_boards.insert(compressed_board::from_board(thing.game.board)).second)) {

						++number_of_things_filtered_1;
						++number_of_things_filtered_1_this_iter;
						total_ply_depth_filter1 += depth;
						continue;
					}

					if (depth > 1 && number_of_Is < 3) {
						const auto heights = col_heights(thing.game.board.minos);
						const auto height_diffs_ = height_diffs(heights);
						const auto number_of_diffs_gteq3 = std::ranges::count_if(height_diffs_, [](int a) { return a >= 3; });

						if (number_of_diffs_gteq3 - 1 > number_of_Is) {
							++number_of_things_filtered_4_this_iter;
							continue;
						}else {
							

							
						}
					}else if(depth == 1) {
						//int max_height;
						//const auto heights = col_heights(thing.game.board.minos);
						if (!false &&							
							thing.lines_cleared + total_lines_cleared != 0 &&
							total_lines_sent + thing.garbage_sent == 0) {

							++number_of_things_filtered_4_this_iter;
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

						std::ranges::nth_element(zipped, zipped.begin() + sizeland, std::greater(), [](const auto& a) { return std::get<1>(a); });
						ret.erase(ret.begin() + sizeland, ret.end());
				}

				total_size += ret.size();
				++branches_count;
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
		const auto end_time = std::chrono::steady_clock::now();
		static int total_lines_cleared = 0;
		static int total_lines_sent = 0;
		static uint64_t total_seen = 0;
		static int thingys_placed = 0;

		total_seen += seen_boards.size();
		++thingys_placed;
		total_lines_cleared += res.lines_cleared;
		total_lines_sent += res.garbage_sent;
		std::cout << seen_boards.size() << std::endl;
		std::cout << "avg: " << total_seen / thingys_placed << std::endl;
		std::cout << "this_iter1 " << number_of_things_filtered_1_this_iter << std::endl;
		std::cout << "this_iter2 " << number_of_things_filtered_2_this_iter << std::endl;
		std::cout << "this_iter3 " << number_of_things_filtered_3_this_iter << std::endl;
		std::cout << "this_iter4 " << number_of_things_filtered_4_this_iter << std::endl;
		std::cout << "branch factor " << (double)total_size / branches_count << std::endl;
		std::cout << "efficiency " << (double)total_lines_sent / total_lines_cleared << std::endl;
		std::cout << "time " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) << std::endl;
		return res;
	}

	std::vector<piece_hard_drop_spot> do_not_place_spots(tetris_piece next_piece, const std::array<int8_t, 10>& heights, const tetris_game& game) const {
		if (next_piece == tetris_piece::T) {
			//
		}
		else if (next_piece == tetris_piece::L) {
			//
		}
		else if (next_piece == tetris_piece::J) {
			//
		}
		else if (next_piece == tetris_piece::S) {
			//
		}
		else if (next_piece == tetris_piece::Z) {
			//
		}
		else if (next_piece == tetris_piece::O) {
			//
		}
		else if (next_piece == tetris_piece::I) {
			//
		}

		return {};
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
		return { b, bumpiness_squared };
	}

	static std::tuple<int, int> covered_slots(const tetris_board& board, const std::array<int8_t, 10>& heights) {
		int covered_slots = 0;
		int covered_square = 0;

		for (int x = 0; x < 10; ++x) {
			//*
			const auto things = _mm256_load_si256((const __m256i*) & board.minos[x]);
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
		return { covered_slots, covered_square };
	}

	static int covered_sections(const tetris_board& board) {
		int covered_sections = 0;

		for (int x = 0; x < 10; ++x) {
			const auto things = _mm256_load_si256((const __m256i*) & board.minos[x]);
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
			const auto things = _mm256_load_si256((const __m256i*) & board.minos[x]);
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
		}
		else if (height_idx == 9) {
			return *ranges::min_element(heights | ranges::views::take(9)) - heights[9];
		}
		else {
			return std::min(
				*std::min_element(heights.begin(), heights.begin() + height_idx),
				*std::min_element(heights.begin() + height_idx + 1, heights.end())
			) - heights[height_idx];
		}
	}

	static int second_depth(std::array<int8_t, 10> heights, int well_idx) {
		if (well_idx == 0) {
			return *ranges::min_element(heights | ranges::views::drop(1));
		}
		else if (well_idx == 9) {
			return *ranges::min_element(heights | ranges::views::take(9));
		}
		else {
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
		const auto [bumpines, bumpiness_squared] = bumpiness(game.board, lowest_height_idx, heights);
		//const auto second_deep = second_depth(heights, lowest_height_idx);
		std::array<int8_t, 9> heights_exculding_well = {};
		std::copy(heights.begin(), heights.begin() + lowest_height_idx, heights_exculding_well.begin());
		std::copy(heights.begin() + lowest_height_idx, heights.end(), heights_exculding_well.begin() + lowest_height_idx);
		const auto second_deep = *std::ranges::min_element(heights_exculding_well);
		const auto average_height = ranges::accumulate(heights_exculding_well, 0) / 9.0;
		const auto height_varience =
			ranges::accumulate(heights_exculding_well | ranges::views::transform([](auto a) { return a * a; }), 0) / 9.0 - average_height;

		double ret = total_garbage_sent * total_garbage_sent * (5 - !full_eval * 2.5)
			- std::abs(bumpines)
			- bumpiness_squared * (2 + !full_eval * 1)
			- std::abs(covered) * (8 - !full_eval * 2.5)
			- covered_squared * (3 - !full_eval * 1.5)
			+ total_garbage_sent * (7 - !full_eval * 2.5);

		if (full_eval) {
			if (total_lines_cleared && total_garbage_sent == 0) {
				ret -= 12;
			}

			if (total_lines_cleared >= 2 && total_garbage_sent == 1) {
				ret -= 14;
			}
		}

		if (total_garbage_sent > total_lines_cleared) {
			ret += 25;
		}
		
		if (max_height > 15) {
			ret -= 20;
		}
		else if (max_height > 10) {
			ret -= 5;
		}

		ret -= height_varience * 1.5;

		const auto depth_diff = max_height - second_deep;
		if (depth_diff >= 6) {
			ret -= (depth_diff - 4) * 4;
		}
		else {
			ret += depth_diff;
		}

		ret += garb_state.is_b2b * 8;

		const auto well_dep = std::min(lowest_height - second_deep, 6);

		ret += well_dep * 3;
		ret += (total_garbage_sent - total_lines_cleared) * 2;

		if (average_height >= 5 && average_height <= 9) {
			ret += 13;
		}

		ret -= std::sqrt(std::abs(max_height - 5)) * 2;

		return ret;
	}
};
