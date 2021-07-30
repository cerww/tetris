#pragma once
#include "screen.h"
#include "bytell_hash_map.hpp"
#include "ref_count_ptr.h"
#include "sbo_vector.h"
#include <ranges>

struct ui_thingy {
private:
	struct ui_node :ref_counted {
		ui_node() = default;

		ui_node(std::string s):
			tag_name(std::move(s)) {}

		template<typename R>
		ui_node(std::string s, R&& r):
			tag_name(std::move(s)),
			properties(r | std::views::transform([](auto&& a) {
				auto&& [name, value] = a;
				return std::pair(std::string(name), std::string(value));

			})) {}

		std::string tag_name;
		ska::bytell_hash_map<std::string, std::string> properties;
		sbo_vector<ref_count_ptr<ui_node>, 10> children;
	};


public:
	struct node {
		node() = default;

		node(ref_count_ptr<ui_node> a):
			m_me(std::move(a)) {}


	private:
		ref_count_ptr<ui_node> m_me = nullptr;
		friend ui_thingy;
	};

	void C(std::string tag_name) {
		auto n = make_ref_count_ptr<ui_node>(std::move(tag_name));
		if (!m_current_stack.empty()) {
			m_current_stack.back()->children.push_back(n);
		}
		m_current_stack.push_back(n);
		//return n;
	}

	template<typename R>
	void C(std::string tag_name, R&& properties) {
		auto n = make_ref_count_ptr<ui_node>(std::move(tag_name), std::forward<R>(properties));
		if (!m_current_stack.empty()) {
			m_current_stack.back()->children.push_back(n);
		}
		m_current_stack.push_back(n);
		//return n;
	}

	void add(const node& n) {
		m_current_stack.back()->children.push_back(n.m_me);
	}

	node E(std::string_view tag_name) {
		if (m_current_stack.empty() || m_current_stack.back()->tag_name != tag_name) {
			throw std::runtime_error("wat");
		}
		auto r = std::move(m_current_stack.back());
		m_current_stack.pop_back();
		return r;
	}


private:
	std::unique_ptr<ui_node> m_root_node = std::make_unique<ui_node>();
	std::vector<ref_count_ptr<ui_node>> m_current_stack;

};


struct no_properties {};

struct nodeland {

	nodeland() = default;


private:
	std::string m_tag_name;
	ska::bytell_hash_map<std::string, std::string> m_properties;
	sbo_vector<nodeland, 10> m_children;
};


struct ui_stuff {
	int x_start = 0;
	int y_start = 0;
	int parent_width = 0;
	int parent_height = 0;
};

struct draw_stuff_result {
	int height_used = 0;
	int width_used = 0;
	
};

struct ui_node {

	template<typename T>
	ui_node(T thing) {
		using type = model_t<T, sizeof(model_t<T, true>) <= sizeof(m_data)>;
		new(m_data.data()) type(std::move(thing));
	}

	ui_node(const ui_node& other) = delete;
	ui_node& operator=(const ui_node& other) = delete;

	/*
	screen_thingy(const screen_thingy& other) {
		other.me().copy_to(m_data.data());
	}
	*/
	ui_node(ui_node&& other) noexcept {
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

	ui_node& operator=(ui_node&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		std::destroy_at(this);
		other.me().move_to(m_data.data());
		return *this;
	}

	~ui_node() {
		std::destroy_at(&me());
	}


	void update(event_handler_t& a) {
		me().update(a);		
	}

	void add_node(ui_node new_node) {
		me().add_node(std::move(new_node));
	}

	draw_stuff_result draw(sf::RenderWindow& w, const ui_stuff& a) {
		return me().draw(w, a);
	}

	draw_stuff_result update_and_draw(event_handler_t& a, sf::RenderWindow& w, const ui_stuff& ui_data) {
		return {};
	}

private:

	struct concept_ {
		concept_() = default;

		concept_(const concept_&) = delete;
		concept_& operator=(const concept_&) = delete;
		
		concept_& operator=(concept_&&) = delete;
		concept_(const concept_&&) = delete;

		virtual ~concept_() = default;

		virtual void update(event_handler_t&) = 0;
		virtual draw_stuff_result draw(sf::RenderWindow&, const ui_stuff&) = 0;
		virtual void add_node(ui_node) = 0;
		
		virtual void move_to(void*) = 0;
		virtual void copy_to(void*) const = 0;//remove this?
	};


	template<typename T, bool>//not sbo
	struct model_t final :concept_ {
		model_t(T thing) :
			m_me(std::make_unique<T>(std::move(thing))) { }

		void update(event_handler_t& e) override {
			m_me->update(e);
		};
		
		draw_stuff_result draw(sf::RenderWindow& w, const ui_stuff& a) override {
			return m_me->draw(w, std::move(a));
		}

		void add_node(ui_node w) override {
			m_me->add_node(w);
		}

		void move_to(void* ptr) override {
			new(ptr) model_t(std::move(*this));
		}

		void copy_to(void* ptr) const override {
			//new(ptr) model_t(*m_me);
		}

	private:
		std::unique_ptr<T> m_me;
	};

	template<typename T>//is sbo
	struct model_t<T, true> final :concept_ {
		model_t(T thing) :
			m_me(std::move(thing)) { }

		void update(event_handler_t& e) override{
			m_me.update(e);
		};

		draw_stuff_result draw(sf::RenderWindow& w, const ui_stuff& a) override {
			return m_me.draw(w, std::move(a));
		}

		void move_to(void* ptr) override {
			new(ptr) model_t(std::move(*this));
		}

		void add_node(ui_node w) override {
			m_me.add_node(w);
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

	alignas(std::max_align_t) std::array<std::byte, 256> m_data = {};

};


template<typename T>
concept ui_node_c = requires(T t, event_handler_t & e, ui_node n)
{
	{ t.update(e) }->std::same_as<void>;
	{ t.draw(e.window(), ui_stuff()) }->std::same_as<draw_stuff_result>;
	{ t.add_node(std::move(t)) }->std::same_as<void>;
	{ t.add_node(n) }->std::same_as<void>;
};


struct root_node_t {

	void add_node(ui_node new_node) {
		children.push_back(std::move(new_node));
	}

	void draw() {
		//for()
	}
	
	std::vector<ui_node> children;
};



struct ui_tree {




	void add(ui_node node) {
		
	}
	
	
private:
	root_node_t m_root;

	std::vector<ui_node*> m_current_stack;
	
	
};
