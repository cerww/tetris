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
#include "buttonland.h"


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

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings);


	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;

	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_player;
	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_ai;

	boring_button m_back_button = boring_button(0,10,200,100);
	
};

inline std::optional<screen_thingy> dead_state::update(event_handler_t& event_handler, game_data& settings) {
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

