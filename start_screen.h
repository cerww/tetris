#pragma once
#include "screen.h"
#include "playing_pvAI_screen.h"
#include "ui_thingy.h"
#include "all_game_data.h"




struct start_screen {
	start_screen(all_game_data& t_data):m_data(t_data) {
		sf::Text vs_ai_text;
		vs_ai_text.setString("vs ai");
		vs_ai_text.setFillColor(sf::Color::Red);
		vs_ai_text.setFont(t_data.default_font);
		m_vs_ai_button.set_text(std::move(vs_ai_text));

		sf::Text play_40l_sprint;
		play_40l_sprint.setString("40L sprint");
		play_40l_sprint.setFillColor(sf::Color::Red);
		play_40l_sprint.setFont(t_data.default_font);
		m_40L_sprint_button.set_text(std::move(play_40l_sprint));

		sf::Text solo_text;
		solo_text.setString("Solo");
		solo_text.setFillColor(sf::Color::Red);
		solo_text.setFont(t_data.default_font);
		m_solo_button.set_text(std::move(solo_text));

		sf::Text watch_ai;
		watch_ai.setString("watch ai");
		watch_ai.setFillColor(sf::Color::Red);
		watch_ai.setFont(t_data.default_font);
		m_watch_ai_button.set_text(std::move(watch_ai));

		sf::Text settings_text;
		settings_text.setString("settings");
		settings_text.setFillColor(sf::Color::Red);
		settings_text.setFont(t_data.default_font);
		m_settings_button.set_text(std::move(settings_text));

		m_custom_bag_button.set_text("custom bag",m_data.default_font,sf::Color::Red);

		m_cheese_mode_button.set_text("cheese mode",m_data.default_font,sf::Color::Red);
	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

private:
	all_game_data& m_data;

	boring_button m_vs_ai_button = boring_button(400,200,200,100);
	boring_button m_40L_sprint_button = boring_button(400,320,200,100);
	boring_button m_solo_button = boring_button(400,440,200,100);
	boring_button m_watch_ai_button = boring_button(400,560,200,100);
	boring_button m_settings_button = boring_button(700,200,200,100);
	boring_button m_custom_bag_button = boring_button(700,320,200,100);
	boring_button m_cheese_mode_button = boring_button(700,440,200,100);

	
};



coroutine_screen start_screen2(all_game_data& data);

