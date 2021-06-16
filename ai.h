#pragma once
#include <bitset>
#include <set>
#include <unordered_set>
#include <tuple>

#include "tetris_stuff.h"
#include <compare>
#include <ranges>
#include "bytell_hash_map.hpp"
#include <mmintrin.h>
#include <immintrin.h>
#include <iostream>

constexpr auto is_garbage_row = [](auto&& row) {
	return ranges::contains(row, tetris_block::garbage);
};

constexpr int garbage_height(const tetris_game& game) {
	const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
		return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
			return game.board.minos[x][y];
		});
	});
	return (int)(std::ranges::find_if(rows, is_garbage_row) - rows.begin());
}


struct match_pattern {
	int width = 0;
	int height = 0;
	std::array<std::array<int8_t, 5>, 5> pattern;//0 empty, 1 there,2 doesn't matter
	std::tuple<int, int, int> outcome;

	std::optional<std::tuple<int, int, int>> match(const tetris_board& board, int start_x, int end_x, int start_y, int end_y) const noexcept {
		for (int x = start_x; x < end_x - width; ++x) {
			for (int y = start_y; y < end_y - height; ++y) {
				for (int j = 0; j < height; ++j) {
					for (int i = 0; i < width; ++i) {
						//
						if ((pattern[j][i] == 0 && board.minos[x + i][y + j] != tetris_block::empty) ||
							(pattern[j][i] == 1 && board.minos[x + i][y + j] == tetris_block::empty)) {
							goto break_inner;
						}
					}
				}
				goto wat;
			break_inner:;
				continue;
			wat:;
				const auto [x_s, y_s, o] = outcome;
				//std::cout << "watmoreland" << std::endl;
				return std::tuple(x_s + x, y_s + y, o);
			}
		}

		return std::nullopt;
	}

};

inline match_pattern create_match_pattern(int w, int h, std::array<std::array<int8_t, 5>, 5> pattern, std::tuple<int, int, int> o) {
	std::reverse(pattern.begin(), pattern.begin() + h);
	return {w, h, pattern, o};
}

const auto tspin1 = create_match_pattern(
	3, 3,
	{
		{
			{{0, 0, 1}},
			{{0, 0, 0}},
			{{1, 0, 1}},
		}
	},
	{1, 1, 2}
);


struct place_next_piece_entry {
	int orientation = 0;
	int x = 0;
	int y = 0;
	bool swap_piece = false;
	rotate_info last_rotate_info;
};

struct piece_hard_drop_spot {
	int8_t x = 0;
	int8_t orientation = 0;
};

