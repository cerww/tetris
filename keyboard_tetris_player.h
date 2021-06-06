#pragma once
#include <iostream>

#include "tetris_stuff.h"

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
			std::accumulate(m_garbage_recieved.begin(), m_garbage_recieved.end(), 0),
			m_is_dead
		};
	}

	void recieve_update(const keyboard_update update, int garbage_recieved) {
		if (!m_is_doing_stuff) {
			return;
		}
		m_garbage_recieved.push_back((int8_t)garbage_recieved);

		if (m_delay_between_pieces.has_value()) {
			m_delay_between_pieces.value() -= update.time_since_last_update;
			if (m_delay_between_pieces.value() <= 0ms) {
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
			if (success) {
				//play sound?
			}
		} else if (update.new_actions_pressed[(int)action::rotate_right]) {
			const auto [success, is_tspin, tspin_mini] = next_game_state.rotate_right();
			m_last_rotation.t_spin = is_tspin && !next_game_state.can_move_piece_down();
			m_last_rotation.t_spin_mini = tspin_mini && !next_game_state.can_move_piece_down();
			if (success) {
				//play sound?
			}
		} else if (update.new_actions_pressed[(int)action::rotate_180]) {
			const auto [success, is_tspin, tspin_mini] = next_game_state.rotate_180();
			m_last_rotation.t_spin = is_tspin && !next_game_state.can_move_piece_down();
			m_last_rotation.t_spin_mini = tspin_mini && !next_game_state.can_move_piece_down();
			if (success) {
				//play sound?
			}
		}
		//movement
		if (update.new_actions_pressed[(int)action::move_left] && update.new_actions_pressed[(int)action::move_right]) {
			//do nothing
		} else if (update.new_actions_pressed[(int)action::move_left]) {
			if (next_game_state.move_left()) {
				m_last_rotation = {m_last_rotation.success, false, false};
				//play sound?
			}
		} else if (update.new_actions_pressed[(int)action::move_right]) {
			if (next_game_state.move_right()) {
				m_last_rotation = {m_last_rotation.success, false, false};
				//play sound?
			}
		} else {//didn't just press a move_buttom
			if (update.actions_held[(int)action::move_left] && update.actions_held[(int)action::move_right]) {
				//do nothing
			} else if (update.actions_held[(int)action::move_left]) {
				const auto time_to_sub_left = std::min(update.time_since_last_update, m_das_time_left);
				m_das_time_left -= time_to_sub_left;
				if (m_das_time_left <= 0ms) {
					m_arr_time_left -= update.time_since_last_update - time_to_sub_left;
					while (m_arr_time_left <= 0ms) {
						if (next_game_state.move_left()) {
							//play sound?
						}
						m_arr_time_left += m_settings.arr_time;
					}
				}
			} else if (update.actions_held[(int)action::move_right]) {
				const auto time_to_sub_left = std::min(update.time_since_last_update, m_das_time_left);
				m_das_time_left -= time_to_sub_left;
				if (m_das_time_left <= 0ms) {
					m_arr_time_left -= update.time_since_last_update - time_to_sub_left;
					while (m_arr_time_left <= 0ms) {
						next_game_state.move_right();
						m_arr_time_left += m_settings.arr_time;
					}
				}
			} else {
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
			garbage_sent_this_update = on_piece_lock(lines_cleared, m_last_rotation, next_game_state);
		} else if (update.actions_held[(int)action::soft_drop]) {
			if (next_game_state.can_move_piece_down()) {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update) * m_settings.soft_drop_multiplier;
			} else {
				m_soft_drop_timing -= std::chrono::microseconds(update.time_since_last_update) / 2;
				m_max_soft_dropping_time -= update.time_since_last_update;
			}
		} else {
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
				garbage_sent_this_update += on_piece_lock(lines_cleared.value(), m_last_rotation, next_game_state);
			}
		}
		if (!next_game_state.can_move_piece_down() && m_max_soft_dropping_time <= 0s) {
			const auto lines_cleared = next_game_state.hard_drop();
			if (next_game_state.preview_pieces.size() <= 6) {
				next_game_state.generate_new_pieces(m_random_engine);
			}
			garbage_sent_this_update = on_piece_lock(lines_cleared, m_last_rotation, next_game_state);
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
	int on_piece_lock(int lines_cleared, rotate_info tspin_stuff, tetris_game& state) {
		m_can_swap_held_piece = true;
		auto lines_sent = m_garbage_calculator(lines_cleared, tspin_stuff, state);
		while (lines_sent && !m_garbage_recieved.empty()) {
			const auto amount_to_subtract = std::min(lines_sent, (int)m_garbage_recieved.back());
			m_garbage_recieved.back() -= amount_to_subtract;
			lines_sent -= amount_to_subtract;
			if (m_garbage_recieved.back() <= 0) {
				m_garbage_recieved.pop_back();
			}
		}

		m_delay_between_pieces = m_settings.delay_between_drops;
		m_max_soft_dropping_time = m_settings.max_soft_dropping_time;
		if (!m_garbage_recieved.empty()) {
			if (true) {
				sbo_vector<int, 20> stuff;
				stuff.reserve(std::accumulate(m_garbage_recieved.begin(), m_garbage_recieved.end(), 0));
				//std::cout << stuff.capacity() << std::endl;
				const std::uniform_int_distribution<> dist(0, 9);
				for (auto num_garb : m_garbage_recieved) {
					const auto hole_idx = dist(m_random_engine);
					stuff.resize(stuff.size() + num_garb, hole_idx);
					//std::cout << hole_idx << std::endl;
				}
				m_garbage_recieved.clear();
				state.spawn_garbage(std::span(stuff.data(), std::min(32, (int)stuff.size())));
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

	sbo_vector<int8_t, 20> m_garbage_recieved;
	rotate_info m_last_rotation = {};

	garbage_calculator m_garbage_calculator;

	std::chrono::milliseconds m_das_time_left = m_settings.das_time;
	std::chrono::milliseconds m_arr_time_left = m_settings.arr_time;
	std::optional<std::chrono::milliseconds> m_delay_between_pieces;

	std::chrono::microseconds m_soft_drop_timing = 1s;
	std::chrono::milliseconds m_max_soft_dropping_time = m_settings.max_soft_dropping_time;


	std::mutex m_mut;
};
