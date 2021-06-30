#pragma once
#include <fmt/core.h>
#include <vector>
#include <span>
#include <string>
#include <array>
#include <string_view>
#include <optional>
#include <ranges>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <numeric>
#include <range/v3/all.hpp>
#include <immintrin.h>

#include "rotation_table.h"
#include "sbo_vector.h"
#include <bit>

using namespace std::literals;


template<typename I, typename S>
struct subrange {
	subrange() = default;

	subrange(I t_b, S t_e) :
		b(std::move(t_b)), e(std::move(t_e)) {}

	I b;
	S e;

	I begin() const noexcept {
		return b;
	}

	S end() const noexcept {
		return e;
	}
};

enum struct tetris_block :int8_t {
	empty,
	orange,
	blue,
	yellow,
	light_blue,
	purple,
	red,
	green,
	garbage,
	dead
};

enum struct tetris_piece :int8_t {
	L,
	J,
	O,
	I,
	T,
	Z,
	S,
	no_piece
};

struct tetris_board {
	alignas(32) std::array<std::array<tetris_block, 32>, 10> minos = {};

	bool can_place_piece_on_board(int x, int y, std::span<const std::pair<int, int>> piece_offsets) const noexcept {
		return std::ranges::all_of(piece_offsets, [&](std::pair<int, int> offset) {
			const int new_x = x + offset.first;
			const int new_y = y + offset.second;

			return new_x >= 0 && new_x < 10 && new_y >= 0 && new_y <= 22 &&
					minos[new_x][new_y] == tetris_block::empty;
		});
	}

	bool can_place_piece_on_board(int x, int y, const std::array<std::pair<int8_t, int8_t>, 4>& piece_offsets) const noexcept {

		return  (((x + piece_offsets[0].first) >= 0) && ((x + piece_offsets[0].first) < 10) && (y + piece_offsets[0].second >= 0) && (y + piece_offsets[0].second <= 22) &&
					(minos[(x + piece_offsets[0].first)][(y + piece_offsets[0].second)] == tetris_block::empty)) &&
				(((x + piece_offsets[1].first) >= 0) && ((x + piece_offsets[1].first) < 10) && (y + piece_offsets[1].second >= 0) && (y + piece_offsets[1].second <= 22) &&
					(minos[(x + piece_offsets[1].first)][(y + piece_offsets[1].second)] == tetris_block::empty)) &&
				(((x + piece_offsets[2].first) >= 0) && ((x + piece_offsets[2].first) < 10) && (y + piece_offsets[2].second >= 0) && (y + piece_offsets[2].second <= 22) &&
					(minos[(x + piece_offsets[2].first)][(y + piece_offsets[2].second)] == tetris_block::empty)) &&
				(((x + piece_offsets[3].first) >= 0) && ((x + piece_offsets[3].first) < 10) && (y + piece_offsets[3].second >= 0) && (y + piece_offsets[3].second <= 22) &&
					(minos[(x + piece_offsets[3].first)][(y + piece_offsets[3].second)] == tetris_block::empty));


	}

	void place_pieces(int x, int y, std::span<const std::pair<int8_t, int8_t>> piece_offsets, tetris_block block_color) {
		for (const auto offset : piece_offsets) {
			minos[x + offset.first][y + offset.second] = block_color;
		}
	}

	bool is_empty() const noexcept {
		return minos == decltype(minos){};
	}

};

