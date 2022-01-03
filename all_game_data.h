#pragma once
#include "game_data.h"
#include "tetris_ai_player.h"
#include <boost/asio.hpp>

struct all_game_data {
	game_keybinds data;
	tetris_game_settings game_settings;
	ai_settings ai_game_settings;
	sf::Font default_font;
	boost::asio::io_context* ioc;
};


inline std::pair<std::array<bool, (int)action::size>, std::array<bool, (int)action::size>> next_actions_pressed(
	std::array<bool, (int)action::size> prev,event_handler_t& event_handler,game_keybinds& settings) {

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

	for (auto [a, b, c] : ranges::views::zip(prev, actions_this_frame, new_actions_this_frame)) {
		c = b && !a;
	}

	return {actions_this_frame,new_actions_this_frame};
}




