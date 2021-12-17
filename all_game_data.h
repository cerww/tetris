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