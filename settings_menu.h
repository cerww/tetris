#pragma once
#include "all_game_data.h"
#include "buttonland.h"
#include "screen.h"
#include "start_screen.h"

namespace settings_watlandworld {
	constexpr auto add = [](auto a) {
		return [b = a](auto& c) {
			c += b;
		};
	};

	constexpr auto subtract = [](auto a) {
		return [b = a](auto& c) {
			c -= b;
		};
	};

	constexpr auto inverse_subtract = [](double a) {
		return[b = a]<typename U>(U & c) {
			const auto wat = (int64_t)(1000 / (1000 / (double)c.count() - b));
			c = U(wat);
		};
	};

	constexpr auto inverse_add = [](double a) {
		return[b = a]<typename U>(U & c) {
			const auto wat = (int64_t)(1000 / (1000 / (double)c.count() + b));
			c = U(wat);
		};
	};

	constexpr auto toggle = [](bool& b) {b = !b; };
}

struct thing {
	boring_button decrease_button;
	boring_button increase_button;
	sf::Text text;
	int start_x = 0;
	int start_y = 0;

	std::function<std::string()> text_fn;

	void draw(sf::RenderTarget& target) {
		
	}
};

struct settings_menu {
	settings_menu(all_game_data& t_data) :
		m_data(t_data) {

		sf::Text back_button_text;
		back_button_text.setFont(t_data.default_font);
		back_button_text.setString("Back");
		back_button_text.setFillColor(sf::Color::Red);
		m_back_button.set_text(std::move(back_button_text));

		set_text_to_thing(m_plus_das_button, "+");
		set_text_to_thing(m_sub_das_button, "-");

		set_text_to_thing(m_plus_arr_button, "+");
		set_text_to_thing(m_sub_arr_button, "-");

		set_text_to_thing(m_plus_ai_speed_button, "+");
		set_text_to_thing(m_sub_ai_speed_button, "-");

		set_text_to_thing(m_plus_ai_speed2_button, "+");
		set_text_to_thing(m_sub_ai_speed2_button, "-");

		set_text_to_thing(m_toggle_ai_tspin, "Toggle AI T-spins");

		/*
		set_text_to_thing(m_plus_das_button,"+");

		set_text_to_thing(m_plus_das_button,"+");
		set_text_to_thing(m_plus_das_button,"+");

		set_text_to_thing(m_plus_das_button,"+");
		set_text_to_thing(m_plus_das_button,"+");
		*/
		set_up_text();
		

	}

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& settings) {
		auto& window = event_handler.window();
		
		using namespace settings_watlandworld;
		if (m_back_button.update(event_handler, event_handler.time_since_last_poll())) {
			return start_screen(m_data);
		}

		const auto time_step = event_handler.time_since_last_poll();

		if (m_toggle_ai_tspin.update(event_handler, time_step)) {
			m_data.ai_game_settings.use_tspins = !m_data.ai_game_settings.use_tspins;
		} else if (m_plus_das_button.update(event_handler, time_step)) {
			do_thing(m_data.game_settings.das_time, add(1ms), time_step);
		} else if (m_sub_das_button.update(event_handler, time_step)) {
			do_thing(m_data.game_settings.das_time, subtract(1ms), time_step);
		} else if (m_plus_arr_button.update(event_handler, time_step)) {
			do_thing(m_data.game_settings.arr_time, add(1ms), time_step);
		} else if (m_sub_arr_button.update(event_handler, time_step)) {
			do_thing(m_data.game_settings.arr_time, subtract(1ms), time_step);
		} else if (m_plus_ai_speed_button.update(event_handler, time_step)) {
			do_thing(m_data.ai_game_settings.piece_delay, add(1ms), time_step);
		} else if (m_sub_ai_speed_button.update(event_handler, time_step)) {
			do_thing(m_data.ai_game_settings.piece_delay, subtract(1ms), time_step);
		} else if (m_plus_ai_speed2_button.update(event_handler, time_step)) {
			do_thing(m_data.ai_game_settings.piece_delay, inverse_add(0.1), time_step);
		} else if (m_sub_ai_speed2_button.update(event_handler, time_step)) {
			do_thing(m_data.ai_game_settings.piece_delay, inverse_subtract(0.1), time_step);
		} else if (m_toggle_ai_tspin.update(event_handler, time_step)) {
			do_thing(m_data.ai_game_settings.use_tspins, toggle, time_step);
		} else {
			m_button_pressed = false;
			m_auto_repeat_click_time_left = m_init_auto_repeat_delay;
			m_time_till_init_repeat_press = m_init_time_till_auto_repeat;
		}


		window.clear(sf::Color(200, 200, 200));
		m_back_button.draw(window);

		m_plus_das_button.draw(window);
		sf::Text das_value;
		das_value.setPosition(450, 100);
		das_value.setFont(m_data.default_font);
		das_value.setCharacterSize(13);
		das_value.setFillColor(sf::Color::Red);
		das_value.setString(fmt::format("{}ms", m_data.game_settings.das_time.count()));
		window.draw(das_value);
		m_sub_das_button.draw(window);

		m_plus_arr_button.draw(window);
		sf::Text arr_text;
		arr_text.setFont(m_data.default_font);
		arr_text.setPosition(450, 160);
		arr_text.setCharacterSize(13);
		arr_text.setFillColor(sf::Color::Red);
		arr_text.setString(fmt::format("{}ms", m_data.game_settings.arr_time.count()));
		window.draw(arr_text);
		m_sub_arr_button.draw(window);

		m_plus_ai_speed_button.draw(window);
		sf::Text ai_delay;
		ai_delay.setPosition(450, 220);
		ai_delay.setFont(m_data.default_font);
		ai_delay.setCharacterSize(13);
		ai_delay.setFillColor(sf::Color::Red);
		ai_delay.setString(fmt::format("{}ms", m_data.ai_game_settings.piece_delay.count()));
		window.draw(ai_delay);
		m_sub_ai_speed_button.draw(window);


		sf::Text ai_speed;
		ai_speed.setPosition(450,280);
		ai_speed.setFont(m_data.default_font);
		ai_speed.setCharacterSize(13);
		ai_speed.setFillColor(sf::Color::Red);
		ai_speed.setString(fmt::format("{:.2f}", 1000 / (double)m_data.ai_game_settings.piece_delay.count()));
		window.draw(ai_speed);
		m_plus_ai_speed2_button.draw(window);
		m_sub_ai_speed2_button.draw(window);

		sf::Text ai_uses_tspins;
		ai_uses_tspins.setPosition(400, 430);
		ai_uses_tspins.setFont(m_data.default_font);
		ai_uses_tspins.setCharacterSize(13);
		ai_uses_tspins.setFillColor(sf::Color::Red);
		ai_uses_tspins.setString(fmt::format("{}", m_data.ai_game_settings.use_tspins ? "uses Tspins" : "tspins disabled"));
		window.draw(ai_uses_tspins);
		m_toggle_ai_tspin.draw(window);

		window.draw(m_das_text);
		window.draw(m_arr_text);
		window.draw(m_ai_piece_delay_text);


		return std::nullopt;
	}