constexpr static std::array<std::array<std::array<std::pair<int8_t, int8_t>, 4>, 4>, 7> piece_offsets = {
	{
		{
			{//L
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{-1, 0}, std::pair{1, 0}
				},
				{
					std::pair{0, 0}, std::pair{0, 1}, std::pair{1, -1}, std::pair{0, -1}
				},
				{
					std::pair{0, 0}, std::pair{-1, -1}, std::pair{-1, 0}, std::pair{1, 0}
				},
				{
					std::pair{0, 0}, std::pair{0, 1}, std::pair{-1, 1}, std::pair{0, -1}
				},
			}
		},
		{
			{//J
				{
					std::pair{0, 0}, std::pair{-1, 1}, std::pair{-1, 0}, std::pair{1, 0}
				},
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{0, 1}, std::pair{0, -1}
				},
				{
					std::pair{0, 0}, std::pair{1, -1}, std::pair{-1, 0}, std::pair{1, 0}
				},
				{
					std::pair{0, 0}, std::pair{-1, -1}, std::pair{0, 1}, std::pair{0, -1}
				},
			}
		},
		{
			{//O,"middle" is bottom left
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{1, 0}, std::pair{0, 1}
				},
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{1, 0}, std::pair{0, 1}
				},
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{1, 0}, std::pair{0, 1}
				},
				{
					std::pair{0, 0}, std::pair{1, 1}, std::pair{1, 0}, std::pair{0, 1}
				},
			}
		},
		{
			{//I
				{
					std::pair{0, 0}, std::pair{-1, 0}, std::pair{1, 0}, std::pair{2, 0}
				},
				{
					std::pair{1, 0}, std::pair{1, -1}, std::pair{1, 1}, std::pair{1, -2}
				},
				{
					std::pair{0, 0}, std::pair{-1, 0}, std::pair{1, 0}, std::pair{2, 0}
				},
				{
					std::pair{0, 0}, std::pair{0, -1}, std::pair{0, 1}, std::pair{0, -2}
				},
			}
		},
		{
			{//T
				{
					std::pair{0, 0}, std::pair{1, 0}, std::pair{-1, 0}, std::pair{0, 1}
				},
				{
					std::pair{0, 0}, std::pair{1, 0}, std::pair{0, -1}, std::pair{0, 1}
				},
				{
					std::pair{0, 0}, std::pair{1, 0}, std::pair{-1, 0}, std::pair{0, -1}
				},
				{
					std::pair{0, 0}, std::pair{-1, 0}, std::pair{0, 1}, std::pair{0, -1}
				},
			}
		},
		{
			{//Z
				{
					std::pair{0, 0}, std::pair{-1, 1}, std::pair{0, 1}, std::pair{1, 0}
				},
				{
					std::pair{0, 0}, std::pair{0, -1}, std::pair{1, 0}, std::pair{1, 1}
				},
				{
					std::pair{0, 0}, std::pair{0, -1}, std::pair{-1, 0}, std::pair{1, -1}
				},
				{
					std::pair{0, 0}, std::pair{0, 1}, std::pair{-1, 0}, std::pair{-1, -1}
				},
			}
		},
		{
			{//S
				{
					std::pair{0, 0}, std::pair{-1, 0}, std::pair{0, 1}, std::pair{1, 1}
				},
				{
					std::pair{0, 0}, std::pair{0, 1}, std::pair{1, 0}, std::pair{1, -1}
				},
				{
					std::pair{0, 0}, std::pair{1, 0}, std::pair{0, -1}, std::pair{-1, -1}
				},
				{
					std::pair{0, 0}, std::pair{-1, 1}, std::pair{-1, 0}, std::pair{0, -1}
				},
			}
		}
	}
};
constexpr auto asudjhasdas = sizeof(std::array<std::array<std::array<std::array<std::pair<int8_t, int8_t>, 5>, 4>, 4>, 7>);

constexpr int get_col_height(std::span<const tetris_block> col) {
	int ret = 20;
	while (ret >= 1 && col[ret - 1] == tetris_block::empty) {
		--ret;
	}
	return ret;
}

inline int get_col_height(const std::array<tetris_block, 32>& col) {
	//const alignas(32) std::array<tetris_block, 32> col = col_;

	const auto things = _mm256_load_si256((const __m256i*)&col);
	const auto zeros = _mm256_setzero_si256();
	const auto c = _mm256_cmpeq_epi8(things, zeros);
	const unsigned thingy1 = _mm256_movemask_epi8(c);
	const auto h = std::countl_zero(~thingy1);
	return 32 - h;

	int ret = 20;
	while (ret >= 1 && col[ret - 1] == tetris_block::empty) {
		--ret;
	}

	return ret;
}

inline std::array<int8_t, 10> col_heights(const std::array<std::array<tetris_block, 32>, 10>& board) {
	std::array<int8_t, 10> heights = {};
	for (int i = 0; i < heights.size(); ++i) {
		heights[i] = (int8_t)get_col_height(board[i]);
	}
	return heights;
}

struct rotate_info {
	bool success = true;
	bool t_spin = false;
	bool t_spin_mini = false;
};