inline std::array<sbo_vector<piece_hard_drop_spot, 40>, 7> possible_hard_drop_spots = {
	{
		{//L
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
			piece_hard_drop_spot(1, 2),
			piece_hard_drop_spot(2, 2),
			piece_hard_drop_spot(3, 2),
			piece_hard_drop_spot(4, 2),
			piece_hard_drop_spot(5, 2),
			piece_hard_drop_spot(6, 2),
			piece_hard_drop_spot(7, 2),
			piece_hard_drop_spot(8, 2),
			piece_hard_drop_spot(0, 1),
			piece_hard_drop_spot(1, 1),
			piece_hard_drop_spot(2, 1),
			piece_hard_drop_spot(3, 1),
			piece_hard_drop_spot(4, 1),
			piece_hard_drop_spot(5, 1),
			piece_hard_drop_spot(6, 1),
			piece_hard_drop_spot(7, 1),
			piece_hard_drop_spot(8, 1),
			piece_hard_drop_spot(9, 3),
			piece_hard_drop_spot(1, 3),
			piece_hard_drop_spot(2, 3),
			piece_hard_drop_spot(3, 3),
			piece_hard_drop_spot(4, 3),
			piece_hard_drop_spot(5, 3),
			piece_hard_drop_spot(6, 3),
			piece_hard_drop_spot(7, 3),
			piece_hard_drop_spot(8, 3),
		},
		{//J
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
			piece_hard_drop_spot(1, 2),
			piece_hard_drop_spot(2, 2),
			piece_hard_drop_spot(3, 2),
			piece_hard_drop_spot(4, 2),
			piece_hard_drop_spot(5, 2),
			piece_hard_drop_spot(6, 2),
			piece_hard_drop_spot(7, 2),
			piece_hard_drop_spot(8, 2),
			piece_hard_drop_spot(0, 1),
			piece_hard_drop_spot(1, 1),
			piece_hard_drop_spot(2, 1),
			piece_hard_drop_spot(3, 1),
			piece_hard_drop_spot(4, 1),
			piece_hard_drop_spot(5, 1),
			piece_hard_drop_spot(6, 1),
			piece_hard_drop_spot(7, 1),
			piece_hard_drop_spot(8, 1),
			piece_hard_drop_spot(9, 3),
			piece_hard_drop_spot(1, 3),
			piece_hard_drop_spot(2, 3),
			piece_hard_drop_spot(3, 3),
			piece_hard_drop_spot(4, 3),
			piece_hard_drop_spot(5, 3),
			piece_hard_drop_spot(6, 3),
			piece_hard_drop_spot(7, 3),
			piece_hard_drop_spot(8, 3),
		},
		{//O
			piece_hard_drop_spot(0, 0),
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
		},
		{//I
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(0, 3),
			piece_hard_drop_spot(1, 3),
			piece_hard_drop_spot(2, 3),
			piece_hard_drop_spot(3, 3),
			piece_hard_drop_spot(4, 3),
			piece_hard_drop_spot(5, 3),
			piece_hard_drop_spot(6, 3),
			piece_hard_drop_spot(7, 3),
			piece_hard_drop_spot(8, 3),
			piece_hard_drop_spot(9, 3),
		},
		{//T
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
			piece_hard_drop_spot(1, 2),
			piece_hard_drop_spot(2, 2),
			piece_hard_drop_spot(3, 2),
			piece_hard_drop_spot(4, 2),
			piece_hard_drop_spot(5, 2),
			piece_hard_drop_spot(6, 2),
			piece_hard_drop_spot(7, 2),
			piece_hard_drop_spot(8, 2),
			piece_hard_drop_spot(0, 1),
			piece_hard_drop_spot(1, 1),
			piece_hard_drop_spot(2, 1),
			piece_hard_drop_spot(3, 1),
			piece_hard_drop_spot(4, 1),
			piece_hard_drop_spot(5, 1),
			piece_hard_drop_spot(6, 1),
			piece_hard_drop_spot(7, 1),
			piece_hard_drop_spot(8, 1),
			piece_hard_drop_spot(9, 3),
			piece_hard_drop_spot(1, 3),
			piece_hard_drop_spot(2, 3),
			piece_hard_drop_spot(3, 3),
			piece_hard_drop_spot(4, 3),
			piece_hard_drop_spot(5, 3),
			piece_hard_drop_spot(6, 3),
			piece_hard_drop_spot(7, 3),
			piece_hard_drop_spot(8, 3),
		},
		{//Z
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
			piece_hard_drop_spot(0, 1),
			piece_hard_drop_spot(1, 1),
			piece_hard_drop_spot(2, 1),
			piece_hard_drop_spot(3, 1),
			piece_hard_drop_spot(4, 1),
			piece_hard_drop_spot(5, 1),
			piece_hard_drop_spot(6, 1),
			piece_hard_drop_spot(7, 1),
			piece_hard_drop_spot(8, 1),
		},
		{//S
			piece_hard_drop_spot(1, 0),
			piece_hard_drop_spot(2, 0),
			piece_hard_drop_spot(3, 0),
			piece_hard_drop_spot(4, 0),
			piece_hard_drop_spot(5, 0),
			piece_hard_drop_spot(6, 0),
			piece_hard_drop_spot(7, 0),
			piece_hard_drop_spot(8, 0),
			piece_hard_drop_spot(1, 1),
			piece_hard_drop_spot(2, 1),
			piece_hard_drop_spot(3, 1),
			piece_hard_drop_spot(4, 1),
			piece_hard_drop_spot(5, 1),
			piece_hard_drop_spot(6, 1),
			piece_hard_drop_spot(7, 1),
			piece_hard_drop_spot(8, 1),
			piece_hard_drop_spot(0, 1),
		}
	},
};

