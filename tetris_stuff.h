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


#include "rotation_table.h"
#include "sbo_vector.h"

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


	std::array<std::array<tetris_block, 32>, 10> minos = {};


	bool can_place_piece_on_board(int x, int y, std::span<const std::pair<int, int>> piece_offsets) const noexcept {
		return std::ranges::all_of(piece_offsets, [&](std::pair<int, int> offset) {
			const int new_x = x + offset.first;
			const int new_y = y + offset.second;

			return new_x >= 0 && new_x < 10 && new_y >= 0 && new_y <= 22 &&
					minos[new_x][new_y] == tetris_block::empty;
		});
	}

	void place_pieces(int x, int y, std::span<const std::pair<int, int>> piece_offsets, tetris_block block_color) {
		for (const auto offset : piece_offsets) {
			minos[x + offset.first][y + offset.second] = block_color;
		}
	}

};

constexpr static std::array<std::array<std::array<std::pair<int, int>, 4>, 4>, 7> piece_offsets = {
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
			{//O
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


struct rotate_info {
	bool success = true;
	bool t_spin = false;
	bool t_spin_mini = false;
};

struct tetris_game {

	int hard_drop() {
		auto a = drop_piece_1();
		while (!a.has_value()) {
			a = drop_piece_1();
		}
		return *a;
	}

	std::optional<int> drop_piece_1() {
		if (board.can_place_piece_on_board(piece_center_x, piece_center_y-1, piece_offsets[(int)current_piece][orientation])) {
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
		//orientation -1
		const int new_orientation = (orientation + 4 - 1) % 4;
		if (board.can_place_piece_on_board(piece_center_x, piece_center_y, piece_offsets[(int)current_piece][new_orientation])) {
			orientation = new_orientation;
			if (current_piece == tetris_piece::T) {
				return { true, is_tspin(), false };
			}
			else {
				return { true, false, false };
			}
		}
		
		
		const auto new_centers_to_test = rotation_table[(int)current_piece][orientation][new_orientation];

		for(auto [x_offset,y_offset]: new_centers_to_test) {
			if (board.can_place_piece_on_board(piece_center_x + x_offset, piece_center_y + y_offset, piece_offsets[(int)current_piece][new_orientation])) {
				orientation = new_orientation;
				piece_center_x += x_offset;
				piece_center_y += y_offset;
				if (current_piece == tetris_piece::T) {
					return { true, is_tspin(), false };
				}
				else {
					return { true, false, false };
				}
			}
		}
		
		return {false, false};
	}

	rotate_info rotate_right() {
		const int new_orientation = (orientation + 1) % 4;
		if (board.can_place_piece_on_board(piece_center_x, piece_center_y, piece_offsets[(int)current_piece][new_orientation])) {
			orientation = new_orientation;
			if (current_piece == tetris_piece::T) {
				return { true, is_tspin(), false };
			}
			else {
				return { true, false, false };
			}
		}

		
		const auto new_centers_to_test = rotation_table[(int)current_piece][orientation][new_orientation];

		for (auto [x_offset, y_offset] : new_centers_to_test) {
			if (board.can_place_piece_on_board(piece_center_x + x_offset, piece_center_y + y_offset, piece_offsets[(int)current_piece][new_orientation])) {
				orientation = new_orientation;
				piece_center_x += x_offset;
				piece_center_y += y_offset;
				if (current_piece == tetris_piece::T) {
					return { true, is_tspin(), false };
				}
				else {
					return { true, false, false };
				}
			}
		}

		return { false, false };
	}

	rotate_info rotate_180() {
		return {false, false, false};
	}

	bool move_right() {
		if (board.can_place_piece_on_board(piece_center_x + 1, piece_center_y - 1, piece_offsets[(int)current_piece][orientation])) {
			++piece_center_x;
			return true;
		}
		return false;
	}

	bool move_left() {
		if (board.can_place_piece_on_board(piece_center_x - 1, piece_center_y - 1, piece_offsets[(int)current_piece][orientation])) {
			--piece_center_x;
			return true;
		}
		return false;
	}

	int clear_lines() {
		int number_of_lines_cleared = 0;
		auto row = [&](int y) {
			return ranges::views::iota(0, 10) | ranges::views::transform([&,y_=y](int n)->auto& {
				return board.minos[n][y_];
			});
		};
		for (int y = 0; y < board.minos[0].size(); ++y) {
			if (std::ranges::all_of(row(y), [](tetris_block a) { return a != tetris_block::empty; })) {
				++number_of_lines_cleared;
				for (auto& a : row(y)) {
					a = tetris_block::dead;
				}
			}
		}
		for (auto& column : board.minos) {
			auto it = std::stable_partition(column.begin(), column.end(), [](tetris_block a) { return a != tetris_block::dead; });
			for (auto& block : subrange(it, column.end())) {
				block = tetris_block::empty;
			}
		}
		return number_of_lines_cleared;
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

	bool swap_held_piece() {
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
			return ranges::views::iota(0, 10) | ranges::views::transform([&](int n)->auto& {
				return board.minos[n][y];
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
		while (board.can_place_piece_on_board(piece_center_x, ret_y, piece_offsets[(int)current_piece][orientation])) {
			--ret_y;
		}
		return ret_y;
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
		return (piece_center_y + 1 < 20 && piece_center_x + 1 < 10
					? (board.minos[piece_center_x + 1][piece_center_y + 1] != tetris_block::empty)
					: 1 +
					piece_center_y + 1 < 20 && piece_center_x - 1 < 10
					? (board.minos[piece_center_x - 1][piece_center_y + 1] != tetris_block::empty)
					: 1 +
					piece_center_y - 1 >= 0 && piece_center_x + 1 < 10
					? (board.minos[piece_center_x + 1][piece_center_y - 1] != tetris_block::empty)
					: 1 +
					piece_center_y - 1 >= 0 && piece_center_x - 1 >= 0
					? (board.minos[piece_center_x - 1][piece_center_y - 1] != tetris_block::empty)
					: 1) >= 3;
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

	int operator()(int lines_cleared, rotate_info last_rotation, const tetris_game& state) {
		if (lines_cleared == 0) {
			m_is_b2b = false;
			m_current_combo = -1;
			return 0;
		}
		++m_current_combo;

		if (lines_cleared == 1) {
			if (last_rotation.t_spin || last_rotation.t_spin_mini) {
				if (std::exchange(m_is_b2b, true)) { }
			}
		}

	}

	bool m_is_b2b = false;
	int m_current_combo = -1;
};

struct tetris_game_keyboard_player {
	explicit tetris_game_keyboard_player(tetris_game_settings settings) :
		m_random_engine(([]() {
			std::random_device r;
			return r();
		})()),
		m_settings(settings) {
		while (m_game.preview_pieces.size() <= 6) {
			m_game.generate_new_pieces(m_random_engine);
		}
		m_game.try_spawn_new_piece();
	}

	tetris_game_update get_update() {
		std::lock_guard lock(m_mut);
		return {
			m_game,
			std::exchange(m_garbage_sent_since_last_update, 0),
			m_garbage_calculator.m_current_combo,
			std::accumulate(m_garbage_recieved.begin(), m_garbage_recieved.end(),0),
			m_is_dead
		};
	}

	void recieve_update(const keyboard_update update, int garbage_recieved) {
		if (!m_is_doing_stuff) {
			return;
		}
		m_garbage_recieved.push_back(garbage_recieved);
		
		if (m_delay_between_pieces.has_value()) {
			m_delay_between_pieces.value() -= update.time_since_last_update;
			if(m_delay_between_pieces.value() <=0ms) {
				auto next_game_state = m_game;
				const bool still_alive = next_game_state.try_spawn_new_piece();
				m_delay_between_pieces = std::nullopt;

				{
					std::lock_guard lock(m_mut);
					m_game = next_game_state;
					m_is_dead = !still_alive;
				}
			}
			
			return;
		}

		

		auto next_game_state = m_game;
		
		if (update.new_actions_pressed[(int)action::rotate_left] && update.new_actions_pressed[(int)action::rotate_right]) {
			//do nothing!
		} else if (update.new_actions_pressed[(int)action::rotate_left]) {
			const auto [success, is_tspin, tspin_mini] = next_game_state.rotate_left();
			m_last_rotation.t_spin = is_tspin && !next_game_state.can_move_piece_down();
			m_last_rotation.t_spin_mini = tspin_mini && !next_game_state.can_move_piece_down();
		} else if (update.new_actions_pressed[(int)action::rotate_right]) {
			const auto [success, is_tspin, tspin_mini] = next_game_state.rotate_right();
			m_last_rotation.t_spin = is_tspin && !next_game_state.can_move_piece_down();
			m_last_rotation.t_spin_mini = tspin_mini && !next_game_state.can_move_piece_down();
		} else if (update.new_actions_pressed[(int)action::rotate_180]) {
			const auto [success, is_tspin, tspin_mini] = next_game_state.rotate_180();
			m_last_rotation.t_spin = is_tspin && !next_game_state.can_move_piece_down();
			m_last_rotation.t_spin_mini = tspin_mini && !next_game_state.can_move_piece_down();
		}
		//movement
		if (update.new_actions_pressed[(int)action::move_left] && update.new_actions_pressed[(int)action::move_right]) {
			//do nothing
		} else if (update.new_actions_pressed[(int)action::move_left]) {
			if (next_game_state.move_left()) {
				m_last_rotation = {m_last_rotation.success, false, false};
			}
		} else if (update.new_actions_pressed[(int)action::move_right]) {
			if (next_game_state.move_right()) {
				m_last_rotation = {m_last_rotation.success, false, false};
			}
		}else {//didn't just press a move_buttom
			if (update.actions_held[(int)action::move_left] && update.actions_held[(int)action::move_right]) {
				//do nothing
			}
			else if (update.actions_held[(int)action::move_left]) {
				const auto time_to_sub_left = std::min(update.time_since_last_update, m_das_time_left);
				m_das_time_left -= time_to_sub_left;
				if (m_das_time_left <= 0ms) {
					m_arr_time_left -= update.time_since_last_update - time_to_sub_left;
					while (m_arr_time_left <= 0ms) {
						next_game_state.move_left();
						m_arr_time_left += m_settings.arr_time;
					}
				}
			}
			else if (update.actions_held[(int)action::move_right]) {
				const auto time_to_sub_left = std::min(update.time_since_last_update, m_das_time_left);
				m_das_time_left -= time_to_sub_left;
				if (m_das_time_left <= 0ms) {
					m_arr_time_left -= update.time_since_last_update - time_to_sub_left;
					while (m_arr_time_left <= 0ms) {
						next_game_state.move_right();
						m_arr_time_left += m_settings.arr_time;
					}
				}
			}
			else {
				m_das_time_left = m_settings.das_time;
				m_arr_time_left = m_settings.arr_time;
			}
		}

		if (update.new_actions_pressed[(int)action::swap_held_piece] && m_can_swap_held_piece) {
			m_can_swap_held_piece = false;
			next_game_state.swap_held_piece();
			if (next_game_state.preview_pieces.size() <= 6) {
				next_game_state.generate_new_pieces(m_random_engine);
			}
		}

		int garbage_sent_this_update = 0;
		
		if (update.new_actions_pressed[(int)action::hard_drop]) {
			const auto lines_cleared = next_game_state.hard_drop();
			if (next_game_state.preview_pieces.size() <= 6) {
				next_game_state.generate_new_pieces(m_random_engine);
			}
			garbage_sent_this_update = on_line_clear(lines_cleared, m_last_rotation, next_game_state);
		} else if (update.actions_held[(int)action::soft_drop]) {
			if (next_game_state.can_move_piece_down()) {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update) * m_settings.soft_drop_multiplier;
			} else {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update) / 2;
				m_max_soft_dropping_time -= update.time_since_last_update;
			}			
		}else {
			if (next_game_state.can_move_piece_down()) {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update);
			} else {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update) / 2;
				m_max_soft_dropping_time -= update.time_since_last_update;
			}
		}

		while (m_soft_drop_timing <= 0s) {
			m_soft_drop_timing += 1s;
			m_last_rotation.t_spin = m_last_rotation.t_spin_mini = false;
			const auto lines_cleared = next_game_state.drop_piece_1();
			if (lines_cleared.has_value()) {
				if (next_game_state.preview_pieces.size() <= 6) {
					next_game_state.generate_new_pieces(m_random_engine);
				}
				garbage_sent_this_update = on_line_clear(lines_cleared.value(), m_last_rotation, next_game_state);				
			}
		}
		if (!next_game_state.can_move_piece_down() && m_max_soft_dropping_time <= 0s) {
			const auto lines_cleared = next_game_state.hard_drop();
			if (next_game_state.preview_pieces.size() <= 6) {
				next_game_state.generate_new_pieces(m_random_engine);
			}
			garbage_sent_this_update = on_line_clear(lines_cleared, m_last_rotation, next_game_state);
		}

		{
			std::lock_guard lock(m_mut);
			m_game = next_game_state;
			m_garbage_sent_since_last_update += garbage_sent_this_update;
		}
	}

	void start_doing_stuff_now() {
		m_is_doing_stuff = true;
	}

	void stop_doing_stuff() {
		m_is_doing_stuff = false;
	}

private:
	int on_line_clear(int lines_cleared, rotate_info tspin_stuff, tetris_game& state) {
		m_can_swap_held_piece = true;
		const auto lines_sent = m_garbage_calculator(lines_cleared, tspin_stuff, state);
		m_delay_between_pieces = m_settings.delay_between_drops;
		if (!m_garbage_recieved.empty()) {
			if(true) {
				sbo_vector<int, 20> stuff;
				stuff.reserve(std::accumulate(m_garbage_recieved.begin(), m_garbage_recieved.end(), 0));

				const std::uniform_int_distribution<> dist(0, 9);
				for (auto num_garb: m_garbage_recieved) {
					const auto hole_idx = dist(m_random_engine);
					//ranges::push_back(stuff, ranges::views::repeat(hole_idx, num_garb));
					stuff.resize(stuff.size() + num_garb, hole_idx);
				}
				m_garbage_recieved.clear();
				state.spawn_garbage(stuff);
			} else {
				sbo_vector<int, 20> stuff;
				stuff.resize(std::accumulate(m_garbage_recieved.begin(), m_garbage_recieved.end(), 0));

				const std::uniform_int_distribution<> dist(0, 9);
				for (auto& hole_idx : stuff) {
					hole_idx = dist(m_random_engine);
				}
				m_garbage_recieved.clear();
				state.spawn_garbage(stuff);
			}
		}
		return lines_sent;
	}

	bool m_is_doing_stuff = false;
	std::mt19937 m_random_engine;
	tetris_game_settings m_settings;
	tetris_game m_game = {};
	int m_garbage_sent_since_last_update = 0;

	bool m_can_swap_held_piece = true;
	bool m_is_dead = false;

	sbo_vector<int8_t,20> m_garbage_recieved;
	rotate_info m_last_rotation = {};

	garbage_calculator m_garbage_calculator;

	std::chrono::milliseconds m_das_time_left = m_settings.das_time;
	std::chrono::milliseconds m_arr_time_left = m_settings.arr_time;
	std::optional<std::chrono::milliseconds> m_delay_between_pieces;

	std::chrono::microseconds m_soft_drop_timing = 1s;
	std::chrono::milliseconds m_max_soft_dropping_time = m_settings.max_soft_dropping_time;


	std::mutex m_mut;
};
