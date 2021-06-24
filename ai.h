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
#include <fstream>


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

	constexpr bool operator==(const piece_hard_drop_spot&) const noexcept = default;

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

	for (next_move_thing next : get_possible_next(game, prev_garb_state, ply_depth,total_lines_cleared,total_lines_sent)) {
		if (ply_depth == 1 || (filter2(game, next.game, next.garbage_sent, prev_garb_state, next.garb_state) && filter(next.game, ply_depth))) {
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
		for (next_move_thing next : get_possible_next(game, prev_garb_state, ply_depth, total_lines_cleared, total_lines_sent)) {

			if (ply_depth == 1 || (filter2(game, next.game, next.garbage_sent, prev_garb_state, next.garb_state) && filter(next.game, ply_depth))) {
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
	std::vector<double> vals;
	vals.reserve(40);
	const auto next_ = possible_next(game, prev_garb_state, ply_depth_init,0,0);
	for (next_move_thing next : next_) {
		if (filter(next.game, ply_depth_init)) {
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
			vals.push_back(val);
			if (val > ret_val) {
				ret_val = val;
				ret_move = std::move(next);
			}
		}
	}
	if (game.held_piece != game.current_piece) {
		game.swap_held_piece();
		const auto next_ = possible_next(game, prev_garb_state, ply_depth_init,0,0);
		for (next_move_thing next : next_) {
			if (filter(next.game, ply_depth_init)) {
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
				vals.push_back(val);
				if (val > ret_val) {
					ret_val = val;
					ret_move = std::move(next);
				}
			}
		}
	}

	std::ranges::sort(vals,std::greater());

	std::cout << ret_val << std::endl;

	std::cout << ranges::views::all(vals) << std::endl;
	return ret_move;
}

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
