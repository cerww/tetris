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
	std::vector<nodeland> m_children;
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


template<typename T>
concept ui_node_c = requires(T t, event_handler_t& e)
{
	{ t.update(e) }->std::same_as<void>;
	{ t.draw(e.window(), ui_stuff()) }->std::same_as<draw_stuff_result>;
	//update and draw?
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

	/*
	void add_node(ui_node new_node) {
		me().add_node(std::move(new_node));
	}
	*/

	draw_stuff_result draw(sf::RenderWindow& w, const ui_stuff& a) {
		return me().draw(w, a);
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
		//virtual void add_node(ui_node) = 0;

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

		/*
		void add_node(ui_node w) override {
			m_me->add_node(w);
		}
		*/
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

		void update(event_handler_t& e) override {
			m_me.update(e);
		};

		draw_stuff_result draw(sf::RenderWindow& w, const ui_stuff& a) override {
			return m_me.draw(w, std::move(a));
		}

		void move_to(void* ptr) override {
			new(ptr) model_t(std::move(*this));
		}

		/*
		void add_node(ui_node w) override {
			m_me.add_node(w);
		}
		*/

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

struct root_node_t {

	void add_node(ui_node new_node) {
		children.push_back(std::move(new_node));
	}

	draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing) {
		ui_stuff thing_so_far = thing;
		int max_hieght_in_row = 0;
		for (auto& child : children) {
			const auto res = child.draw(window, thing_so_far);
			thing_so_far.x_start += res.width_used;
			max_hieght_in_row = std::max(max_hieght_in_row, res.height_used + thing_so_far.y_start);
			if (thing_so_far.x_start >= thing.parent_width) {
				thing_so_far.x_start = 0;
				thing_so_far.y_start = max_hieght_in_row + thing_so_far.y_start;
			}

		}
	}

	void update(event_handler_t& e) {
		for (auto& child : children) {
			child.update(e);
		}
	}

	std::vector<ui_node> children;
};

struct ui_tree {
	void add_node(ui_node node) {
		m_root.add_node(std::move(node));
	}

	template<typename... T>
	void add_nodes(T&&... nodes) {
		(m_root.add_node(ui_node(std::move(nodes))), ...);
	}

	void update(event_handler_t& event_handler) {
		m_root.update(event_handler);
	}


private:
	root_node_t m_root;
};


template<ui_node_c T>
struct ui_node_ref {

	ui_node_ref(T& a):
		m_me(a) {}

	draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing) const {
		return m_me.draw(window, thing);
	}

	void update(event_handler_t& e) const {
		m_me.update(e);
	}


private:
	T& m_me;
};

enum struct display_type:int8_t {
	inline_,
	block,
	grid,
};

struct default_land {};


template<typename T>
struct with_colour {
	static constexpr int id_land = 0;
};

template<typename T>
struct with_font_size {
	static constexpr int id_land = 1;
};

template<typename T>
struct with_margin_left {
	static constexpr int id_land = 2;
};

template<typename T>
struct with_margin_right {
	static constexpr int id_land = 3;
};

template<typename T>
struct with_margin_top {
	static constexpr int id_land = 4;
};

template<typename T>
struct with_margin_bottom {
	static constexpr int id_land = 5;
};

template<typename T>
struct with_padding_left {
	static constexpr int id_land = 6;
};

template<typename T>
struct with_padding_right {
	static constexpr int id_land = 7;
};

template<typename T>
struct with_padding_top {
	static constexpr int id_land = 8;
};

template<typename T>
struct with_padding_bottom {
	static constexpr int id_land = 9;
};

template<typename T>
struct with_min_width {
	static constexpr int id_land = 9;
};

template<typename T>
struct with_width {
	static constexpr int id_land = 9;
	T a;
};

template<typename T>
struct with_max_width {
	static constexpr int id_land = 9;
};

template<typename T>
struct with_min_height {
	static constexpr int id_land = 9;
};

template<typename T>
struct with_height {
	static constexpr int id_land = 9;
	T a;
};

template<typename T>
struct with_max_height {
	static constexpr int id_land = 9;
};


template<typename T>
struct with_display {
	static constexpr int id_land = 9;
	T thing;
};




template<typename T, template<typename> typename W>
struct is_specialization :std::false_type { };

template<typename T, template<typename> typename W>
struct is_specialization<W<T>, W> :std::true_type { };

template<typename T, typename U>
struct same_template :std::false_type {};

template<typename T, typename U, template<typename>typename W>
struct same_template<W<T>, W<U>> :std::true_type {};

template<typename T, template<typename> typename W>
constexpr bool is_specialization_v = is_specialization<T, W>::value;

static_assert(is_specialization_v<with_max_height<int>, with_max_height>);

static_assert(same_template<with_max_height<int>,with_max_height<double>>::value);

static_assert(!same_template<with_max_height<int>, with_max_width<double>>::value);

