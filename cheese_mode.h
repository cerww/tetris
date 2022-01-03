#pragma once
#include "all_game_data.h"
#include "screen.h"
#include "buttonland.h"

coroutine_screen cheese_screen(all_game_data& data);

coroutine_screen cheese_prev_screen(all_game_data& data);

coroutine_screen cheese_screen(all_game_data& data) {

	co_return cheese_prev_screen(data);
}

constexpr auto always = [](auto& a) {
	return [&]()->auto& {return a; };
};

template<typename T,int start,int step>
struct int_sequence_impl {
	
};

template<int... C, int start, int step>
struct int_sequence_impl<std::index_sequence<C...>,start,step> {
	using value = std::index_sequence<C* step + start...>;
};

template<int start,int end,int step>
struct int_sequence {
	using seq = std::make_index_sequence<(end - start) / step>;
	using value_type = int_sequence_impl<seq, start, step>;
};

template<typename... T>
auto make_thingy(int x,int y,sf::Font& f,T&&... ts) {
	static_assert(sizeof...(T) % 3 == 0);
	constexpr int number_of_buttons = sizeof...(T) / 3;

	

	auto args = std::forward_as_tuple(std::forward<T>(ts)...);

	auto text_functions = [&](auto&&... argsland) {
		const auto indexes_of_things = int_sequence<1, sizeof...(T), 3>::value_type;
		return[&]<int... I>(std::index_sequence<I...>) {
			return std::forward_as_tuple(std::get<I>(args)...);
		}(indexes_of_things);
	};

	struct thing {
		std::array<boring_button, number_of_buttons> subtract_buttons;
		std::array<boring_button, number_of_buttons> add_buttons;
		decltype(text_functions) text_functions_land;
	};

	std::array<boring_button, number_of_buttons> subtract_buttons;
	std::array<boring_button, number_of_buttons> add_buttons;

	for(int i = 0;i<number_of_buttons;++i) {
		boring_button sub_button = boring_button(x,y+i*120,150,100);
		sub_button.set_text("-",f,sf::Color::Red);
		
		
	}

	
}

template<typename S,typename T,typename A>
struct button_properties {
	button_properties() = default;
	explicit button_properties(std::string l, S s, T t, A a) :
		label(std::move(l)),
		sub_button(std::move(s)),text_function(std::move(t)),add_function(std::move(a)){}


	std::string label = "";
	S sub_button;
	T text_function;
	A add_function;
};

template<typename F>
struct button_row {
	boring_button sub_button;
	F text_function;
	boring_button add_button;
};

template<typename... T>
auto make_button_column(int x,int y,sf::Font& f,T&&... button_stuffs) {
	return [&]<int... I>(std::index_sequence<I...>) {
		return std::make_tuple([&](int i, auto&& button_stuff) {
			auto&& [S, text_fn, A] = button_stuff;
			auto ret = button_row<std::decay_t<decltype(text_fn)>>(
				boring_button(x, y + 60 * i, 50, 50), std::move(text_fn), boring_button(x + 100, y + i * 60, 50, 50));
			ret.sub_button.set_text("-",f,sf::Color::Red);
			ret.sub_button.set_on_press(std::move(S));
			ret.add_button.set_text("+",f,sf::Color::Red);
			ret.add_button.set_on_press(std::move(A));
			return ret;
		}(I, button_stuffs)...);
	}(std::index_sequence_for<T...>());

}

coroutine_screen cheese_prev_screen(all_game_data& data) {
	auto [event_handler, keybinds] = co_await wait_for_next_update();

	std::chrono::milliseconds garbage_delay = 2s;
	int number_of_lines_per_send = 4;

	const auto add = [](auto& thing,auto amount) {
		return [&thing,amount = amount]() {
			thing += amount;
		};
	};

	const auto sub = [](auto& thing, auto amount) {
		return [&thing, amount = amount]() {
			thing -= amount;
		};
	};

	const auto always = [](const auto& b) {
		return [&b]() ->const auto&{
			return b;
		};
	};

	auto buttons = make_button_column(300,200,data.default_font
		,button_properties(1, 1, 1)
	);

	co_return cheese_screen(data);
}



