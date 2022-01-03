#pragma once
#include "start_screen.h"
#include "game_data.h"

coroutine_screen sprint_mode(all_game_data& data, int number_of_lines, std::optional<std::vector<tetris_piece>> bag = std::nullopt);

struct dead_forty_line_sprint_state {
	explicit dead_forty_line_sprint_state(
		all_game_data& g, tetris_game_update last_update_since_dead,int number_of_lines,std::chrono::milliseconds time_before_dead,
		std::optional<std::vector<tetris_piece>> bag = std::nullopt
	) : m_number_of_lines(number_of_lines),
		m_time_before_dead(time_before_dead),
		m_custom_bag(std::move(bag)),
		m_data(g),
		m_last_update_since_dead(std::move(last_update_since_dead)) {	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

private:
	int m_number_of_lines = 40;
	std::chrono::milliseconds m_time_before_dead = 0ms;
	std::optional<std::vector<tetris_piece>> m_custom_bag = std::nullopt;
	all_game_data& m_data;
	tetris_game_update m_last_update_since_dead;

	std::chrono::milliseconds m_time_till_not_dead = 3000ms;

};

struct finished_sprint {
	explicit finished_sprint(
		all_game_data& g, tetris_game_update last_update_since_dead, int number_of_lines, std::chrono::milliseconds time_of_finish,
		std::optional<std::vector<tetris_piece>> bag = std::nullopt
	) : m_number_of_lines(number_of_lines),
		m_time_of_finish(time_of_finish),
		m_custom_bag(std::move(bag)),
		m_data(g),
		m_last_update_since_dead(std::move(last_update_since_dead)) {	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);
private:
	int m_number_of_lines = 40;
	std::chrono::milliseconds m_time_of_finish = 0ms;
	std::optional<std::vector<tetris_piece>> m_custom_bag = std::nullopt;
	all_game_data& m_data;
	tetris_game_update m_last_update_since_dead;

	std::chrono::milliseconds m_time_till_not_dead = 3000ms;
};

struct forty_line_sprint_state {
	explicit forty_line_sprint_state(all_game_data& g,int number_of_lines = 40) :m_data(g),
		player_ptr(std::make_unique<tetris_game_keyboard_player>(g.game_settings)),
		m_remaining_lines(number_of_lines),
		m_starting_lines_needed(number_of_lines) {

		sf::Text back_button_text;
		back_button_text.setFont(g.default_font);
		back_button_text.setString("back");
		m_back_button.set_text(std::move(back_button_text));
		player_ptr->start_doing_stuff_now();
	}

	explicit forty_line_sprint_state(all_game_data& g, std::vector<tetris_piece> bag, int number_of_lines = 40) :
		m_custom_bag(bag),
		m_data(g),
		player_ptr(std::make_unique<tetris_game_keyboard_player>(g.game_settings, std::move(bag))),
		m_remaining_lines(number_of_lines),
		m_starting_lines_needed(number_of_lines) {

		sf::Text back_button_text;
		back_button_text.setFont(g.default_font);
		back_button_text.setString("back");
		m_back_button.set_text(std::move(back_button_text));
		player_ptr->start_doing_stuff_now();
	}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings) {
		if (m_back_button.update(event_handler, event_handler.time_since_last_poll())) {
			return start_screen(m_data);
		}

		m_time_since_start += event_handler.time_since_last_poll();

		auto& window = event_handler.window();
		auto& player = *player_ptr;

		const auto [actions_this_frame, new_actions_this_frame] = next_actions_pressed(actions,event_handler,settings);

		actions = actions_this_frame;

		auto game_update = player.get_update();

		if (game_update.died) {
			return dead_forty_line_sprint_state(m_data, std::move(game_update), m_starting_lines_needed, m_time_since_start, std::move(m_custom_bag));
		}

		m_remaining_lines -= game_update.lines_cleared;
		
		if(m_remaining_lines<=0) {
			return finished_sprint(m_data,std::move(game_update),m_starting_lines_needed,m_time_since_start,std::move(m_custom_bag));
		}

		player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, 0);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		m_back_button.draw(window);
		sf::Text time_text;
		time_text.setFont(m_data.default_font);
		time_text.setString(fmt::format("{:.2f}", (double)m_time_since_start.count() / 1000));
		time_text.setFillColor(sf::Color::Green);
		time_text.setPosition(450,600);

		sf::Text pieces_remaining_text;
		pieces_remaining_text.setFont(m_data.default_font);
		pieces_remaining_text.setString(std::to_string(std::max(m_remaining_lines,0)));
		pieces_remaining_text.setFillColor(sf::Color::Green);
		pieces_remaining_text.setPosition(450, 550);

		window.draw(time_text);
		window.draw(pieces_remaining_text);
		return std::nullopt;
	}


