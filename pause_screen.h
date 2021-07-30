#pragma once
#include "screen.h"


template<typename T>
concept unpause_policy = std::regular_invocable<T, event_handler_t&, game_data&> && std::same_as<std::invoke_result_t<T, event_handler_t&, game_data&>, bool>;


template<typename T>
concept pause_policy = std::regular_invocable<T, event_handler_t&, game_data&> && std::same_as<std::invoke_result_t<T, event_handler_t&, game_data&>, bool>;

template<typename T,unpause_policy... Ps>
struct pause_screen {


	pause_screen(T thing, Ps ... ps):
		paused_screen(std::move(thing)), unpause_policies(std::move(ps)...) { }

	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& stuff) {
		if (std::apply([&](auto&... policy) {
			return (std::invoke(policy, event_handler, stuff) || ...);
			}, unpause_policies)) {
			return std::move(paused_screen);
		}
		return std::nullopt;
	}


	T paused_screen;
	std::tuple<Ps...> unpause_policies;
};

//bad! use CRTP?
template<typename T,pause_policy... Ps>
struct not_pause_screen {

	not_pause_screen(T& thing, Ps ... ps) :
		me(thing),
		pause_policies(std::move(ps)...) {}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& stuff) {
		if(std::apply([&](auto&... policy) {
			return (std::invoke(policy,event_handler,stuff) || ...);
		},pause_policies)) {
			//wat ;-;
		}
		return std::nullopt;
	}

	T& me;
	std::tuple<Ps...> pause_policies;
};
