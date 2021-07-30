#include <iostream>
#include <unordered_set>

#include "tetris_stuff.h"
#include "sfml_event_handler.h"
#include "sfml_event_handler_extensions.h"
#include "keyboard_tetris_player.h"
#include <boost/asio.hpp>
#include "ai.h"
#include "ai_arena.h"
#include "tetris_ai_player.h"
#include "good_ai.h"
#include "ref_count_ptr.h"
#include "game_data.h"
#include "screen.h"
#include "draw_game.h"
#include "playing_pvAI_screen.h"

enum struct game_state {
	open,
	transition_to_playing,
	playing,
	dying,
	dead,
	settings,
};

struct starting {
	std::chrono::milliseconds time_to_start = 3s;
};

struct dying_state {
	std::chrono::milliseconds time_till_dead_for_real = 2s;
};

struct dead { };

struct dead_state;

struct playing_pVai_state;


int main() {
	bool do_ai_thing = false;
	if (do_ai_thing) {
		do_ai_thingy();

		return 0;
	}

	boost::asio::io_context executor;
	auto y = std::jthread([&]() {
		auto work_guard = boost::asio::make_work_guard(executor);
		executor.run();
	});


	sf::RenderWindow window(sf::VideoMode(1500, 800), "wat");

	event_handler_t event_handler(window);

	game_data settings;

	window.setFramerateLimit(60);
	static constexpr int awasdasd = sizeof(track_hold_times<>);

	screen_thingy game_state = playing_pVai_state(
		tetris_game_settings{
			.das_time = 50ms,
			.arr_time = 8ms,
			.delay_between_drops = 5ms,
			.garbage_delay = 20ms,
			.max_soft_dropping_time = 5s,
			.soft_drop_multiplier = 50
		},
		ai_settings{
			.piece_delay = 1ms
		},
		executor.get_executor()

	);

	while (window.isOpen()) {
		event_handler.poll_stuff();

		auto next_state = game_state.update(event_handler, settings);
		if (next_state.has_value()) {
			game_state = std::move(*next_state);

		}
		
		window.display();
	}

	return 0;
}
