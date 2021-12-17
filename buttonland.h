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

	explicit boring_button(std::function<void()> on_press) :
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
		}
		else if (m_hovered) {
			rect.setOutlineColor(sf::Color(30, 255, 30));
		}
		else {
			rect.setOutlineColor(sf::Color(60, 255, 60));
		}
		window.draw(rect);
		if (m_text.has_value()) {
			auto& text = *m_text;
			text.setPosition((float)m_x, (float)m_y);
			window.draw(text);
		}
	}

	bool update(const event_handler_t& event_handler, std::chrono::milliseconds time_since_last_update) {
		const auto [mouse_x, mouse_y] = event_handler.current_mouse_position();
		const auto in_bounding_box =
			mouse_x >= m_x && mouse_x <= m_x + m_w && mouse_y >= m_y && mouse_y <= m_h + m_y;
		if (event_handler.is_held(sf::Mouse::Left)) {
			if (!m_pressed_not_over && !m_pressed_over) {
				if (in_bounding_box) {
					m_pressed_over = true;
					return true;
				}
				else {
					m_pressed_not_over = true;
					return false;
				}
			}
			else if (m_pressed_over) {
				if (!in_bounding_box) {
					m_pressed_not_over = true;
					m_pressed_over = false;
					m_click_over_time = 0ms;
					return false;
				}
				else {
					m_click_over_time += time_since_last_update;
					return true;
				}
			}
			else if (m_pressed_not_over) {

			}
		} else {
			if (m_pressed_over) {
				m_pressed_not_over = m_pressed_over = false;
				if (m_on_press) {
					m_on_press();
				}
				//return true;
			}
			m_pressed_not_over = m_pressed_over = false;
		}
		m_hovered = in_bounding_box;
		if (m_hovered) {
			m_hovered_time += time_since_last_update;
		}
		else {
			m_hovered_time = 0ms;
		}
		return false;
	}

	void set_on_press(std::function<void()> a) {
		m_on_press = std::move(a);
	}

	void set_text(std::optional<sf::Text> t) {
		m_text = std::move(t);		
	}

private:
	std::function<void()> m_on_press;
	bool m_pressed_not_over = false;
	bool m_hovered = false;
	bool m_pressed_over = false;
	int m_x = 0;
	
	int m_y = 0;	
	int m_w = 0;
	int m_h = 0;	
	std::chrono::milliseconds m_hovered_time = 0ms;
	std::chrono::milliseconds m_click_over_time = 0ms;
	std::optional<sf::Text> m_text;
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
		const auto [mouse_x, mouse_y] = event_handler.current_mouse_position();
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

struct boring_button2 {
	boring_button2() = default;

	explicit boring_button2(std::function<void()> on_press) :
		m_on_press(std::move(on_press)) {}

	explicit boring_button2(std::function<void()> on_press, int x, int y, int w, int h) :
		m_on_press(std::move(on_press)),
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h) {}

	explicit boring_button2(int x, int y, int w, int h) :
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h) {}

	explicit boring_button2(std::function<void()> on_press, int x, int y, int w, int h,
		std::function<void(std::optional<std::chrono::milliseconds>, std::optional<std::chrono::milliseconds>, int, int, int, int, sf::RenderTarget&)> draw_fn) :
		m_on_press(std::move(on_press)),
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h),
		m_draw_fn(std::move(draw_fn)) {}

	void draw(sf::RenderTarget& window) {
		//draw box		
		if (m_pressed_over) {
			m_draw_fn(std::nullopt, m_click_over_time, m_x, m_y, m_w, m_h, window);
		}
		else if (m_hovered) {
			m_draw_fn(m_hovered_time, std::nullopt, m_x, m_y, m_w, m_h, window);
		}
		else {
			m_draw_fn(std::nullopt, std::nullopt, m_x, m_y, m_w, m_h, window);
		}
	}

	bool update(const event_handler_t& event_handler, std::chrono::milliseconds time_since_last_update) {
		const auto [mouse_x, mouse_y] = event_handler.current_mouse_position();
		const auto in_bounding_box =
			mouse_x >= m_x && mouse_x <= m_x + m_w && mouse_y >= m_y && mouse_y <= m_h + m_y;
		if (event_handler.is_held(sf::Mouse::Left)) {
			if (!m_pressed_not_over && !m_pressed_over) {
				if (in_bounding_box) {
					m_pressed_over = true;
				}
				else {
					m_pressed_not_over = true;
				}
			}
			else if (m_pressed_over) {
				if (!in_bounding_box) {
					m_pressed_not_over = true;
					m_pressed_over = false;
					m_click_over_time = 0ms;
				}
				else {
					m_click_over_time += time_since_last_update;
				}
			}
			else if (m_pressed_not_over) {

			}
		}
		else {
			if (m_pressed_over) {
				if (m_on_press) {
					m_on_press();
				}
				return true;
			}
			m_pressed_not_over = m_pressed_over = false;
		}
		m_hovered = in_bounding_box;
		if (m_hovered) {
			m_hovered_time += time_since_last_update;
		}
		else {
			m_hovered_time = 0ms;
		}
		return false;
	}

private:
	std::function<void()> m_on_press;
	std::function<void(std::optional<std::chrono::milliseconds>, std::optional<std::chrono::milliseconds>, int, int, int, int, sf::RenderTarget&)> m_draw_fn;
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


template<typename... T>
struct hover_draw_thing {
	explicit hover_draw_thing(T... t) :m_things(std::move(t)...) {}
	std::tuple<T...> m_things;
	void operator()(std::optional<std::chrono::milliseconds> hover_time, int x, int y, int w, int h, sf::RenderTarget& window) {
		std::apply([&](auto&... things) {
			(things(hover_time, x, y, w, h, window), ...);
		}, m_things);
	}
};

