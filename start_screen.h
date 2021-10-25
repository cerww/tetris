#pragma once
#include "screen.h"
#include "playing_pvAI_screen.h"
#include "ui_thingy.h"




struct start_screen {

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& settings) {
		auto& window = event_handler.window();
		window.clear(sf::Color::White);
		
		window.display();		
		
		return std::nullopt;
	}

private:
	
};



