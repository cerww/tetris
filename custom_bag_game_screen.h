#pragma once
#include "forty_line_sprint_screen.h"
#include "screen.h"
#include "playing_pvAI_screen.h"
#include "playing_solo.h"

struct impl_print_on_move {
	impl_print_on_move() = default;

	explicit impl_print_on_move(std::string s) :text(std::move(s)){}

	impl_print_on_move(impl_print_on_move&& o):text(std::move(o.text)) {
		fmt::print("{}\n",text);
	}

	impl_print_on_move& operator=(impl_print_on_move&& o)  {
		text = std::move(o.text);
		fmt::print("{}\n", text);
	}
	impl_print_on_move(const impl_print_on_move&) = default;
	impl_print_on_move& operator=(const impl_print_on_move&) = default;

	std::string text;
};

struct custom_bag_game_screen_prev_screen {
	explicit custom_bag_game_screen_prev_screen(all_game_data& t_data, std::vector<tetris_piece> bag) :
		m_data(t_data),
		m_bag(std::move(bag)) {

		//fix_buttons();
		m_play_vs_ai_button.set_text("vs ai", m_data.default_font, sf::Color::Red);
		m_play_solo_button.set_text("solo", m_data.default_font, sf::Color::Red);
		m_40Lsprint_button.set_text("40L sprint", m_data.default_font, sf::Color::Red);
		m_cheese_mode_button.set_text("cheese", m_data.default_font, sf::Color::Red);
	}

	explicit custom_bag_game_screen_prev_screen(all_game_data& t_data) :
		m_data(t_data) {

		//fix_buttons();
		m_play_vs_ai_button.set_text("vs ai",m_data.default_font,sf::Color::Red);
		m_play_solo_button.set_text("solo", m_data.default_font, sf::Color::Red);
		m_40Lsprint_button.set_text("40L sprint", m_data.default_font, sf::Color::Red);
		m_cheese_mode_button.set_text("cheese", m_data.default_font, sf::Color::Red);
	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings);

	void fix_buttons() {
		fmt::print("{}\n", (int64_t)this);
		m_buttons.clear();
		for (int i = 0; i < m_bag.size(); ++i) {
			auto& [top_button, bot_button] = m_buttons.emplace_back(boring_button(150 + i * 50, 400, 50, 50), boring_button(150 + i * 50, 500, 50, 50));
			top_button.set_text("^", m_data.default_font, sf::Color::Red);
			bot_button.set_text("V", m_data.default_font, sf::Color::Red);

			top_button.set_on_press([this, i]() {
				m_bag[i] = (tetris_piece)(((int)m_bag[i] + 1) % 7);
			});

			bot_button.set_on_press([this, i]() {
				m_bag[i] = (tetris_piece)(((int)m_bag[i] - 1 + 7) % 7);
			});

		}
		m_add_button = boring_button(150 + (int)m_bag.size() * 50,450,50,50);
		m_add_button.set_text("+", m_data.default_font, sf::Color::Red);
		m_remove_button.set_text("-", m_data.default_font, sf::Color::Red);
		m_add_button.set_on_press([this]() {
			m_bag.push_back(tetris_piece::I);
			fix_buttons();
		});

		if (m_bag.size() > 1) {
			m_remove_button.set_on_press([this]() {
				m_bag.pop_back();
				fix_buttons();
			});
		}else {
			m_remove_button.set_on_press([]() {});
		}
	}

private:
	all_game_data& m_data;

	std::vector<tetris_piece> m_bag = { tetris_piece::I,tetris_piece::L,tetris_piece::J,tetris_piece::O,tetris_piece::Z,tetris_piece::S,tetris_piece::T };
	std::vector<std::pair<boring_button, boring_button>> m_buttons;
	boring_button m_remove_button = boring_button(100,450,50,50);
	boring_button m_add_button;

	boring_button m_play_vs_ai_button = boring_button(100,600,100,50);
	boring_button m_play_solo_button = boring_button(220,600,100,50);
	boring_button m_40Lsprint_button = boring_button(340,600,100,50);
	boring_button m_cheese_mode_button = boring_button(460,600,100,50);