struct next_move_thing {
	tetris_game game;
	int lines_cleared = 0;
	int garbage_sent = 0;
	garbage_calculator garb_state;
	bool dead = false;
	rotate_info rotate_stuff;
};

static constexpr auto asdasd = sizeof(next_move_thing);

template<typename P, typename E, typename F, typename F2>
double iterate_board(
	int ply_depth,
	tetris_game game,
	P&& get_possible_next,
	E&& evaluate,
	garbage_calculator prev_garb_state,
	int total_lines_sent,
	int total_lines_cleared,
	F&& filter,
	F2&& filter2
) {
	if (!ply_depth) {
		return evaluate(game, prev_garb_state, total_lines_sent, total_lines_cleared);
	}

	double ret = -1293212312;

	if (!game.try_spawn_new_piece()) {
		return ret;
	}

	for (next_move_thing next : get_possible_next(game, prev_garb_state, ply_depth)) {
		if (ply_depth == 1 || (filter2(game, next.game, next.garbage_sent, prev_garb_state, next.garb_state) && filter(next.game))) {
			const auto val = iterate_board(
				ply_depth - 1,
				std::move(next.game),
				get_possible_next,
				evaluate,
				next.garb_state,
				total_lines_sent + next.garbage_sent,
				next.lines_cleared + total_lines_cleared,
				filter,
				filter2
			);
			ret = std::max(ret, val);
		}
	}
	if (game.held_piece != game.current_piece) {
		game.swap_held_piece();
		for (next_move_thing next : get_possible_next(game, prev_garb_state, ply_depth)) {

			if (ply_depth == 1 || (filter2(game, next.game, next.garbage_sent, prev_garb_state, next.garb_state) && filter(next.game))) {
				const auto val = iterate_board(
					ply_depth - 1,
					std::move(next.game),
					get_possible_next,
					evaluate,
					next.garb_state,
					total_lines_sent + next.garbage_sent,
					next.lines_cleared + total_lines_cleared,
					filter,
					filter2
				);
				ret = std::max(ret, val);
			}
		}
	}
	return ret;
}

template<typename P, typename E, typename F, typename F2>
next_move_thing iterate_board1(
	int ply_depth_init,
	tetris_game game,
	P&& possible_next,
	E&& evaluate,
	garbage_calculator prev_garb_state,
	F&& filter,
	F2&& filter2
) {
	double ret_val = -129321312312;
	next_move_thing ret_move;

	if (!game.try_spawn_new_piece()) {
		return next_move_thing{std::move(game), 0, 0, prev_garb_state, true, {false, false, false}};
	}

	const auto next_ = possible_next(game, prev_garb_state, ply_depth_init);
	for (next_move_thing next : next_) {
		if (filter(next.game)) {
			const auto val = iterate_board(
				ply_depth_init - 1,
				next.game,
				possible_next,
				evaluate,
				next.garb_state,
				next.garbage_sent,
				next.lines_cleared,
				filter,
				filter2
			);
			if (val > ret_val) {
				ret_val = val;
				ret_move = std::move(next);
			}
		}
	}
	if (game.held_piece != game.current_piece) {
		game.swap_held_piece();
		const auto next_ = possible_next(game, prev_garb_state, ply_depth_init);
		for (next_move_thing next : next_) {
			if (filter(next.game)) {
				const auto val = iterate_board(
					ply_depth_init - 1,
					next.game,
					possible_next,
					evaluate,
					next.garb_state,
					next.garbage_sent,
					next.lines_cleared,
					filter,
					filter2
				);
				if (val > ret_val) {
					ret_val = val;
					ret_move = std::move(next);
				}
			}
		}
	}
	std::cout << ret_val << std::endl;
	return ret_move;
}

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
		for (int i = 0; i < 10; ++i) {
			ret ^= std::hash<uint32_t>()(board.board[i]) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
		}
		return ret;
	}
};

