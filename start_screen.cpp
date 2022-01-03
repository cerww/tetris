#include"start_screen.h"

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
	}

	window.clear(sf::Color::White);

	m_vs_ai_button.draw(window);
	m_40L_sprint_button.draw(window);
	m_solo_button.draw(window);
	m_watch_ai_button.draw(window);
	m_settings_button.draw(window);
	m_custom_bag_button.draw(window);

	window.display();

	return std::nullopt;
}