inline int height_start(tetris_piece piece, int orientation, const tetris_board& board, int x) noexcept {
	if (piece == tetris_piece::I) {
		if (orientation == 1) {
			return get_col_height(board.minos[x + 1]) + 2;
		} else if (orientation == 0) {
			return std::max(
				get_col_height(board.minos[x - 1]),
				std::max(get_col_height(board.minos[x]),
						 std::max(get_col_height(board.minos[x + 1]), get_col_height(board.minos[x + 2])))
			);
		} else if (orientation == 2) {
			return std::max(
				get_col_height(board.minos[x - 1]),
				std::max(get_col_height(board.minos[x]),
						 std::max(get_col_height(board.minos[x + 1]), get_col_height(board.minos[x - 2])))
			);
		} else if (orientation == 3) {
			return get_col_height(board.minos[x]) + 2;
		}
	} else if (piece == tetris_piece::J) {
		if (orientation == 0) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 0;
		} else if (orientation == 1) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 2;
		} else if (orientation == 3) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x - 1])) + 1;
		}
	} else if (piece == tetris_piece::O) {
		return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
	} else if (piece == tetris_piece::Z) {
		if (orientation == 0 || orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 1) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 3) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x - 1])) + 1;
		}
	} else if (piece == tetris_piece::S) {
		if (orientation == 0 || orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 1) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 3) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x - 1])) + 1;
		}
	} else if (piece == tetris_piece::T) {
		if (orientation == 0) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 0;
		} else if (orientation == 1) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 3) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x - 1])) + 1;
		}
	} else if (piece == tetris_piece::L) {
		if (orientation == 0) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 0;
		} else if (orientation == 1) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x + 1])) + 1;
		} else if (orientation == 2) {
			return std::max(std::max(get_col_height(board.minos[x - 1]), get_col_height(board.minos[x])), get_col_height(board.minos[x + 1])) + 2;
		} else if (orientation == 3) {
			return std::max(get_col_height(board.minos[x]), get_col_height(board.minos[x - 1])) + 1;
		}
	}

	const auto heights = col_heights(board.minos);
	return *std::ranges::max_element(heights) + 2;
}

struct tetris_game {

	int hard_drop() {
		piece_center_y = std::min(height_start(current_piece,orientation,board,piece_center_x), piece_center_y);

		auto a = drop_piece_1();
		while (!a.has_value()) {
			a = drop_piece_1();
		}
		return *a;
	}

	std::optional<int> drop_piece_1() {
		if (board.can_place_piece_on_board(piece_center_x, piece_center_y - 1, piece_offsets[(int)current_piece][orientation])) {
			--piece_center_y;
			return std::nullopt;
		} else {
			board.place_pieces(piece_center_x, piece_center_y, piece_offsets[(int)current_piece][orientation],
							   tetris_block((int)current_piece + 1));

			return clear_lines();
		}
	}

	bool can_move_piece_down() const noexcept {
		return board.can_place_piece_on_board(piece_center_x, piece_center_y - 1, piece_offsets[(int)current_piece][orientation]);
	}

	rotate_info rotate_left() {
		return rotate_me((orientation + 4 - 1) % 4);
	}

	rotate_info rotate_right() {
		return rotate_me((orientation + 1) % 4);
	}

	rotate_info rotate_180() {
		return {false, false, false};
	}

	bool move_right() {
		if (board.can_place_piece_on_board(piece_center_x + 1, piece_center_y, piece_offsets[(int)current_piece][orientation])) {
			++piece_center_x;
			return true;
		}
		return false;
	}

	bool move_left() {
		if (board.can_place_piece_on_board(piece_center_x - 1, piece_center_y, piece_offsets[(int)current_piece][orientation])) {
			--piece_center_x;
			return true;
		}
		return false;
	}

