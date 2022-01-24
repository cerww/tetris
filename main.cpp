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
#include "start_screen.h"
#include "ui_thingy.h"

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

struct print_on_destruction {
	print_on_destruction() = default;

	explicit print_on_destruction(std::string s) :text(std::move(s)) {}

	print_on_destruction(const print_on_destruction&) = default;
	print_on_destruction& operator=(const print_on_destruction&) = default;

	~print_on_destruction() {
		std::cout << text << std::endl;
	}
	std::string text;
};

coroutine_screen wat_screen(all_game_data& game_data) {
	std::array<bool, (int)action::size> actions = {};

	print_on_destruction awe("potatoland");
	int a = 0;
	//auto watmoreland = std::make_unique<tetris_game_keyboard_player>(game_data.game_settings);
	//watmoreland->start_doing_stuff_now();
	boring_button back_button = boring_button(0, 10, 200, 100);
	int i = 0;
	//auto& player = *watmoreland;
	auto watmoreworldland = std::make_unique<decltype(std::declval<tetris_game_keyboard_player>().get_update().game_state)>();
	co_await wait_for_next_update();
	//co_return start_screen(game_data);
	co_return start_screen2(game_data);
	while(true) {
		auto [event_handler, settings] = co_await wait_for_next_update();
		const auto time_step = event_handler.time_since_last_poll();
		if(back_button.update(event_handler,event_handler.time_since_last_poll())) {
			co_return start_screen(game_data);
		}
		auto& window = event_handler.window();
		const auto [actions_this_frame, new_actions_this_frame] = next_actions_pressed(actions, event_handler, settings);

		actions = actions_this_frame;

		//auto game_update = player.get_update();


		//player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, 0);

		window.clear(sf::Color::Red);
		sf::Text wat;
		wat.setFont(game_data.default_font);
		wat.setFillColor(sf::Color::Green);
		wat.setString("aaa");
		wat.setPosition(200, 200);
		window.draw(wat);
		++i;
		if (i == 120) {
			co_return start_screen(game_data);
		}
	}
}


struct a_coroutine {
	struct promise_type {
		std::suspend_never initial_suspend() const noexcept {
			return {};
		}

		std::suspend_always final_suspend() const noexcept {
			return {};
		}

		void return_value() {

		}

		void unhandled_exception() const noexcept {

		}

		std::suspend_always yield_value(int a) {			
			return {};
		}

		a_coroutine get_return_object() {
			const auto handle = std::coroutine_handle<promise_type>::from_promise(*this);
			return a_coroutine(handle);
		}

	};
	std::coroutine_handle<> h;
};

struct potato {
	alignas(32) std::array<std::array<int8_t, 32>, 10> thing = {};
};

a_coroutine coro() {
	int y = 0;
	potato p;
	//decltype(std::declval<tetris_game_keyboard_player>().get_update().game_state.board) watmoreworldland;
	int k = 0;

	co_await std::suspend_always();

}

int main() {
	{
		//auto aaa = coro();
		//aaa.h.destroy();
	}
	std::cout << "watmorelandia" << std::endl;
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

	game_keybinds settings;

	window.setFramerateLimit(60);
	static constexpr int awasdasd = sizeof(track_hold_times<>);

	all_game_data game_data{		
		.game_settings = tetris_game_settings{
			.das_time = 50ms,
			.arr_time = 8ms,
			.delay_between_drops = 5ms,
			.garbage_delay = 20ms,
			.max_soft_dropping_time = 5s,
			.soft_drop_multiplier = 50
		},
		.ai_game_settings = ai_settings{.piece_delay = 1000ms},
	};
	
	game_data.default_font.loadFromFile("arial.ttf");

	game_data.ioc = &executor;


	//screen_thingy game_state = start_screen(game_data);
	screen_thingy game_state = wat_screen(game_data);
	//game_state = wat_screen(game_data);

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