template<typename... T>
struct normal_draw_thing {
	explicit normal_draw_thing(T... t) :m_things(std::move(t)...) {}
	std::tuple<T...> m_things;
	void operator()(int x, int y, int w, int h, sf::RenderTarget& window) {
		std::apply([&](auto&... things) {
			(things(x, y, w, h, window), ...);
		}, m_things);
	}
};

template<typename... T>
struct click_draw_things {
	explicit click_draw_things(T... t) :m_things(std::move(t)...) {}
	std::tuple<T...> m_things;

	void operator()(std::optional<std::chrono::milliseconds> click_time, int x, int y, int w, int h, sf::RenderTarget& window) {
		std::apply([&](auto&... things) {
			(things(click_time, x, y, w, h, window), ...);
		}, m_things);

	}

};


template<typename... Ts>
auto boring_button_draw_function(Ts&&... t) {
	return [things = std::make_tuple(std::move(t)...)](
		std::optional<std::chrono::milliseconds> hover_time, 
		std::optional<std::chrono::milliseconds> click_time, int x, int y, int w, int h, sf::RenderTarget& window) {

		if (click_time.has_value()) {
			std::apply([&](auto&&... draw_fns) {
				([&]<typename T>(T & fn) {
					if constexpr (requires{fn.click_draw(click_time.value(), x, y, w, h, window); }) {
						fn.click_draw(click_time.value(), x, y, w, h, window);
					}
					else if constexpr (requires{typename T::use_normal_draw; }) {
						fn.normal_draw(x, y, w, h, window);
					}
				}(draw_fns), ...);
				}, things);
		}
		else if (hover_time.has_value()) {

			std::apply([&](auto&&... draw_fns) {
				([&]<typename T>(T & fn) {
					if constexpr (requires{fn.hover_draw(hover_time.value(),x, y, w, h, window); }) {
						fn.hover_draw(hover_time.value(), x, y, w, h, window);
					}
					else if constexpr (requires{typename T::use_normal_draw; }) {
						fn.normal_draw(x, y, w, h, window);
					}
				}(draw_fns), ...);
				}, things);
		}
		else {
			std::apply([&](auto&&... draw_fns) {
				([&]<typename T>(T& fn) {
					if constexpr (requires{fn.normal_draw(x, y, w, h, window); }) {
						fn.normal_draw(x, y, w, h, window);
					}
				}(draw_fns), ...);
			}, things);
		}

	};
}

template<typename T>
struct text_land {
	text_land() = default;

	explicit text_land(T t):
		m_text_fn(std::move(t)){}

	void normal_draw(int x,int y,int w,int h,sf::RenderTarget& window) {
		auto&& text = m_text_fn();
		text.setPosition(sf::Vector2f(x, y));
		window.draw(text);
	}

	void hover_draw(std::optional<std::chrono::milliseconds>,int x, int y, int w, int h, sf::RenderTarget& window) {
		auto&& text = m_text_fn();
		text.setPosition(sf::Vector2f(x, y));
		window.draw(text);
	}

	void click_draw(std::optional<std::chrono::milliseconds>, int x, int y, int w, int h, sf::RenderTarget& window) {
		auto&& text = m_text_fn();
		text.setPosition(sf::Vector2f(x, y));
		window.draw(text);
	}

	using use_normal_draw = int;

private:
	T m_text_fn;
};


template<>
struct text_land<sf::Text> {
	text_land() = default;

	explicit text_land(sf::Text t) :
		m_text(std::move(t)) {}

	void normal_draw(int x, int y, int w, int h, sf::RenderTarget& window) {
		m_text.setPosition(sf::Vector2f(x, y));
		window.draw(m_text);
	}

	void hover_draw(std::optional<std::chrono::milliseconds>, int x, int y, int w, int h, sf::RenderTarget& window) {
		m_text.setPosition(sf::Vector2f(x, y));
		window.draw(m_text);
	}

	void click_draw(std::optional<std::chrono::milliseconds>, int x, int y, int w, int h, sf::RenderTarget& window) {
		m_text.setPosition(sf::Vector2f(x, y));
		window.draw(m_text);
	}

private:
	sf::Text m_text;
};

//template<typename T>
text_land(sf::Text)->text_land<sf::Text>;


template<typename T>
text_land(T)->text_land<T>;


template<typename C,typename S>
struct border_land {
	border_land() = default;

	explicit border_land(C c,S s):m_colour(std::move(c)),m_size(std::move(s)){}

	void normal_draw(int x, int y, int w, int h, sf::RenderTarget& window) {
		auto&& colour = [&]() {
			if constexpr (std::same_as<C, sf::Color>) {
				return m_colour;
			}
			else {
				return std::invoke(m_colour);
			}
		}();

		int64_t size = (int64_t)[&]() {
			if constexpr (std::integral<S> || std::floating_point<S>) {
				return m_size;
			}
			else {
				return std::invoke(m_size);
			}
		}();
		sf::RectangleShape rect;
		rect.setOutlineColor(colour);
		rect.setOutlineThickness(size);
		window.draw(rect);
	}

	using use_normal_draw = int;

private:
	C m_colour;
	S m_size;
};


template<typename T,typename U>
struct same_property:std::false_type {};


template<template<typename>typename A,typename T, typename U>
struct same_property<A<T>,A<U>> :std::true_type {};

template<template<typename> typename... things>
struct property_land {

	

};

