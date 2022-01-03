#pragma once
#include <coroutine>
#include <iostream>
#include "game_data.h"
#include "sfml_event_handler_extensions.h"

using event_handler_t = sfml_event_handler<track_hold_times<>, track_mouse_pos,track_text>;

struct screen_thingy;

template<typename T>
concept scene = requires(T t, event_handler_t & a, game_keybinds& b)
{
	{ t.update(a, b) }->std::same_as<std::optional<screen_thingy>>;
};

struct screen_thingy {

	template<scene T>
	screen_thingy(T thing) {
		using type = model_t<T, sizeof(model_t<T, true>) <= sizeof(m_data)>;
		new(m_data.data()) type(std::move(thing));
	}
	screen_thingy(const screen_thingy& other) = delete;
	screen_thingy& operator=(const screen_thingy& other) = delete;
	
	/*
	screen_thingy(const screen_thingy& other) {
		other.me().copy_to(m_data.data());
	}
	*/
	screen_thingy(screen_thingy&& other) noexcept {
		other.me().move_to(m_data.data());
	}
	
	/*
	screen_thingy& operator=(const screen_thingy& other) {
		if (this == &other) {
			return *this;
		}
		std::destroy_at(this);
		other.me().copy_to(m_data.data());
		return *this;
	}
	*/

	screen_thingy& operator=(screen_thingy&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		std::destroy_at(this);
		other.me().move_to(m_data.data());
		return *this;
	}

	~screen_thingy() {
		std::destroy_at(&me());
	}


	std::optional<screen_thingy> update(event_handler_t& a, game_keybinds& b) {
		std::optional<screen_thingy> ret;
		me().update(a, b, ret);
		return ret;
	}

private:

	struct concept_ {
		virtual ~concept_() = default;

		virtual void update(event_handler_t&, game_keybinds&, std::optional<screen_thingy>& out) = 0;
		virtual void move_to(void*) = 0;
		virtual void copy_to(void*) const = 0;
	};


	template<typename T, bool>//not sbo
	struct model_t final :concept_ {
		model_t(T thing):
			m_me(std::make_unique<T>(std::move(thing))) { }

		void update(event_handler_t& a, game_keybinds& b, std::optional<screen_thingy>& out) override {
			out = m_me->update(a, b);
		}

		void move_to(void* ptr)override {
			new(ptr) model_t(std::move(*this));
		}

		void copy_to(void* ptr) const override{
			//new(ptr) model_t(*m_me);
		}

	private:
		std::unique_ptr<T> m_me;
	};

	template<typename T>//is sbo
	struct model_t<T, true> final :concept_ {


		model_t(T thing) :
			m_me(std::move(thing)) { }

		void update(event_handler_t& a, game_keybinds& b, std::optional<screen_thingy>& out) override {
			out = m_me.update(a, b);
		}

		void move_to(void* ptr)override {
			new(ptr) model_t(std::move(*this));
		}

		void copy_to(void* ptr) const override {
			//new(ptr) model_t(m_me);
		}

	private:
		T m_me;
	};

	concept_& me() {
		return *std::launder(reinterpret_cast<concept_*>(m_data.data()));
	}

	const concept_& me() const {
		return *std::launder(reinterpret_cast<const concept_*>(m_data.data()));
	}

	alignas(std::max_align_t) std::array<std::byte, 1024> m_data = {};
};


template<typename... Ts>
struct multiple_screens {
	multiple_screens(Ts... things) :inner_screens(std::move(things)...) {}


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& stuff) {
		return std::apply([&](auto&... wat) {
			return update_impl(event_handler, stuff, wat...);
		},inner_screens);
		
	}

	template<typename F,typename... R>
	std::optional<screen_thingy> update_impl(event_handler_t& event_handler, game_keybinds& stuff,F& first,R&... r) {
		const auto res = first.update(event_handler, stuff);
		if(res) {
			return res;
		}else {
			if constexpr (sizeof...(R) >= 1) {
				return update_impl(event_handler, stuff, r...);
			}else {
				return std::nullopt;
			}
		}
	}
	
	std::tuple<Ts...> inner_screens;
};


struct coroutine_screen {
	struct all_data_and_stuff {
		std::coroutine_handle<> h;

		event_handler_t* next_event_handler = nullptr;
		game_keybinds* next_stuff = nullptr;

		std::optional<std::optional<screen_thingy>> next_screen = std::optional<std::optional<screen_thingy>>(std::optional<screen_thingy>(std::nullopt));
	};

	struct promise_type {
		promise_type() = default;
		coroutine_screen get_return_object() noexcept {
			stuff.h = std::coroutine_handle<promise_type>::from_promise(*this);
			return coroutine_screen(&stuff);
		}

		std::suspend_never initial_suspend()const noexcept {
			return {};
		}

		std::suspend_always final_suspend()const noexcept {
			return {};
		}

		void unhandled_exception() noexcept{
			//wat
		}

		void return_value(screen_thingy screen) {
			stuff.next_screen.emplace(std::optional(std::move(screen)));
		}

		all_data_and_stuff stuff;
	};


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_keybinds& stuff) {

		m_thing->next_event_handler = &event_handler;
		m_thing->next_stuff = &stuff;

		m_thing->h.resume();
		m_thing->next_event_handler = nullptr;
		m_thing->next_stuff = nullptr;
		//std::cout << "b" << std::endl;

		if(m_thing->next_screen.has_value()) {
			auto ret = std::move(*m_thing->next_screen);
			m_thing->next_screen = std::nullopt;
			return ret;
		}else {
			return std::nullopt;
			throw std::runtime_error("wat");
		}
	}

	coroutine_screen(coroutine_screen&& other)noexcept:m_thing(std::exchange(other.m_thing,nullptr)) {}
	
	coroutine_screen& operator=(coroutine_screen&& other)noexcept {
		if(this == &other) {
			return *this;
		}
		if (m_thing) {
			m_thing->h.destroy();
		}
		m_thing = std::exchange(other.m_thing, nullptr);

		return *this;
	}

	~coroutine_screen() {
		if (m_thing) {
			m_thing->h.destroy();
		}
	}

	coroutine_screen(const coroutine_screen&) = delete;
	coroutine_screen& operator=(const coroutine_screen&) = delete;
private:
	
	explicit coroutine_screen(all_data_and_stuff* a):m_thing(a) {

	}

	all_data_and_stuff* m_thing;
};

struct wait_for_next_update {
	bool await_ready() {
		return false;
	}

	void await_suspend(std::coroutine_handle<coroutine_screen::promise_type> h) {
		m_data = &h.promise().stuff;
	}

	std::tuple<event_handler_t&, game_keybinds&> await_resume() {
		return std::tie(*m_data->next_event_handler,*m_data->next_stuff);
	}

private:
	const coroutine_screen::all_data_and_stuff* m_data = nullptr;

};

/*

coroutine_screen fn_name(args){
	while(true){
		auto&[event_handler,settings] = co_await thing;
		everything
		draw
		co_yield next screen 
	}
	co_return;
}

 */