#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "screen.h"
#include "meta_fns.h"

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

	void set_text(std::string s,sf::Font& f,sf::Color c) {
		m_text.emplace();
		m_text->setFillColor(c);
		m_text->setFont(f);
		m_text->setString(s);
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

template<typename T,typename F>
const T& value_or_thing(F& f) {
	if constexpr(std::convertible_to<F,T>) {
		return T(f);
	} else {
		return f();
	}
}

template<typename...T>
struct cooler_button {

	cooler_button(int x,int y,int w,int h,T... ts):m_x(x),m_y(y),m_w(w),m_h(h),callbacks(std::move(ts)...) {}

	bool update(event_handler_t& event_handler) {
		const auto [mouse_x, mouse_y] = event_handler.current_mouse_position();
		const auto in_bounding_box =
			mouse_x >= m_x && mouse_x <= m_x + m_w && mouse_y >= m_y && mouse_y <= m_h + m_y;
		const auto time_since_last_update = event_handler.time_since_last_poll();

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
		}
		else {
			if (m_pressed_over) {
				m_pressed_not_over = m_pressed_over = false;
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

	void draw(sf::RenderTarget& window) {
		//std::apply();
	}

private:
	int m_x = 0;
	int m_y = 0;
	int m_w = 0;
	int m_h = 0;
	bool m_pressed_not_over = false;
	bool m_hovered = false;
	bool m_pressed_over = false;

	std::chrono::milliseconds m_hovered_time = 0ms;
	std::chrono::milliseconds m_click_over_time = 0ms;

	std::tuple<T...> callbacks;
};

namespace button_properties_wat {

	template<typename T,typename V = void>
	auto value_type_value() {
		if constexpr(std::invocable<T>) {
			return std::declval<T>()();
		}else {
			return std::declval<T>();
		}
	}

	template<typename C,typename T>
	struct property_t {

		explicit property_t(T a) :m_me(std::move(a)) {}

		decltype(value_type_value<T>()) value() {
			if constexpr (std::invocable<T>) {
				return m_me();
			}
			else {
				return m_me;
			}
		}
		using property_value = C;
		using return_type = decltype(value_type_value<T>());
	private:
		T m_me;
	};

	template<typename C>
	struct create_property_function {

		template<std::movable T>
		property_t<C,T> operator()(T a)const noexcept {
			return property_t<C, T>(std::move(a));
		}
	};

	template<typename T,typename E,typename V = void>
	concept property_for = std::same_as<E, typename T::property_value> && (std::same_as<V,void> || std::convertible_to<typename T::return_value,V>);

	template<typename T>
	int last_true_idx(const T& a) {
		for (int i = a.size() - 1; i >= 0; --i) {
			if (a[i]) {
				return i;
			}
		}
		return -1;
	}

	struct hover_t{};
	struct thickness_t{};
	struct active_t{};

	template<typename ...T>
	struct property_holder {
		explicit property_holder(T... a):m_properties(std::move(a)...){}

		template<typename C,typename R = void>
		decltype(auto) value() {
			constexpr std::array<bool, sizeof...(T)> arr = {property_for<T,C>...};
			constexpr auto idx = last_true_idx(arr);
			if constexpr (std::is_void_v<R>) {
				return std::get<idx>(m_properties).value();
			}else {
				return R(std::get<idx>(m_properties).value());
			}
		}

		template<typename C,typename F,typename R = void>
		decltype(auto) value_or_default(F&& f) {
			constexpr std::array<bool, sizeof...(T)> arr = { property_for<T,C>... };
			constexpr auto idx = last_true_idx(arr);
			if constexpr (idx != -1) {
				if constexpr (std::is_void_v<R>) {
					return std::get<idx>(m_properties).value();
				}else {
					return R(std::get<idx>(m_properties).value());
				}
			}else {
				if constexpr (std::is_void_v<R>) {
					if constexpr (std::invocable<F>) {
						return f();
					}else {
						return std::forward<F>(f);
					}
				}
				else {
					if constexpr (std::invocable<F>) {
						return R(f());
					}
					else {
						return R(std::forward<F>(f));
					}
				}
			}
		}

		template<typename C,typename F>
		void apply_on_all(F&& f) {
			std::apply([&](auto&... p) {
				([&]<typename U>(U & u) {
					if constexpr(property_for<U,C>) {
						f(u);
					}
				}(p),...);
			}, m_properties);
		}

	private:
		std::tuple<T...> m_properties;
	};

	template<typename... T>
	struct border: property_holder<T...> {
		explicit border(sf::Color c, T... s) :property_holder(std::move(s)...) ,m_color(c) {}

		void update(event_handler_t& event_handler, std::optional<std::chrono::milliseconds> hover_time,std::optional<std::chrono::milliseconds> click_time) {
			//do nothing!
			
		}

		void draw(int x,int y,int w,int h,bool hovered, bool clicked, sf::RenderTarget& window) {
			const int thickness = [&]() {
				return 1;
			}();

			const auto colour = [&]() {
				return m_color;
			}();

			sf::RectangleShape box;
			box.setSize(sf::Vector2f((float)w, (float)h));
			box.setPosition((float)x, (float)y);
			box.setOutlineColor(colour);
			window.draw(box);
		}

		static int draw_index() noexcept {
			return 1;
		}

	private:
		sf::Color m_color;
	};

	template<typename T>
	struct on_click {

		explicit on_click(T a):m_function(std::move(a)){}

		void draw(int x, int y, int w, int h, bool hovered, bool clicked, sf::RenderTarget& window) {
			
		}

	private:
		T m_function;
	};



}
template<typename...T>
auto make_button(int x,int y,int w,int h,T&&... button_stuffs) {
	//auto button = make_button(x,y,w,h,border(white,pixel(1)),on_click([](){}),on_mouse_over(...),text("",color),background(black,hover(red),click(blue)))
	return cooler_button(x, y, w, h, std::forward<T>(button_stuffs)...);

}