inline int max_allowed_extra_covered_sections(tetris_piece piece, int orientation) {
	if (piece == tetris_piece::I) {
		return 2;
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

inline std::array<int8_t, 8> disallowed_extra_thingys(tetris_piece piece, int orientation, int extra_sections) {
	if (piece == tetris_piece::I) {
		if (extra_sections == 2) {
			return {4, 5, 6, 7, 8, 9, 3, 10};
		}
		return {4, 5, 6, 7, 8, 9, 0, 0};
	} else if (piece == tetris_piece::Z || piece == tetris_piece::S) {
		return {3, 5, 6, 7, 8, 0, 0, 0};
	} else if (piece == tetris_piece::L || piece == tetris_piece::J) {
		if (orientation == 1 || orientation == 3) {
			return {2, 3, 5, 6, 7, 8, 0, 0};
		} else {
			return {6, 7, 8, 4, 0, 0, 0, 0};
		}
	} else if (piece == tetris_piece::O) {
		return {0, 0, 8, 7, 6, 5, 0, 0};
	} else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return {3, 0, 0, 0, 0, 0, 0, 0};
		}
		return {5, 4, 0, 0, 0, 0, 0, 0};
	} else {
		return {0, 0, 0, 0, 0, 0, 0, 0};
	}
}

struct flatstacking_ai {
	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		static uint64_t number_of_things_filtered_1 = 0;
		static uint64_t number_of_things_filtered_2 = 0;
		static uint64_t number_of_things_filtered_3 = 0;

		uint64_t number_of_things_filtered_1_this_iter = 0;
		uint64_t number_of_things_filtered_2_this_iter = 0;
		uint64_t number_of_things_filtered_3_this_iter = 0;

		ska::bytell_hash_set<compressed_board> seen_boards;
		seen_boards.reserve(15000);

		const auto original_heights = col_heights(game.board.minos);
		const auto covered_sections_count0 = covered_sections(game.board, original_heights);
		const auto covered_slots0 = covered_slots_raw(game.board, original_heights);

		auto res = iterate_board1(
			4, game,
			[&](const tetris_game& game, garbage_calculator calc, int depth) {//next
				const auto prev_heights = col_heights(game.board.minos);
				const auto covered_sections_count = covered_sections(game.board, prev_heights);
				const auto covered_slots = covered_slots_raw(game.board, prev_heights);
				std::vector<next_move_thing> ret;
				ret.reserve(possible_hard_drop_spots[(int)game.current_piece].size());
				for (const auto& a : possible_hard_drop_spots[(int)game.current_piece]) {
					next_move_thing thing;
					if (game.board.can_place_piece_on_board(a.x, 21, piece_offsets[(int)game.current_piece][a.orientation])) {
						thing.garb_state = calc;
						thing.game = game;
						const auto just_placed_piece = game.current_piece;
						thing.game.piece_center_x = (int)a.x;
						thing.game.orientation = (int)a.orientation;
						thing.rotate_stuff = {true, false, false};
						thing.lines_cleared = thing.game.hard_drop();
						const bool is_pc = thing.lines_cleared && thing.game.board.is_empty();

						thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, is_pc);
						if (is_pc) {
							ret.clear();
							ret.push_back(std::move(thing));
							return ret;
						}
						/*
						
						*/
						const auto heights = col_heights(thing.game.board.minos);
						const auto covered_sections2 = covered_sections(thing.game.board, heights);
						const auto extra_sections = covered_sections2 - covered_sections_count;
						if (covered_sections2 - covered_sections_count <= max_allowed_extra_covered_sections(just_placed_piece, a.orientation) &&
							(covered_sections2 - covered_sections_count0 <= std::clamp(4 - depth, 1, 3))
						) {

							const auto extra_covered_slots = covered_slots_raw(thing.game.board, heights) - covered_slots;

							if (extra_sections > 0) {
								const auto disallowed_extra_thingys_ = disallowed_extra_thingys(just_placed_piece, a.orientation, extra_sections);
								if (extra_covered_slots >= 8 || ranges::contains(disallowed_extra_thingys_, extra_covered_slots)) {
									++number_of_things_filtered_3;
									++number_of_things_filtered_3_this_iter;
									continue;
								}
							}

							//seen_boards.insert(compressed_board::from_board(thing.game.board)).second;
							if (!true || seen_boards.insert(compressed_board::from_board(thing.game.board)).second) {
								ret.push_back(std::move(thing));
							}else {
								++number_of_things_filtered_1;
								++number_of_things_filtered_1_this_iter;
							}			
								

						} else {
							++number_of_things_filtered_2;
							++number_of_things_filtered_2_this_iter;
						}
					}
				}

				if (game.current_piece == tetris_piece::T) {
					const auto a = tspin1.match(game.board, 0, 10, 0, 20);
					if (a) {
						const auto [x, y, o] = a.value();
						next_move_thing thing;
						thing.garb_state = calc;
						thing.game = game;
						thing.rotate_stuff = {true, true, false};
						thing.lines_cleared = thing.game.place_current_piece(x, y, o).value();
						//thing.lines_cleared = thing.game.hard_drop();
						thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff, thing.game.board.is_empty());
					}
				}