	int clear_lines() {
		
		auto row = [&](int y) {
			return ranges::views::iota(0, 10) | ranges::views::transform([&, y_ = y](int n)->auto& {
				return board.minos[n][y_];
			});
		};

		const auto zeros = _mm256_setzero_si256();

		uint32_t compressed_cols = 0xFFFFFFFF;
		for (int x = 0; x < 10 && compressed_cols; ++x) {
			const auto things = _mm256_load_si256((const __m256i*) & board.minos[x]);
			const auto c = _mm256_cmpeq_epi8(things, zeros);
			auto b = ~(unsigned)_mm256_movemask_epi8(c);
			compressed_cols &= b;
		}
		const int number_of_lines_cleared = std::popcount(compressed_cols);
		if(compressed_cols) {
			const auto y_start = std::countr_zero(compressed_cols);
			compressed_cols >>=y_start;
			for(int y = y_start;y<32 && compressed_cols;(++y,compressed_cols >>=1)) {
				if(compressed_cols &1) {
					std::ranges::fill(row(y), tetris_block::dead);
				}				
			}
			for (auto& column : board.minos) {
				auto thing = std::ranges::remove_if(column, [](tetris_block a) { return a == tetris_block::dead; });
				std::ranges::fill(thing, tetris_block::empty);
			}
		}
		return number_of_lines_cleared;
		/*
		std::array<int8_t, 10> heights = {};
		for (int i = 0; i < heights.size(); ++i) {
			heights[i] = (int8_t)get_col_height(board.minos[i]);
		}
		
		const auto min_height = *std::ranges::min_element(heights);

		for (int y = 0; y < min_height; ++y) {
			if (std::ranges::all_of(row(y), [](tetris_block a) { return a != tetris_block::empty; })) {
				++number_of_lines_cleared;
				std::ranges::fill(row(y), tetris_block::dead);
			}
		}
		if (number_of_lines_cleared) {
			for (auto& column : board.minos) {
				auto thing = std::ranges::remove_if(column, [](tetris_block a) { return a == tetris_block::dead; });
				std::ranges::fill(thing, tetris_block::empty);
			}
		}
		return number_of_lines_cleared;
		*/
	}

	bool try_spawn_new_piece() {
		current_piece = preview_pieces.front();
		piece_center_x = 4;
		piece_center_y = 21;
		preview_pieces.erase(preview_pieces.begin());
		orientation = 0;

		return board.can_place_piece_on_board(4, 21, piece_offsets[(int)current_piece][0]);
	}

	std::optional<int> place_current_piece(int x, int y, int orientation_) {
		if (board.can_place_piece_on_board(x, y, piece_offsets[(int)current_piece][orientation_])) {
			board.place_pieces(x, y, piece_offsets[(int)current_piece][orientation_], tetris_block((int)current_piece + 1));
			return clear_lines();
		}
		return std::nullopt;
	}

	void generate_new_pieces(std::mt19937& engine) {
		std::array<tetris_piece, 7> pieces = {tetris_piece::I, tetris_piece::J, tetris_piece::T, tetris_piece::Z, tetris_piece::S, tetris_piece::L, tetris_piece::O};
		std::ranges::shuffle(pieces, engine);
		ranges::push_back(preview_pieces, pieces);
	}

	template<size_t N>
	void generate_new_pieces(std::mt19937& engine, std::array<tetris_piece, N> pieces) {
		std::ranges::shuffle(pieces, engine);
		ranges::push_back(preview_pieces, pieces);
	}

	bool swap_held_piece() {
		//requires piece to have spawned already
		if (held_piece) {
			std::swap(*held_piece, current_piece);
			orientation = 0;

			piece_center_x = 4;
			piece_center_y = 21;
			return board.can_place_piece_on_board(4, 21, piece_offsets[(int)current_piece][0]);
		} else {
			held_piece = current_piece;
			return try_spawn_new_piece();
		}
	}

	void spawn_garbage(std::span<const int> garbage_holes) {
		auto row = [&](int y) {
			return ranges::views::iota(0, 10) | ranges::views::transform([&,y_ = y](int n)->auto& {
				return board.minos[n][y_];
			});
		};

		for (auto& column : board.minos) {
			std::shift_right(column.begin(), column.end(), garbage_holes.size());
		}

		for (auto [i, hole_idx] : ranges::views::enumerate(garbage_holes)) {
			std::ranges::fill(row((int)i), tetris_block::garbage);
			row((int)i)[hole_idx] = tetris_block::empty;
		}
	}

	int get_ghost_piece_y() const noexcept {
		int ret_y = piece_center_y;
		while (board.can_place_piece_on_board(piece_center_x, ret_y - 1, piece_offsets[(int)current_piece][orientation])) {
			--ret_y;
		}
		return ret_y;
	}

