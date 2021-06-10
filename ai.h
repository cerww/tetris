#pragma once
#include <set>
#include <unordered_set>
#include <tuple>

#include "tetris_stuff.h"
#include <compare>
#include <ranges>


constexpr int get_col_height(std::span<const tetris_block> col) {
	int ret = 20;
	while (ret >= 1 && col[ret] == tetris_block::empty) {
		--ret;
	}
	return ret;
}

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

inline std::array<sbo_vector<piece_hard_drop_spot, 30>, 7> possible_hard_drop_spots = {
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
	rotate_info rotate_stuff;
	std::tuple<int, int, int> where;//x,y,orientation
	int lines_cleared = 0;
	int garbage_sent = 0;
	garbage_calculator garb_state;
	bool dead = false;
};

template<typename P, typename E, typename F>
double iterate_board(
	int ply_depth,
	tetris_game game,
	P&& get_possible_next,
	E&& evaluate,
	garbage_calculator prev_garb_state,
	int total_lines_sent,
	F&& filter
) {

	if (!ply_depth) {
		return evaluate(game, prev_garb_state, total_lines_sent);
	}

	double ret = -1293212312;

	if (!game.try_spawn_new_piece()) {
		return ret;
	}

	for (next_move_thing next : get_possible_next(game, prev_garb_state)) {
		if (filter(next.game)) {
			const auto val = iterate_board(ply_depth - 1, std::move(next.game), get_possible_next, evaluate, next.garb_state, total_lines_sent + next.garbage_sent, filter);
			ret = std::max(ret, val);
		}
	}
	if (game.held_piece != game.current_piece) {
		game.swap_held_piece();
		for (next_move_thing next : get_possible_next(game, prev_garb_state)) {
			if (filter(next.game)) {
				const auto val = iterate_board(ply_depth - 1, std::move(next.game), get_possible_next, evaluate, next.garb_state, total_lines_sent + next.garbage_sent, filter);
				ret = std::max(ret, val);
			}
		}
	}
	return ret;
}

template<typename P, typename E, typename F>
next_move_thing iterate_board1(
	int ply_depth_init,
	tetris_game game,
	P&& possible_next,
	E&& evaluate,
	garbage_calculator prev_garb_state,
	F&& filter
) {

	double ret_val = -129321312312;
	next_move_thing ret_move;


	if (!game.try_spawn_new_piece()) {
		return next_move_thing{std::move(game), {false, false, false}, {}, 0, 0, prev_garb_state, true};
	}
	const auto next_ = possible_next(game, prev_garb_state);
	for (next_move_thing next : next_) {
		if (filter(next.game)) {
			const auto val = iterate_board(ply_depth_init - 1, next.game, possible_next, evaluate, next.garb_state, 0 + next.garbage_sent, filter);
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
				const auto val = iterate_board(ply_depth_init - 1, next.game, possible_next, evaluate, next.garb_state, 0 + next.garbage_sent, filter);
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
		for (int x = 0; x < 10; ++x) {
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
		return std::hash<std::string_view>()(std::string_view((char*)board.board.data(), (char*)board.board.data() + 40));
	}
};

struct flatstacking_ai {

	struct data_i_care_about {
		bool is_alive = true;
		int num_holes = 0;
		int height = 0;
		int garbage_height = 0;
		double varience = 0;
		int garbage_sent = 0;
		int covered_squares = 0;
		int covered_squares_squared = 0;

		auto operator<=>(const data_i_care_about& other) const noexcept = default;

		double to_value_i_care_about() const noexcept {
			return 100 + is_alive * (garbage_sent - std::abs(height - 7) - num_holes - varience - garbage_height);
		}

	};

	next_move_thing operator()(const tetris_game& game, garbage_calculator garbage_state, int garbage_receiving/*ignore this var for this fn*/) const {
		std::unordered_set<compressed_board> seen_boards;
		seen_boards.reserve(300);
		return iterate_board1(
			4, game,
			[](const tetris_game& game, garbage_calculator calc) {//next
				std::vector<next_move_thing> ret;
				ret.reserve(10);
				for (const auto& a : possible_hard_drop_spots[(int)game.current_piece]) {
					//std::cout << (int)a.x << ' ' << (int)a.orientation << std::endl;
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
			[this](const tetris_game& game, garbage_calculator calc, int garb_sent) {
				return evaluate_board(game, garb_sent);
			},
			garbage_state,
			[&](const tetris_game& game) {
				return seen_boards.insert(compressed_board::from_board(game.board)).second;
			}
		);

	}

	static std::tuple<int, int> bumpiness(const tetris_board& board, int well_idx, std::array<int, 10> heights) {
		std::shift_left(heights.begin() + well_idx, heights.end(), 1);
		int b = 0;
		int bumpiness_squared = 0;
		std::adjacent_difference(heights.begin(), heights.end() - 1, heights.begin());
		for (int height_diff : std::span(heights.begin() + 1, heights.end() - 1)) {
			b += height_diff;
			bumpiness_squared += height_diff * height_diff;
		}
		//std::cout << bumpiness_squared << std::endl;
		return {b, bumpiness_squared};
	}

	static std::tuple<int, int> covered_slots(const tetris_board& board, std::array<int, 10> heights) {
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
		//std::cout << covered_slots << std::endl;

		return {covered_slots, covered_square};
	}

	static double evaluate_board(const tetris_game& game, int total_garbage_sent) {
		const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
			return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
				return game.board.minos[x][y];
			});
		});

		std::array<int, 10> heights = {};
		//ranges::copy(game.board.minos | ranges::views::transform([](auto& a) { return get_col_height(a); }), heights);
		for (int i = 0; i < heights.size(); ++i) {
			heights[i] = get_col_height(game.board.minos[i]);
			//std::cout << heights[i] << std::endl;
		}

		const auto [lowest_height_it, max_height_it] = std::ranges::minmax_element(heights);
		const auto lowest_height_idx = std::ranges::min_element(heights) - heights.begin();
		const auto lowest_height = *lowest_height_it;
		const auto max_height = *max_height_it;
		const auto [covered, covered_squared] = covered_slots(game.board, heights);
		const auto [bumpines, bumpiness_squared] = bumpiness(game.board, lowest_height_idx, heights);


		return total_garbage_sent * total_garbage_sent - std::abs(bumpines) - bumpiness_squared - std::abs(covered) - covered_squared;
	}


};
