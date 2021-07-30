#pragma once
#include "game_data.h"
#include "sfml_event_handler_extensions.h"

using event_handler_t = sfml_event_handler<track_hold_times<>, track_mouse_pos>;

struct screen_thingy;

template<typename T>
concept scene = requires(T t, event_handler_t & a, game_data& b)
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


	std::optional<screen_thingy> update(event_handler_t& a, game_data& b) {
		std::optional<screen_thingy> ret;
		me().update(a, b, ret);
		return ret;
	}

private:

	struct concept_ {
		virtual ~concept_() = default;

		virtual void update(event_handler_t&, game_data&, std::optional<screen_thingy>& out) = 0;
		virtual void move_to(void*) = 0;
		virtual void copy_to(void*) const = 0;
	};


	template<typename T, bool>//not sbo
	struct model_t final :concept_ {
		model_t(T thing):
			m_me(std::make_unique<T>(std::move(thing))) { }

		void update(event_handler_t& a, game_data& b, std::optional<screen_thingy>& out) override {
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

		void update(event_handler_t& a, game_data& b, std::optional<screen_thingy>& out) override {
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


	std::optional<screen_thingy> update(event_handler_t& event_handler, game_data& stuff) {
		return std::apply([&](auto&... wat) {
			return update_impl(event_handler, stuff, wat...);
		},inner_screens);
		
	}

	template<typename F,typename... R>
	std::optional<screen_thingy> update_impl(event_handler_t& event_handler, game_data& stuff,F& first,R&... r) {
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







