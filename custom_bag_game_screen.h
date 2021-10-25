#pragma once
#include "screen.h"
#include "playing_pvAI_screen.h"


struct custom_bag_game_screen_prev_screen {
	explicit custom_bag_game_screen_prev_screen(tetris_game_settings player_settings, std::vector<tetris_piece> bag):
		m_player_settings(std::move(player_settings)),
		m_bag(std::move(bag)) { }

	explicit custom_bag_game_screen_prev_screen(tetris_game_settings player_settings) :
		m_player_settings(std::move(player_settings)){ }

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings);
private:
	tetris_game_settings m_player_settings;
	std::vector<tetris_piece> m_bag = { tetris_piece::I,tetris_piece::L,tetris_piece::J,tetris_piece::O,tetris_piece::Z,tetris_piece::S,tetris_piece::T };
	
};



struct custom_bag_game_screen {
	explicit custom_bag_game_screen(tetris_game_settings player_settings, std::vector<tetris_piece> bag) { }

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings) {
		auto& window = event_handler.window();
		auto& player = *player_ptr;


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
			} else if (amount > 0) {
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
		if (game_update.died) {
			player.stop_doing_stuff();

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);

			//return dead_state(std::move(player_ptr), std::move(ai_player_ptr));
		} else {
			player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, 0);

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);

			//const auto heights = col_heights(game_update.game_state.board.minos);
		}
		return std::nullopt;
	}

private:
	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;

};



inline std::optional<screen_thingy> custom_bag_game_screen_prev_screen::update(event_handler_t& event_handler, game_data& settings) {
	//start button



	
	return std::nullopt;
}
