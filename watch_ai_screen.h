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
#include "start_screen.h"


struct watch_ai_dead {
	/*
	dead_state(std::unique_ptr<tetris_game_keyboard_player> player, std::unique_ptr<tetris_ai_player> ai_player) :
		player_ptr(std::move(player)),
		ai_player_ptr(std::move(ai_player)) {}
	*/

	watch_ai_dead(std::unique_ptr<tetris_ai_player> ai_player, all_game_data& t_game_data) :
		ai_player_ptr(std::move(ai_player)),
		game_data(t_game_data) {}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

	all_game_data& game_data;

	std::chrono::milliseconds time_till_playing = 2s;

	std::unique_ptr<tetris_ai_player> ai_player_ptr;
};



struct watch_ai_screen {
	watch_ai_screen(all_game_data& t_data, boost::asio::any_io_executor executor) :
		//player_ptr(std::make_unique<tetris_game_keyboard_player>(t_data.game_settings)),
		ai_player_ptr(std::make_unique<tetris_ai_player>(std::move(executor), t_data.ai_game_settings)),
		game_data(t_data)
	{

		//player_ptr->start_doing_stuff_now();
		ai_player_ptr->start_doing_stuff();
		sf::Text back_button_text;
		back_button_text.setFont(t_data.default_font);
		back_button_text.setString("Back");
		back_button_text.setFillColor(sf::Color::Red);
		m_back_button.set_text(std::move(back_button_text));

	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);


	//std::array<bool, (int)action::size> actions = {};

	//std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	std::unique_ptr<tetris_ai_player> ai_player_ptr;
	all_game_data& game_data;

	//std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_player;
	std::vector<std::pair<int, std::chrono::milliseconds>> garbage_to_ai;

	boring_button m_back_button = boring_button(0, 10, 200, 100);

};

inline std::optional<screen_thingy> watch_ai_dead::update(event_handler_t& event_handler, game_keybinds& settings) {
	auto& window = event_handler.window();
	auto& ai_player = *ai_player_ptr;

	time_till_playing -= event_handler.time_since_last_poll();

	window.clear(sf::Color(100, 100, 100));
	draw_tetris_board(window, ai_player.get_update(), 800, 600);

	if (time_till_playing <= 0s) {
		time_till_playing = 2s;
		return screen_thingy(watch_ai_screen(game_data, ai_player.get_executor()));
	}
	return std::nullopt;
}


inline std::optional<screen_thingy> watch_ai_screen::update(event_handler_t& event_handler, game_keybinds& settings) {

	if (m_back_button.update(event_handler, event_handler.time_since_last_poll())) {
		return start_screen(game_data);
	}

	auto& window = event_handler.window();
	//auto& player = *player_ptr;
	auto& ai_player = *ai_player_ptr;

	const auto ai_game_update = ai_player.get_update();
	if (ai_game_update.died) {
		ai_player.stop_doing_stuff();
		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, ai_game_update, 800, 600);

		return watch_ai_dead(std::move(ai_player_ptr), game_data);
	} else {		

		//ai_player2.receive_update(event_handler.time_since_last_poll(), garbage_to_ai_this_update + 2 * new_actions_this_frame[7]);
		ai_player.receive_update(event_handler.time_since_last_poll(), 0);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, ai_game_update, 800, 600);
		m_back_button.draw(window);

		//std::cout << flatstacking_ai::covered_sections(game_update.game_state.board, heights)<<std::endl;
	}
	return std::nullopt;
}







