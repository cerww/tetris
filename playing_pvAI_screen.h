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
#include "all_game_data.h"


struct dead_state {
	/*
	dead_state(std::unique_ptr<tetris_game_keyboard_player> player, std::unique_ptr<tetris_ai_player> ai_player) :
		player_ptr(std::move(player)),
		ai_player_ptr(std::move(ai_player)) {}
	*/

	dead_state(std::unique_ptr<tetris_game_keyboard_player> player, std::unique_ptr<tetris_ai_player> ai_player, all_game_data& t_game_data,
		std::optional<std::vector<tetris_piece>> t_custom_bag = std::nullopt) :
		custom_bag(std::move(t_custom_bag)),
		player_ptr(std::move(player)),
		ai_player_ptr(std::move(ai_player)),
		game_data(t_game_data)	{}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

	std::optional<std::vector<tetris_piece>> custom_bag = std::nullopt;

	std::chrono::milliseconds time_till_playing = 2s;

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;
	all_game_data& game_data;
};



struct playing_pVai_state {
	playing_pVai_state(all_game_data& t_data, boost::asio::any_io_executor executor) :
		player_ptr(std::make_unique<tetris_game_keyboard_player>(t_data.game_settings)),
		ai_player_ptr(std::make_unique<tetris_ai_player>(std::move(executor), t_data.ai_game_settings)),
		game_data(t_data)
	{

		player_ptr->start_doing_stuff_now();
		ai_player_ptr->start_doing_stuff();
		sf::Text back_button_text;
		back_button_text.setFont(t_data.default_font);
		back_button_text.setString("Back");
		back_button_text.setFillColor(sf::Color::Red);
		m_back_button.set_text(std::move(back_button_text));
	}

	explicit playing_pVai_state(all_game_data& t_data, boost::asio::any_io_executor executor,std::vector<tetris_piece> player_bag):
		m_custom_bag(player_bag),
		player_ptr(std::make_unique<tetris_game_keyboard_player>(t_data.game_settings, std::move(player_bag))),
		ai_player_ptr(std::make_unique<tetris_ai_player>(std::move(executor), t_data.ai_game_settings)),
		game_data(t_data) {

		player_ptr->start_doing_stuff_now();
		ai_player_ptr->start_doing_stuff();
		sf::Text back_button_text;
		back_button_text.setFont(t_data.default_font);
		back_button_text.setString("Back");
		back_button_text.setFillColor(sf::Color::Red);
		m_back_button.set_text(std::move(back_button_text));
	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

	std::optional<std::vector<tetris_piece>> m_custom_bag = std::nullopt;

	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;
	all_game_data& game_data;

	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_player;
	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_ai;

	boring_button m_back_button = boring_button(0,10,200,100);
	
};

inline std::optional<screen_thingy> dead_state::update(event_handler_t& event_handler, game_keybinds& settings) {
	auto& window = event_handler.window();
	auto& player = *player_ptr;
	auto& ai_player = *ai_player_ptr;

	time_till_playing -= event_handler.time_since_last_poll();

	window.clear(sf::Color(100, 100, 100));
	draw_tetris_board(window, player.get_update(), 200, 600);
	draw_tetris_board(window, ai_player.get_update(), 800, 600);

	if (time_till_playing <= 0s) {
		time_till_playing = 2s;
		if (custom_bag.has_value()) {
			return screen_thingy(playing_pVai_state(game_data, ai_player.get_executor(), std::move(custom_bag.value())));
		} else {
			return screen_thingy(playing_pVai_state(game_data, ai_player.get_executor()));
		}
	}
	return std::nullopt;
}