template<typename...Ts>
struct typelist {
	static constexpr bool empty = !(sizeof...(Ts));
};

static_assert(typelist<>::empty);

template<typename... T>
struct css_stuff {
	css_stuff() = default;

	auto display(display_type wat) {
		return combine_with(with_display<display_type>(wat));
	}

	auto width(int wat) {
		return combine_with(with_width<int>(wat));
	}

	auto height(int wat) {
		return combine_with(with_height<int>(wat));
	}
	

private:

	explicit css_stuff(std::tuple<T...> a) :
		m_properties(std::move(a)) {}

	template<typename...U>
	static auto from_tuple(std::tuple<U...> thing) {
		return css_stuff<U...>(std::move(thing));
	}

	template<typename U>
	auto combine_with(U new_property) {
		if constexpr (sizeof...(T) == 0) {
			return from_tuple(std::make_tuple(std::move(new_property)));
		}else {
			using new_type = decltype(combined_type(typelist<U>(), typelist<T...>(), typelist<>()));
			new_type wat;
			wat.steal_stuff(m_properties, new_property);
			return wat.to_thing();
		}
	}

	template<typename...>
	friend struct css_stuff_fake;

	template<typename...>
	friend struct css_stuff;

	template<typename...U>
	struct css_stuff_fake {
		alignas(std::tuple<U...>) std::array<std::byte, sizeof(std::tuple<U...>)>thing;

		css_stuff<U...> to_thing() {
			return css_stuff<U...>(as_thing());
		}	

		std::tuple<U...>& as_thing() {
			return *std::launder((std::tuple<U...>*)&thing);
		}

		template<typename... Ts,typename O>
		void steal_stuff(std::tuple<Ts...>& other,O& wat) {
			new (&std::get<O>(as_thing())) O(std::move(wat));
			((new (&std::get<Ts>(as_thing())) Ts(std::move(std::get<Ts>(other)))), ...);
		}
	};


	template<typename U, typename F, typename...R, typename...Current>
	static auto combined_type(typelist<U>, typelist<F, R...>, typelist<Current...>) {
		if constexpr (same_template<U, F>::value) {
			return css_stuff_fake<Current..., U, R...>();
		} else {
			if constexpr (sizeof...(R) == 0) {
				return css_stuff_fake<Current..., F,U>();
			} else {
				return combined_type(typelist<U>(), typelist<R...>(), typelist<Current..., F>());
			}
		}
	}

	std::tuple<T...> m_properties;
};

template<typename...T>
css_stuff(std::tuple<T...>)->css_stuff<T...>;

struct css_properties {
	//colour
	//bg colour
	//font-size
	//margins
	//padding
	//border
	//border style
	//border radius
	//display: grid,inline,block;no inline-grid
	//(max/min)width
	//(max/min)height
	//grid row gap
	//grid column gap
	//grid template columns
	//grid template rows
	//grid row/column span
	//grid column gap background
	//grid row gap background
	//cursor
	//place items:left,right,center
	//
	//hover properties
	//active properties
};

template<typename T>
concept css_styles = true;

template<typename T>
concept css_ui_node = requires(T t, css_properties c, sf::RenderWindow& w, ui_stuff b)
{
	{ t.draw(w, b, c) }->std::same_as<draw_stuff_result>;
};

template<ui_node_c T>
struct ui_button {

private:

};


struct text_node {
	draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing) const {
		return {};
	}

	draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing, css_properties& properties) const {
		return {};
		//return m_me.draw(window, thing);		
	}

	void update(event_handler_t& e) const {
		//do nothing
	}

private:
	std::string m_text;

};

template<typename T>
struct on_mouseover {
	using mouseover = std::true_type;
	T fn;
};

template<typename T>
struct on_mouseoff {
	using mouseoff = std::true_type;
	T fn;
};

template<typename T>
struct on_click {
	using click = std::true_type;
	T fn;
};

template<typename... Ts>
struct element_node {

	void draw(sf::RenderWindow& window, const ui_stuff thing, css_properties& properties) const { }

	void update(event_handler_t& e) const {
		/*
		if(e.current_mouse_position().x) {
			
		}
		*/
	}

	std::pair<std::optional<int>, std::optional<int>> calculate_position(int parent_width, int parent_height, int wanted_x, int wanted_y) {
		return {};
	}

	std::pair<std::optional<int>, std::optional<int>> wanted_dimensions(int parent_width, int parent_height) {
		return {};
	}

	std::pair<int, int> set_location(int parent_width, int parent_height, int x, int y) {
		return {};
	}

private:
	bool m_hover = false;
	bool m_active = false;
	std::tuple<Ts...> m_children;
	css_stuff<Ts...> m_css_properties;
};


template<typename T>
struct html_doc {
	draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing) const {
		css_properties properties;
	}


	void update(event_handler_t& e) const { }

private:
};


void testlanda() {
	auto y = css_stuff().display(display_type::block).width(500).display(display_type::grid).height(203);
}
