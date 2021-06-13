#pragma once
#include <set>
#include <unordered_set>
#include <tuple>

#include "tetris_stuff.h"
#include <compare>
#include <ranges>
#include "bytell_hash_map.hpp"
#include <mmintrin.h>
#include <immintrin.h>

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
	int lines_cleared = 0;
	int garbage_sent = 0;
	tetris_game game;
	garbage_calculator garb_state;
	bool dead = false;
	rotate_info rotate_stuff;
};

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
		return evaluate(game, prev_garb_state, total_lines_sent,total_lines_cleared);
	}

	double ret = -1293212312;

	if (!game.try_spawn_new_piece()) {
		return ret;
	}

	for (next_move_thing next : get_possible_next(game, prev_garb_state)) {
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
		for (next_move_thing next : get_possible_next(game, prev_garb_state)) {

			if (ply_depth == 1 || (filter2(game,next.game,next.garbage_sent,prev_garb_state,next.garb_state) && filter(next.game))) {
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
		return next_move_thing{0, 0, std::move(game), prev_garb_state, true, {false, false, false}};
	}
	const auto next_ = possible_next(game, prev_garb_state);
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
		const auto next_ = possible_next(game, prev_garb_state);
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
			//return ret;
			/*
			const __m256i thingys = _mm256_set_epi8(
				(char)b.minos[x][0], (char)b.minos[x][1], (char)b.minos[x][2], (char)b.minos[x][3],
				(char)b.minos[x][4], (char)b.minos[x][5], (char)b.minos[x][6], (char)b.minos[x][7],
				(char)b.minos[x][8], (char)b.minos[x][9], (char)b.minos[x][10], (char)b.minos[x][11],
				(char)b.minos[x][12], (char)b.minos[x][13], (char)b.minos[x][14], (char)b.minos[x][15],
				(char)b.minos[x][16], (char)b.minos[x][17], (char)b.minos[x][18], (char)b.minos[x][19],
				(char)b.minos[x][20], (char)b.minos[x][21], (char)b.minos[x][22], (char)b.minos[x][23],
				(char)b.minos[x][24], (char)b.minos[x][25], (char)b.minos[x][26], (char)b.minos[x][27],
				(char)b.minos[x][28], (char)b.minos[x][29], (char)b.minos[x][30], (char)b.minos[x][31]
			);
			const auto res = _mm256_cmpeq_epi8_mask(compare_to, thingys);
			ret.board[x] = _cvtmask32_u32(res);
			*/

			for (int y = 0; y < b.minos[x].size(); ++y) {
				if (b.minos[x][y] != tetris_block::empty) {
					ret.board[x] |= 1 << y;
				}
			}
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



struct flatstacking_ai {
	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		//std::unordered_set<compressed_board> seen_boards;
		//seen_boards.reserve(300);
		ska::bytell_hash_set<compressed_board> seen_boards;
		seen_boards.reserve(60000);
		return iterate_board1(
			3, game,
			[](const tetris_game& game, garbage_calculator calc) {//next
				std::vector<next_move_thing> ret;
				ret.reserve(possible_hard_drop_spots[(int)game.current_piece].size());
				for (const auto& a : possible_hard_drop_spots[(int)game.current_piece]) {
					next_move_thing thing;
					thing.garb_state = calc;
					thing.game = game;
					thing.game.piece_center_x = a.x;
					thing.game.orientation = a.orientation;
					thing.rotate_stuff = {true, false, false};
					thing.lines_cleared = thing.game.hard_drop();
					thing.garbage_sent = thing.garb_state(thing.lines_cleared, thing.rotate_stuff);
					ret.push_back(std::move(thing));
				}
				return ret;
			},
			[this](const tetris_game& game, garbage_calculator calc, int garb_sent,int lines_cleared) {
				return evaluate_board(game, garb_sent, lines_cleared);
			},
			garbage_state,
			[&](const tetris_game& game)->bool {
				return seen_boards.insert(compressed_board::from_board(game.board)).second;
			},
			[](const tetris_game& prev_game,const tetris_game& next_game,int garbage_sent,garbage_calculator prev_garb_state,garbage_calculator next_garb_state) {
				return true;
			}
		);

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

	static std::tuple<int, int> covered_slots(const tetris_board& board, std::array<int8_t, 10> heights) {
		int covered_slots = 0;
		int covered_square = 0;

		for (int x = 0; x < 10; ++x) {
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

	static double evaluate_board(const tetris_game& game, int total_garbage_sent,int total_lines_cleared) {
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
		const auto lowest_height_idx = std::ranges::min_element(heights) - heights.begin();
		const auto lowest_height = *lowest_height_it;
		const auto max_height = *max_height_it;
		const auto [covered, covered_squared] = covered_slots(game.board, heights);
		const auto [bumpines, bumpiness_squared] = bumpiness(game.board, lowest_height_idx, heights);
		double ret = total_garbage_sent * total_garbage_sent - std::abs(bumpines) - bumpiness_squared - std::abs(covered) * 2 - covered_squared * 3;
		if(total_lines_cleared && total_garbage_sent==0) {
			ret -= 5;
		}
		return ret;
	}
};