	rotate_info rotate_me(int new_orientation) {
		if (board.can_place_piece_on_board(piece_center_x, piece_center_y, piece_offsets[(int)current_piece][new_orientation])) {
			orientation = new_orientation;
			if (current_piece == tetris_piece::T) {
				return {true, is_tspin(), false};
			} else {
				return {true, false, false};
			}
		}
		const auto new_centers_to_test = rotations_offsets_to_test((int)current_piece, orientation, new_orientation);

		for (auto [x_offset, y_offset] : new_centers_to_test) {
			if (board.can_place_piece_on_board(piece_center_x + x_offset, piece_center_y + y_offset, piece_offsets[(int)current_piece][new_orientation])) {
				orientation = new_orientation;
				piece_center_x += x_offset;
				piece_center_y += y_offset;
				if (current_piece == tetris_piece::T) {
					return {true, is_tspin(), false};
				} else {
					return {true, false, false};
				}
			}
		}

		return {false, false};
	}

	tetris_board board = {};
	sbo_vector<tetris_piece, 16> preview_pieces;
	tetris_piece current_piece;
	int orientation = 0;

	int piece_center_x = 0;
	int piece_center_y = 0;
	std::optional<tetris_piece> held_piece;


private:
	bool is_tspin() const noexcept {
		return (
			(piece_center_y + 1 < 22 && piece_center_x + 1 < 10
				 ? (board.minos[piece_center_x + 1][piece_center_y + 1] != tetris_block::empty)
				 : 1) +
			(piece_center_y + 1 < 22 && piece_center_x - 1 >= 0
				 ? (board.minos[piece_center_x - 1][piece_center_y + 1] != tetris_block::empty)
				 : 1) +
			(piece_center_y - 1 >= 0 && piece_center_x + 1 < 10
				 ? (board.minos[piece_center_x + 1][piece_center_y - 1] != tetris_block::empty)
				 : 1) +
			(piece_center_y - 1 >= 0 && piece_center_x - 1 >= 0
				 ? (board.minos[piece_center_x - 1][piece_center_y - 1] != tetris_block::empty)
				 : 1)) >= 3;
	}
};

constexpr int auwiwekjyhasjdasd = sizeof(tetris_game);

struct tetris_game_update {
	tetris_game game_state;
	int garbage_sent = 0;
	int current_combo = -1;
	int garbage_recieving = 0;
	bool died = false;
};

enum struct action {
	soft_drop,
	hard_drop,
	move_left,
	move_right,
	rotate_right,
	rotate_left,
	rotate_180,
	swap_held_piece,
	size
};

struct keyboard_update {
	std::array<bool, (int)action::size> actions_held;
	std::array<bool, (int)action::size> new_actions_pressed;
	std::chrono::milliseconds time_since_last_update = 0ms;
};

struct tetris_game_settings {
	std::chrono::milliseconds das_time = 70ms;
	std::chrono::milliseconds arr_time = 14ms;
	std::chrono::milliseconds delay_between_drops = 0ms;
	std::chrono::milliseconds garbage_delay = 30ms;
	std::chrono::milliseconds max_soft_dropping_time = 5s;
	int soft_drop_multiplier = 3;
};


struct garbage_calculator {
	static constexpr std::array<int, 15> combo_table = {
		0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5
	};

	int operator()(int lines_cleared, rotate_info last_rotation, bool is_pc = false) {
		if (lines_cleared == 0) {
			//is_b2b = false;
			current_combo = -1;
			return 0;
		}
		++current_combo;

		if (lines_cleared == 1) {
			if (last_rotation.t_spin || last_rotation.t_spin_mini) {
				return 2 + std::exchange(is_b2b, true) + combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			} else {
				is_b2b = false;
				return combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			}
		} else if (lines_cleared == 2) {
			if (last_rotation.t_spin || last_rotation.t_spin_mini) {
				return 4 + std::exchange(is_b2b, true) + combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			} else {
				is_b2b = false;
				return 1 + combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			}
		} else if (lines_cleared == 3) {
			if (last_rotation.t_spin || last_rotation.t_spin_mini) {
				return 6 + std::exchange(is_b2b, true) + combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			} else {
				is_b2b = false;
				return combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + 2 + is_pc * 10;
			}
		} else if (lines_cleared == 4) {
			if (last_rotation.t_spin || last_rotation.t_spin_mini) {
				return 8 + std::exchange(is_b2b, true) + combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + is_pc * 10;
			} else {
				is_b2b = true;
				return combo_table[std::min((int)current_combo, (int)combo_table.size() - 1)] + 4 + is_pc * 10;
			}
		}

		return 0;
	}

	bool is_b2b = false;
	int8_t current_combo = -1;

};