	bool m_buttons_need_to_be_fixed = true;
	//std::array<int, 5000> wat = {};
	//impl_print_on_move m___wat = impl_print_on_move("watland");
};


//unused?
struct custom_bag_game_screen {
	explicit custom_bag_game_screen(tetris_game_settings player_settings, std::vector<tetris_piece> bag):
		player_ptr(std::make_unique<tetris_game_keyboard_player>(std::move(player_settings),std::move(bag))) { }

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings) {
		auto& window = event_handler.window();
		auto& player = *player_ptr;


		std::array<bool, (int)action::size> actions_this_frame = {};
		std::ranges::fill(actions_this_frame, false);

		for (auto [action, key] : settings.keybinds) {
			if (event_handler.is_held(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, key] : settings.joystick_binds) {
			if (event_handler.is_any_pressed(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, axis, amount] : settings.joy_stick_axis) {
			if (amount < 0) {
				if (sf::Joystick::getAxisPosition(0, axis) <= amount) {
					actions_this_frame[(int)action] = true;
				}
			} else if (amount > 0) {
				if (sf::Joystick::getAxisPosition(0, axis) >= amount) {
					actions_this_frame[(int)action] = true;
				}
			}
		}
		std::array<bool, (int)action::size> new_actions_this_frame = {};

		for (auto [a, b, c] : ranges::views::zip(actions, actions_this_frame, new_actions_this_frame)) {
			c = b && !a;
		}

		actions = actions_this_frame;

		const auto game_update = player.get_update();
		if (game_update.died) {
			player.stop_doing_stuff();

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);

			//return dead_state(std::move(player_ptr), std::move(ai_player_ptr));
		} else {
			player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, 0);

			window.clear(sf::Color(100, 100, 100));
			draw_tetris_board(window, game_update, 200, 600);

			//const auto heights = col_heights(game_update.game_state.board.minos);
		}
		return std::nullopt;
	}

private:
	std::array<bool, (int)action::size> actions = {};

	std::unique_ptr<tetris_game_keyboard_player> player_ptr;

};



inline std::optional<screen_thingy> custom_bag_game_screen_prev_screen::update(event_handler_t& event_handler, game_keybinds& settings) {
	//start button
	//fmt::print("{}\n", (int64_t)this);
	if(std::exchange(m_buttons_need_to_be_fixed,false)) {
		fix_buttons();
	}
	if(m_play_vs_ai_button.update(event_handler,event_handler.time_since_last_poll())) {
		return playing_pVai_state(m_data, m_data.ioc->get_executor(), std::move(m_bag));
	}else if(m_play_solo_button.update(event_handler,event_handler.time_since_last_poll())) {
		return playing_solo_state(m_data,std::move(m_bag));
	} else if (m_40Lsprint_button.update(event_handler, event_handler.time_since_last_poll())) {
		return sprint_mode(m_data,40, std::move(m_bag));
	}else if(m_cheese_mode_button.update(event_handler,event_handler.time_since_last_poll())) {
		return cheese_prev_screen(m_data, 2s, 1, std::move(m_bag));
	}

	m_add_button.update(event_handler, event_handler.time_since_last_poll());
	m_remove_button.update(event_handler, event_handler.time_since_last_poll());	

	event_handler.window().clear(sf::Color(200, 200, 200));
	for (int i = 0; i < m_bag.size(); ++i) {
		auto& [top_button, bot_button] = m_buttons[i];
		top_button.update(event_handler, event_handler.time_since_last_poll());
		bot_button.update(event_handler, event_handler.time_since_last_poll());
		draw_piece(event_handler.window(), piece_offsets[(int)m_bag[i]][0],i*50+165,475,piece_color(m_bag[i]),0.5);
		top_button.draw(event_handler.window());
		bot_button.draw(event_handler.window());
	}

	m_add_button.draw(event_handler.window());
	m_remove_button.draw(event_handler.window());



	m_play_vs_ai_button.draw(event_handler.window());
	m_play_solo_button.draw(event_handler.window());
	m_40Lsprint_button.draw(event_handler.window());
	m_cheese_mode_button.draw(event_handler.window());
	
	return std::nullopt;
}