				return ret;
			},
			[this](const tetris_game& game, garbage_calculator calc, int garb_sent, int lines_cleared) {
				return evaluate_board(game, garb_sent, lines_cleared, calc);
			},
			garbage_state,
			[&](const tetris_game& game)->bool {
				return true;
				//return seen_boards.insert(compressed_board::from_board(game.board)).second;
				const auto ret = seen_boards.insert(compressed_board::from_board(game.board)).second;
				if(!ret) {
					++number_of_things_filtered_1;
					++number_of_things_filtered_1_this_iter;
				}
				return ret;
			},
			[](const tetris_game& prev_game, const tetris_game& next_game, int garbage_sent, garbage_calculator prev_garb_state, garbage_calculator next_garb_state) {
				return true;
			}
		);

		static uint64_t total_seen = 0;
		static int thingys_placed = 0;
		total_seen += seen_boards.size();
		++thingys_placed;
		std::cout << seen_boards.size() << std::endl;
		std::cout << "avg: " << total_seen / thingys_placed << std::endl;
		std::cout << "this_iter1 " << number_of_things_filtered_1_this_iter << std::endl;
		std::cout << "this_iter2 " << number_of_things_filtered_2_this_iter << std::endl;
		std::cout << "this_iter3 " << number_of_things_filtered_3_this_iter << std::endl;
		return res;
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

	static __declspec(noinline) std::tuple<int, int> covered_slots(const tetris_board& board, const std::array<int8_t, 10>& heights) {
		int covered_slots = 0;
		int covered_square = 0;

		for (int x = 0; x < 10; ++x) {
			/*
			const auto things = _mm256_load_si256((const __m256i*) & board.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			auto b = ~(unsigned)_mm256_movemask_epi8(c);

			int i = 0;
			while(b) {
				if (!(b & 1)) {
					const auto slots = std::min(6, heights[x] - i - 1);
					covered_slots += slots;
					covered_square += slots * slots;
				}
				b >>= 1;
				++i;
			}
			*/

			for (int y = 0; y < heights[x]; ++y) {
				if (board.minos[x][y] == tetris_block::empty) {
					const auto slots = std::min(6, heights[x] - y - 1);
					covered_slots += slots;
					covered_square += slots * slots;
				}
			}
		}
		return {covered_slots, covered_square};
	}

	static int covered_sections(const tetris_board& board, const std::array<int8_t, 10>& heights) {
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
			covered_sections += std::popcount((not_empty_slots1 >> 1) & should_be_empty_slots1) + std::popcount((not_empty_slots2 >> 1) & should_be_empty_slots2);
		}


		return covered_sections;
	}

	static int covered_slots_raw(const tetris_board& board, std::array<int8_t, 10> heights) {
		int covered_sections = 0;

		for (int x = 0; x < 10; ++x) {
			const auto things = _mm256_load_si256((const __m256i*)&board.minos[x]);
			const auto zeros = _mm256_setzero_si256();
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			auto b = ~(unsigned)_mm256_movemask_epi8(c);
			covered_sections += heights[x] - std::popcount(b);
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

	static double evaluate_board(const tetris_game& game, int total_garbage_sent, int total_lines_cleared, garbage_calculator garb_state) {
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
		const auto second_deep = second_depth(heights, lowest_height_idx);
		std::array<int8_t, 9> heights_exculding_well = {};
		std::copy(heights.begin(), heights.begin() + lowest_height_idx, heights_exculding_well.begin());
		std::copy(heights.begin() + lowest_height_idx, heights.end(), heights_exculding_well.begin() + lowest_height_idx);

		const auto height_varience =
				ranges::accumulate(heights_exculding_well | ranges::views::transform([](auto a) { return a * a; }), 0) - ranges::accumulate(heights_exculding_well, 0);

		double ret = total_garbage_sent * total_garbage_sent * 5
				- std::abs(bumpines)
				- bumpiness_squared * 2
				- std::abs(covered) * 8
				- covered_squared * 3
				+ total_garbage_sent * 7;

		if (total_lines_cleared && total_garbage_sent == 0) {
			ret -= 12;
		}

		if (total_lines_cleared >= 2 && total_garbage_sent == 1) {
			ret -= 14;
		}

		if (total_garbage_sent > total_lines_cleared) {
			ret += 25;
		}

		if (max_height > 15) {
			ret -= 20;
		} else if (max_height > 10) {
			ret -= 5;
		}

		ret -= height_varience * 1.5;

		const auto depth_diff = max_height - second_deep;
		if (depth_diff >= 6) {
			ret -= depth_diff * 4;
		}

		ret += garb_state.is_b2b * 5;

		const auto well_dep = std::min(lowest_height - second_deep, 6);

		ret += well_dep * 3;
		ret += (total_garbage_sent - total_lines_cleared) * 2;

		ret -= std::abs(max_height - 4) / 2;
		return ret;
	}
};

struct c4w_ai {
	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		const auto heights = col_heights(game.board.minos);

		//ignore garbage?

		if (game.board.is_empty()) {
			return on_empty_board(game, garbage_state);
		} else {
			//
		}
	}

	//returns 0,2,3,6
	int has_residual_down(const tetris_game& game) const noexcept {
		for (int y = 0; y < 20; ++y) {
			if (game.board.minos[3][y] == tetris_block::empty) {
				//;-;
			}
		}
		return 3;
	}

	next_move_thing on_empty_board(const tetris_game& game, garbage_calculator garbage_state) const {
		if (game.preview_pieces.front() == tetris_piece::I) {
			//wat
		} else if (game.preview_pieces.front() == tetris_piece::O) {
			//wat
		} else if (game.preview_pieces.front() == tetris_piece::L) {
			//wat
		} else if (game.preview_pieces.front() == tetris_piece::J) {
			//
		} else if (game.preview_pieces.front() == tetris_piece::T) {
			next_move_thing ret;
			ret.game = game;
			ret.game.piece_center_x = 7;
			ret.game.hard_drop();
			ret.garb_state = garbage_state;
			ret.garbage_sent = ret.garb_state(0, {true, false, false});
			return ret;
		} else if (game.preview_pieces.front() == tetris_piece::Z) {
			next_move_thing ret;
			ret.game = game;
			ret.game.piece_center_x = 7;
			ret.game.hard_drop();
			ret.garb_state = garbage_state;
			ret.garbage_sent = ret.garb_state(0, {true, false, false});
			return ret;
		} else if (game.preview_pieces.front() == tetris_piece::S) {
			next_move_thing ret;
			ret.game = game;
			ret.game.piece_center_x = 4;
			ret.game.hard_drop();
			ret.garb_state = garbage_state;
			ret.garbage_sent = ret.garb_state(0, {true, false, false});
			return ret;
		}
	}

};