private:
	std::optional<std::vector<tetris_piece>> m_custom_bag = std::nullopt;
	all_game_data& m_data;
	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;
	boring_button m_back_button = boring_button(0, 10, 200, 100);
	std::chrono::milliseconds m_time_since_start = 0ms;
	int m_remaining_lines = 40;
	int m_starting_lines_needed = 40;

};

inline std::optional<screen_thingy> dead_forty_line_sprint_state::update(event_handler_t& event_handler, game_keybinds& settings) {
	const auto time_since_last_update = event_handler.time_since_last_poll();
	auto& window = event_handler.window();
	m_time_till_not_dead -= time_since_last_update;
	if (m_time_till_not_dead <= 0ms) {
		if (m_custom_bag.has_value()) {
			return sprint_mode(m_data,m_number_of_lines, std::move(m_custom_bag.value()));
		} else {
			return sprint_mode(m_data, m_number_of_lines);
		}
	}

	window.clear(sf::Color(100, 100, 100));
	draw_tetris_board(window, m_last_update_since_dead, 200, 600);
	sf::Text time_text;
	time_text.setFont(m_data.default_font);
	time_text.setString(fmt::format("{:.2f}", (double)m_time_before_dead.count() / 1000));
	time_text.setFillColor(sf::Color::Green);
	time_text.setPosition(450, 600);
	window.draw(time_text);
	return std::nullopt;
}



inline std::optional<screen_thingy> finished_sprint::update(event_handler_t& event_handler, game_keybinds& settings) {
	const auto time_since_last_update = event_handler.time_since_last_poll();
	auto& window = event_handler.window();
	m_time_till_not_dead -= time_since_last_update;
	if (m_time_till_not_dead <= 0ms) {
		if (m_custom_bag.has_value()) {
			return forty_line_sprint_state(m_data, std::move(m_custom_bag.value()));
		}
		else {
			return forty_line_sprint_state(m_data);
		}
	}

	window.clear(sf::Color(100, 100, 100));
	draw_tetris_board(window, m_last_update_since_dead, 200, 600);
	sf::Text time_text;
	time_text.setFont(m_data.default_font);
	time_text.setString(fmt::format("{:.2f}", (double)m_time_of_finish.count() / 1000));
	time_text.setFillColor(sf::Color::Green);
	time_text.setPosition(450, 600);
	window.draw(time_text);
	return std::nullopt;
}

namespace internal {
	template<typename T>
	__declspec(noinline) auto thingland(T a) {
		return a();
	}
}

inline coroutine_screen sprint_mode(all_game_data& data,int number_of_lines,std::optional<std::vector<tetris_piece>> bag/*= std::nullopt*/) {
	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr = std::make_unique<tetris_game_keyboard_player>(data.game_settings,bag);
	boring_button back_button = boring_button(0, 10, 200, 100);
	std::chrono::milliseconds time_since_start = 0ms;
	int remaining_lines = number_of_lines;
	int starting_lines_needed = number_of_lines;

	sf::Text back_button_text;
	back_button_text.setFont(data.default_font);
	back_button_text.setString("back");
	back_button.set_text(std::move(back_button_text));
	player_ptr->start_doing_stuff_now();

	while(true) {
		auto [event_handler, settings] = co_await wait_for_next_update();
		if (back_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return start_screen(data);
		}

		time_since_start += event_handler.time_since_last_poll();

		auto& window = event_handler.window();
		auto& player = *player_ptr;

		const auto [actions_this_frame, new_actions_this_frame] = next_actions_pressed(actions, event_handler, settings);

		actions = actions_this_frame;
		//;-;
		//auto game_update_t = std::make_unique<decltype(player.get_update())>(player.get_update());
		/*
		using Y = decltype(player.get_update());
		auto game_update_t = internal::thingland(std::function<std::unique_ptr<Y>()>([&]() {
			return std::make_unique<decltype(player.get_update())>(player.get_update());
		}));
		*/
		auto game_update = player.get_update();
		//auto& game_update = *game_update_t;

		if (game_update.died) {
			co_return dead_forty_line_sprint_state(data, std::move(game_update), starting_lines_needed, time_since_start, std::move(bag));
		}

		remaining_lines -= game_update.lines_cleared;

		if (remaining_lines <= 0) {
			co_return finished_sprint(data, std::move(game_update), starting_lines_needed, time_since_start, std::move(bag));
		}

		player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, 0);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		back_button.draw(window);
		sf::Text time_text;
		time_text.setFont(data.default_font);
		time_text.setString(fmt::format("{:.2f}", (double)time_since_start.count() / 1000));
		time_text.setFillColor(sf::Color::Green);
		time_text.setPosition(450, 600);

		sf::Text pieces_remaining_text;
		pieces_remaining_text.setFont(data.default_font);
		pieces_remaining_text.setString(std::to_string(std::max(remaining_lines, 0)));
		pieces_remaining_text.setFillColor(sf::Color::Green);
		pieces_remaining_text.setPosition(450, 550);

		window.draw(time_text);
		window.draw(pieces_remaining_text);
		//co_yield std::nullopt;
	}
	//co_return;
}


