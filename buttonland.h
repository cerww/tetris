#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "screen.h"


struct imbutton { };


struct imbuttongroup {


private:

	struct button_state {
		bool hover = false;
		bool clicked = false;
		std::optional<std::string> key;
	};

	std::unordered_map<std::string, int> m_key_idxs;
	std::vector<button_state> m_prev_button_states;
	std::string m_last_tag;
};


struct boring_button {
	boring_button() = default;

	explicit boring_button(std::function<void()> on_press):
		m_on_press(std::move(on_press)) {}

	explicit boring_button(std::function<void()> on_press, int x, int y, int w, int h) :
		m_on_press(std::move(on_press)),
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h) {}

	explicit boring_button(int x, int y, int w, int h) :
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h) {}

	void draw(sf::RenderTarget& window) {
		//draw box

		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f((float)m_w, (float)m_h));
		rect.setPosition((float)m_x, (float)m_y);
		rect.setOutlineThickness(-1);
		if (m_pressed_over) {
			rect.setFillColor(sf::Color());
			rect.setOutlineColor(sf::Color(0, 220, 0));
		} else if (m_hovered) {
			rect.setOutlineColor(sf::Color(30, 255, 30));
		} else {
			rect.setOutlineColor(sf::Color(60, 255, 60));
		}
		window.draw(rect);
	}

	bool update(const event_handler_t& event_handler, std::chrono::milliseconds time_since_last_update) {
		const auto [mouse_x, mouse_y] = event_handler.current_mouse_position();
		const auto in_bounding_box =
				mouse_x >= m_x && mouse_x <= m_x + m_w && mouse_y >= m_y && mouse_y <= m_h + m_y;
		if (event_handler.is_held(sf::Mouse::Left)) {
			if(!m_pressed_not_over && !m_pressed_over) {
				if(in_bounding_box) {
					m_pressed_over = true;
				}else {
					m_pressed_not_over = true;
				}
			}else if(m_pressed_over) {
				if(!in_bounding_box) {
					m_pressed_not_over = true;
					m_pressed_over = false;
					m_click_over_time = 0ms;
				}else {
					m_click_over_time += time_since_last_update;
				}
			}else if(m_pressed_not_over) {
				
			}
		} else {
			if(m_pressed_over) {
				if(m_on_press) {
					m_on_press();
				}
				return true;
			}
			m_pressed_not_over = m_pressed_over = false;
		}
		m_hovered = in_bounding_box;
		if(m_hovered) {
			m_hovered_time += time_since_last_update;
		}else {
			m_hovered_time = 0ms;
		}
		return false;
	}

private:
	std::function<void()> m_on_press;
	bool m_pressed_not_over = false;
	bool m_pressed_over = false;
	int m_x = 0;
	int m_y = 0;
	int m_w = 0;
	int m_h = 0;
	bool m_hovered = false;
	std::chrono::milliseconds m_hovered_time = 0ms;
	std::chrono::milliseconds m_click_over_time = 0ms;
};


struct textland {
	std::string_view s;
};


template<typename... T>
void no_state_button(sf::RenderWindow& window, int x, int y, T&&... args) { }

struct text_input {
	text_input() = default;

	explicit text_input(std::string s) :
		m_place_holder_text(std::move(s)) {}

	explicit text_input(std::string s, int t_x, int t_y, int t_w, int t_h) :
		m_place_holder_text(std::move(s)),
		x(t_x),
		y(t_y),
		w(t_w),
		h(t_h) {}

	std::string_view current_text() const noexcept {
		return m_current_text;
	}

	void draw(sf::RenderTarget& window) {
		//draw box,
		//draw letters
	}

	void update(const event_handler_t& event_handler, std::chrono::milliseconds time_since_last_update) {
		const auto [mouse_x,mouse_y] = event_handler.current_mouse_position();
		const auto in_bounding_box =
				mouse_x >= this->x && mouse_x <= this->x + this->w && mouse_y >= this->y && mouse_y <= this->h + this->y;

		const auto pressed_this_update = event_handler.is_held(sf::Mouse::Left);

		if (in_bounding_box) {
			m_mouse_overed = true;
		}

		if (!m_mouse_pressed && pressed_this_update && m_mouse_overed) {
			m_is_selected = true;
		}

		m_mouse_pressed = pressed_this_update;

		if (m_is_selected) {
			if (ranges::contains(event_handler.chars_pressed(), (char)8)) {
				m_current_text.pop_back();
			}

			ranges::push_back(m_current_text, event_handler.chars_pressed() | ranges::views::filter([](auto a) { return a != (char)8; }));
		}
	}

	void set_stuff(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}


private:
	std::string m_current_text;
	std::string m_place_holder_text;

	bool m_is_selected = false;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 20;

	bool m_mouse_overed = false;
	bool m_mouse_pressed = false;


};
