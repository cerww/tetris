#include"start_screen.h"

#include "cheese_mode.h"
#include "custom_bag_game_screen.h"
#include "playing_solo.h"
#include "playing_pvAI_screen.h"
#include "settings_menu.h"
#include "watch_ai_screen.h"
#include "forty_line_sprint_screen.h"


std::optional<screen_thingy> start_screen::update(event_handler_t& event_handler, game_keybinds& settings) {
	auto& window = event_handler.window();
	if(m_solo_button.update(event_handler,event_handler.time_since_last_poll())){
		return playing_solo_state(m_data);
	} else if(m_40L_sprint_button.update(event_handler, event_handler.time_since_last_poll())){
		//return forty_line_sprint_state(m_data);
		return sprint_mode(m_data, 40);
	} else if (m_watch_ai_button.update(event_handler, event_handler.time_since_last_poll())) {
		return watch_ai_screen(m_data, m_data.ioc->get_executor());
	} else if (m_vs_ai_button.update(event_handler, event_handler.time_since_last_poll())) {
		return playing_pVai_state(m_data,m_data.ioc->get_executor());
	}else if(m_settings_button.update(event_handler,event_handler.time_since_last_poll())) {
		return settings_menu(m_data);
	}else if(m_custom_bag_button.update(event_handler,event_handler.time_since_last_poll())) {
		return custom_bag_game_screen_prev_screen(m_data);
	}else if(m_cheese_mode_button.update(event_handler,event_handler.time_since_last_poll())) {
		return cheese_prev_screen(m_data);
	}

	window.clear(sf::Color::White);

	m_vs_ai_button.draw(window);
	m_40L_sprint_button.draw(window);
	m_solo_button.draw(window);
	m_watch_ai_button.draw(window);
	m_settings_button.draw(window);
	m_custom_bag_button.draw(window);
	m_cheese_mode_button.draw(window);

	window.display();

	return std::nullopt;
}

coroutine_screen start_screen2(all_game_data& data) {
	using namespace button_properties_wat;
	boring_button m_vs_ai_button = boring_button(400, 200, 200, 100);
	boring_button m_40L_sprint_button = boring_button(400, 320, 200, 100);
	boring_button m_solo_button = boring_button(400, 440, 200, 100);
	boring_button m_watch_ai_button = boring_button(400, 560, 200, 100);
	boring_button m_settings_button = boring_button(700, 200, 200, 100);
	boring_button m_custom_bag_button = boring_button(700, 320, 200, 100);
	boring_button m_cheese_mode_button = boring_button(700, 440, 200, 100);

	sf::Text vs_ai_text;
	vs_ai_text.setString("vs ai");
	vs_ai_text.setFillColor(sf::Color::Red);
	vs_ai_text.setFont(data.default_font);
	m_vs_ai_button.set_text(std::move(vs_ai_text));

	sf::Text play_40l_sprint;
	play_40l_sprint.setString("40L sprint");
	play_40l_sprint.setFillColor(sf::Color::Red);
	play_40l_sprint.setFont(data.default_font);
	m_40L_sprint_button.set_text(std::move(play_40l_sprint));

	sf::Text solo_text;
	solo_text.setString("Solo");
	solo_text.setFillColor(sf::Color::Red);
	solo_text.setFont(data.default_font);
	m_solo_button.set_text(std::move(solo_text));

	sf::Text watch_ai;
	watch_ai.setString("watch ai");
	watch_ai.setFillColor(sf::Color::Red);
	watch_ai.setFont(data.default_font);
	m_watch_ai_button.set_text(std::move(watch_ai));

	sf::Text settings_text;
	settings_text.setString("settings");
	settings_text.setFillColor(sf::Color::Red);
	settings_text.setFont(data.default_font);
	m_settings_button.set_text(std::move(settings_text));

	m_custom_bag_button.set_text("custom bag", data.default_font, sf::Color::Red);

	m_cheese_mode_button.set_text("cheese mode", data.default_font, sf::Color::Red);
	

	auto button2 = make_button(600, 200, 200, 100, 
		on_click([]() {std::cout << "watmoreland" << std::endl; }),
		border(sf::Color::Red)
	);
	while (true) {
		auto&& [event_handler, settings] = co_await wait_for_next_update();
		auto& window = event_handler.window();
		if (m_solo_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return playing_solo_state(data);
		}
		else if (m_40L_sprint_button.update(event_handler, event_handler.time_since_last_poll())) {
			//return forty_line_sprint_state(m_data);
			co_return sprint_mode(data, 40);
		}
		else if (m_watch_ai_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return watch_ai_screen(data, data.ioc->get_executor());
		}
		else if (m_vs_ai_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return playing_pVai_state(data, data.ioc->get_executor());
		}
		else if (m_settings_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return settings_menu(data);
		}
		else if (m_custom_bag_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return custom_bag_game_screen_prev_screen(data);
		}
		else if (m_cheese_mode_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return cheese_prev_screen(data);
		}

		button2.update(event_handler);

		window.clear(sf::Color::White);
		button2.draw(window);
		m_vs_ai_button.draw(window);
		m_40L_sprint_button.draw(window);
		m_solo_button.draw(window);
		m_watch_ai_button.draw(window);
		m_settings_button.draw(window);
		m_custom_bag_button.draw(window);
		m_cheese_mode_button.draw(window);

		window.display();
	}

}