private:
	void set_text_to_thing(boring_button& b, std::string s) {
		sf::Text text;
		text.setFillColor(sf::Color::Red);
		text.setFont(m_data.default_font);
		text.setString(std::move(s));
		b.set_text(std::move(text));
	}

	template<typename T, typename F>
	void do_thing(T& a, F f, std::chrono::milliseconds time_since_thingy) {
		if (!m_button_pressed) {
			f(a);
			m_button_pressed = true;
		} else {
			if (m_time_till_init_repeat_press <= 0ms) {
				m_auto_repeat_click_time_left -= time_since_thingy;
				while (m_auto_repeat_click_time_left <= 0ms) {
					m_auto_repeat_click_time_left += m_init_auto_repeat_delay;
					f(a);
				}
			} else {
				m_time_till_init_repeat_press -= time_since_thingy;
			}
		}
	}

	void set_up_text() {
		set_text_to_stuff(m_das_text, "DAS", 350, 100);
		set_text_to_stuff(m_arr_text, "ARR", 350, 160);
		set_text_to_stuff(m_ai_piece_delay_text, "AI delay", 320, 220);
		set_text_to_stuff(m_ai_speed_text, "AI speed", 320, 280);
	}

	void set_text_to_stuff(sf::Text& text,std::string s,int x,int y) const {
		text.setFillColor(sf::Color::Red);
		text.setFont(m_data.default_font);
		text.setString(s);
		text.setPosition(x, y);
		text.setCharacterSize(20);		
	}


	all_game_data& m_data;


	boring_button m_back_button = boring_button(0, 10, 200, 100);

	boring_button m_plus_das_button = boring_button(500, 100, 50, 50);
	boring_button m_sub_das_button = boring_button(400, 100, 50, 50);

	boring_button m_plus_arr_button = boring_button(500, 160, 50, 50);
	boring_button m_sub_arr_button = boring_button(400, 160, 50, 50);

	boring_button m_plus_ai_speed_button = boring_button(500, 220, 50, 50);
	boring_button m_sub_ai_speed_button = boring_button(400, 220, 50, 50);

	boring_button m_plus_ai_speed2_button = boring_button(500, 280, 50, 50);
	boring_button m_sub_ai_speed2_button = boring_button(400, 280, 50, 50);

	boring_button m_toggle_ai_tspin = boring_button(400, 340, 150, 50);

	bool m_button_pressed = false;

	//only 1 button can be pressed at a time.
	std::chrono::milliseconds m_init_time_till_auto_repeat = 150ms;
	std::chrono::milliseconds m_init_auto_repeat_delay = 30ms;

	std::chrono::milliseconds m_time_till_init_repeat_press = m_init_time_till_auto_repeat;

	std::chrono::milliseconds m_auto_repeat_click_time_left = m_init_auto_repeat_delay;

	sf::Text m_das_text;
	sf::Text m_arr_text;
	sf::Text m_ai_piece_delay_text;
	sf::Text m_ai_speed_text;


};
