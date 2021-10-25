#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <tuple>
#include <utility>

#include "tetris_stuff.h"


struct game_data {
	std::vector<std::pair<action, sf::Keyboard::Key>> keybinds = {
		{action::move_left, sf::Keyboard::Left},
		{action::soft_drop, sf::Keyboard::Down},
		{action::hard_drop, sf::Keyboard::Space},
		{action::move_right, sf::Keyboard::Right},
		{action::swap_held_piece, sf::Keyboard::LShift},
		{action::rotate_left, sf::Keyboard::Z},
		{action::rotate_right, sf::Keyboard::X},
	};
	std::vector<std::pair<action, int>> joystick_binds = {
		{action::move_left, 14},
		{action::soft_drop, 13},
		{action::hard_drop, 12},
		{action::move_right, 15},
		{action::swap_held_piece, 4},
		{action::rotate_left, 1},
		{action::rotate_right, 2},
	};

	std::vector<std::tuple<action, sf::Joystick::Axis, float>> joy_stick_axis = {
		{action::move_left, sf::Joystick::Axis::PovX, -0.5f},
		{action::move_right, sf::Joystick::Axis::PovX, 0.5f},
		{action::soft_drop, sf::Joystick::Axis::PovY, -0.5f},
		{action::hard_drop, sf::Joystick::Axis::PovY, 0.5f},
	};


};



