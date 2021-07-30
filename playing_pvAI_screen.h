#pragma once
#include <array>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window.hpp>
#include <boost/asio.hpp>


#include "draw_game.h"
#include "keyboard_tetris_player.h"
#include "screen.h"
#include "tetris_ai_player.h"
#include "tetris_stuff.h"
#include "game_data.h"


struct dead_state {
	dead_state(std::unique_ptr<tetris_game_keyboard_player> player, std::unique_ptr<tetris_ai_player> ai_player) :
		player_ptr(std::move(player)),
		ai_player_ptr(std::move(ai_player)) {}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings);

	std::chrono::milliseconds time_till_playing = 2s;

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;
};



struct playing_pVai_state {
	playing_pVai_state(tetris_game_settings player_settings, ai_settings ai_setting, boost::asio::any_io_executor executor) :
		player_ptr(std::make_unique<tetris_game_keyboard_player>(player_settings)),
		ai_player_ptr(std::make_unique<tetris_ai_player>(executor, ai_setting)) {
		player_ptr->start_doing_stuff_now();
		ai_player_ptr->start_doing_stuff();
	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings) {
		auto& window = event_handler.window();
		auto& player = *player_ptr;
		auto& ai_player = *ai_player_ptr;


		std::array<bool, (int)action::size> actions_this_frame = {};
		std::ranges::fill(actions_this_frame, false);

		for (auto [action, key] : settings.keybinds) {
			if (event_handler.is_held(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, key] : settings.joystick_binds) {
			if (event_handler.is_any_pressed(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, axis, amount] : settings.joy_stick_axis) {
			if (amount < 0) {
				if (sf::Joystick::getAxisPosition(0, axis) <= amount) {
					actions_this_frame[(int)action] = true;
				}
			}
			else if (amount > 0) {
				if (sf::Joystick::getAxisPosition(0, axis) >= amount) {
					actions_this_frame[(int)action] = true;
				}
			}
		}
		std::array<bool, (int)action::size> new_actions_this_frame = {};

		for (auto [a, b, c] : ranges::views::zip(actions, actions_this_frame, new_actions_this_frame)) {
			c = b && !a;
		}

		actions = actions_this_frame;

		const auto game_update = player.get_update();
		const auto ai_game_update = ai_player.get_update();
		if (game_update.died || ai_game_update.died) {
			player.stop_doing_stuff();
			ai_player.stop_doing_stuff();
			//state = game_state::open;

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);
			draw_tetris_board(window, ai_game_update, 800, 600);

			return dead_state(std::move(player_ptr), std::move(ai_player_ptr));
		}
		else {
			if (game_update.garbage_sent) {
				std::cout << game_update.garbage_sent << std::endl;
				int garbage_left = game_update.garbage_sent;
				std::ranges::sort(garbage_to_player, std::greater<>(), [](auto& a) { return a.second; });
				while (!garbage_to_player.empty() && garbage_left) {
					const int garb_to_cancel = std::min((int)garbage_left, (int)garbage_to_player.back().first);
					garbage_to_player.back().first -= garb_to_cancel;
					garbage_left -= garb_to_cancel;
					if (garbage_to_player.back().first == 0) {
						garbage_to_player.pop_back();
					}
				}

				if (garbage_left) {
					garbage_to_ai.emplace_back(garbage_left, 1s);
				}
			}

			if (ai_game_update.garbage_sent) {
				int garbage_left = ai_game_update.garbage_sent;
				std::ranges::sort(garbage_to_ai, std::greater<>(), [](auto& a) { return a.second; });
				while (!garbage_to_ai.empty() && garbage_left) {
					const int garb_to_cancel = std::min((int)garbage_left, (int)garbage_to_ai.back().first);
					garbage_to_ai.back().first -= garb_to_cancel;
					garbage_left -= garb_to_cancel;
					if (garbage_to_ai.back().first == 0) {
						garbage_to_ai.pop_back();
					}
				}

				if (garbage_left) {
					garbage_to_player.emplace_back(garbage_left, 1s);
				}
			}

			int garbage_to_ai_this_update = 0;
			int garbage_to_player_this_update = 0;
			if (!garbage_to_ai.empty()) {
				const auto res = std::ranges::partition(garbage_to_ai, [&](auto a) { return a.second > event_handler.time_since_last_poll(); });
				for (auto& a : std::ranges::subrange(garbage_to_ai.begin(), res.begin())) {
					a.second -= event_handler.time_since_last_poll();
				}

				garbage_to_ai_this_update = ranges::accumulate(res | ranges::views::transform([](auto a) { return a.first; }), 0);
				garbage_to_ai.erase(res.begin(), res.end());
			}

			if (!garbage_to_player.empty()) {
				const auto res = std::ranges::partition(garbage_to_player, [&](auto a) { return a.second > event_handler.time_since_last_poll(); });
				for (auto& a : std::ranges::subrange(garbage_to_player.begin(), res.begin())) {
					a.second -= event_handler.time_since_last_poll();
				}

				garbage_to_player_this_update = ranges::accumulate(res | ranges::views::transform([](auto a) { return a.first; }), 0);
				garbage_to_player.erase(res.begin(), res.end());
			}


			//player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, garbage_to_player_this_update);
			//ai_player2.receive_update(event_handler.time_since_last_poll(), garbage_to_ai_this_update + 2 * new_actions_this_frame[7]);
			ai_player.receive_update(event_handler.time_since_last_poll(), garbage_to_ai_this_update + 0 * new_actions_this_frame[7]);

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);
			draw_tetris_board(window, ai_game_update, 800, 600);

			const auto heights = col_heights(game_update.game_state.board.minos);
			//std::cout << flatstacking_ai::covered_sections(game_update.game_state.board, heights)<<std::endl;
		}
		return std::nullopt;
	}


	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;

	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_player;
	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_ai;
};

std::optional<screen_thingy> dead_state::update(event_handler_t& event_handler, game_data& settings) {
	auto& window = event_handler.window();
	auto& player = *player_ptr;
	auto& ai_player = *ai_player_ptr;

	time_till_playing -= event_handler.time_since_last_poll();

	window.clear(sf::Color(100, 100, 100));
	draw_tetris_board(window, player.get_update(), 200, 600);
	draw_tetris_board(window, ai_player.get_update(), 800, 600);

	if (time_till_playing <= 0s) {
		time_till_playing = 2s;
		return screen_thingy(playing_pVai_state(player.settings(), ai_player.settings(), ai_player.get_executor()));
	}
	return std::nullopt;
}








