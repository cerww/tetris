#pragma once
#include "all_game_data.h"
#include "screen.h"
#include "buttonland.h"
#include "start_screen.h"
#include <fmt/chrono.h>

coroutine_screen cheese_prev_screen(all_game_data& data,std::chrono::milliseconds = 2s,int = 1, std::optional<std::vector<tetris_piece>> bag = std::nullopt);

coroutine_screen cheese_screen(all_game_data& data,std::chrono::milliseconds garbage_rate,int garbage_amount,std::optional<std::vector<tetris_piece>> bag = std::nullopt) {
	std::array<bool, (int)action::size> actions = {};

	//std::unique_ptr<tetris_game_keyboard_player> player_ptr = std::make_unique<tetris_game_keyboard_player>(data.game_settings, bag);
	auto player = tetris_game_keyboard_player(data.game_settings,bag);
	boring_button back_button = boring_button(0, 10, 200, 100);
	std::chrono::milliseconds time_since_start = 0ms;

	sf::Text back_button_text;
	back_button_text.setFont(data.default_font);
	back_button_text.setString("back");
	back_button.set_text(std::move(back_button_text));
	player.start_doing_stuff_now();
	auto time_till_next_garbage_send = garbage_rate;
	while (true) {
		auto [event_handler, settings] = co_await wait_for_next_update();
		if (back_button.update(event_handler, event_handler.time_since_last_poll())) {
			co_return start_screen(data);
		}

		time_since_start += event_handler.time_since_last_poll();

		auto& window = event_handler.window();
		//auto& player = *player_ptr;

		const auto [actions_this_frame, new_actions_this_frame] = next_actions_pressed(actions, event_handler, settings);

		actions = actions_this_frame;

		auto game_update = player.get_update();
		//auto& game_update = *game_update_t;

		if (game_update.died) {
			co_return cheese_prev_screen(data, garbage_rate, garbage_amount,std::move(bag));
		}
		time_till_next_garbage_send -= event_handler.time_since_last_poll();
		int garbage_to_send_this_frame = 0;
		while(time_till_next_garbage_send<=0ms) {
			time_till_next_garbage_send += garbage_rate;
			garbage_to_send_this_frame += garbage_amount;
		}


		player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, garbage_to_send_this_frame);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		back_button.draw(window);
		sf::Text time_text;
		time_text.setFont(data.default_font);
		time_text.setString(fmt::format("{:.2f}", (double)time_since_start.count() / 1000));
		time_text.setFillColor(sf::Color::Green);
		time_text.setPosition(450, 600);
		
		window.draw(time_text);
		//window.draw(pieces_remaining_text);
		//co_yield std::nullopt;
	}
	//co_return cheese_prev_screen(data);
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


	std::string label;
	S sub_button;
	T text_function;
	A add_function;
};

template<typename F>
struct button_row {
	sf::Text label;
	boring_button sub_button;
	F text_function;
	boring_button add_button;

	std::tuple<bool,bool> update(event_handler_t& event_handler) {
		return {
			sub_button.update(event_handler,event_handler.time_since_last_poll()),
			add_button.update(event_handler,event_handler.time_since_last_poll())
		};		
	}

	void draw(sf::RenderTarget& window) {
		window.draw(label);
		sub_button.draw(window);
		add_button.draw(window);
		sf::Text t;
		t.setFillColor(label.getFillColor());
		t.setString(text_function());
		t.setPosition(label.getPosition().x + 160, label.getPosition().y);
		t.setFont(*label.getFont());
		t.setCharacterSize(13);
		window.draw(t);
	}

};

template<typename... T>
auto make_button_column(int x,int y,sf::Font& f,T&&... button_stuffs) {
	return [&]<int... I>(std::index_sequence<I...>) {
		return std::make_tuple([&](int i, auto&& button_stuff) {
			auto&& [label,S, text_fn, A] = button_stuff;			
			auto ret = button_row<std::decay_t<decltype(text_fn)>>(
				sf::Text(),
				boring_button(x+100, y + 60 * i, 50, 50), std::move(text_fn), boring_button(x + 230, y + i * 60, 50, 50));

			ret.sub_button.set_text("-",f,sf::Color::Red);
			ret.sub_button.set_on_press(std::move(S));
			ret.add_button.set_text("+",f,sf::Color::Red);
			ret.add_button.set_on_press(std::move(A));
			ret.label.setString(std::move(label));
			ret.label.setFont(f);
			ret.label.setPosition(x, y + i * 60);
			ret.label.setFillColor(sf::Color::Red);
			ret.label.setCharacterSize(13);
			return ret;
		}(I, button_stuffs)...);
	}(std::index_sequence_for<T...>());

}

coroutine_screen cheese_prev_screen(all_game_data& data, std::chrono::milliseconds garbage_delay /*=2s*/, int number_of_lines_per_send /*=1*/,
	std::optional<std::vector<tetris_piece>> bag) {

	auto [event_handler, keybinds] = co_await wait_for_next_update();

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

	constexpr auto always_to_string = [](const auto& b) {
		return [&b](){
			return fmt::format("{}",b);
		};
	};

	auto buttons = make_button_column(300,200,data.default_font,
		button_properties("send rate",sub(garbage_delay,100ms), always_to_string(garbage_delay), add(garbage_delay, 100ms)),
		button_properties("amount",sub(number_of_lines_per_send,1), always_to_string(number_of_lines_per_send), add(number_of_lines_per_send, 1))
	);


	boring_button back_button = boring_button(0, 10, 200, 100);

	boring_button play_button = boring_button(600,200,200,100);
	play_button.set_text("play",data.default_font,sf::Color::Red);

	while(true) {
		auto [event_handler, stuff] = co_await wait_for_next_update();
		auto& window = event_handler.window();

		const auto time_step = event_handler.time_since_last_poll();
		if(back_button.update(event_handler,time_step)) {
			co_return start_screen(data);
		}
		if(play_button.update(event_handler,time_step)) {
			co_return cheese_screen(data, garbage_delay, number_of_lines_per_send,std::move(bag));
		}

		std::apply([&](auto&... a) {
			((void)a.update(event_handler), ...);
		}, buttons);


		window.clear(sf::Color(100, 100, 100));

		std::apply([&](auto&... a) {
			((void)a.draw(event_handler.window()), ...);
		}, buttons);

		back_button.draw(event_handler.window());
		play_button.draw(window);
	}



	//co_return cheese_screen(data);
}



