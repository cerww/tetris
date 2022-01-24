#pragma once
#include "screen.h"
#include "bytell_hash_map.hpp"
#include "ref_count_ptr.h"
#include "sbo_vector.h"
#include <ranges>
#include "css_stuff.h"
namespace ui_thingy_land {
	struct ui_thingy {
	private:
		struct ui_node :ref_counted {
			ui_node() = default;

			ui_node(std::string s) :
				tag_name(std::move(s)) {}

			template<typename R>
			ui_node(std::string s, R&& r) :
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

			node(ref_count_ptr<ui_node> a) :
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
	concept ui_node_c = requires(T t, event_handler_t & e)
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

		ui_node_ref(T& a) :
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


	template<typename T>
	concept css_styles = true;

	template<ui_node_c T>
	struct ui_button {

	private:

	};


	struct element_node_ref {

		template<typename T>
		element_node_ref(T thing) {
			new(m_data.data()) model_t<T>(std::move(thing));
		}

		element_node_ref(const element_node_ref& other) = delete;
		element_node_ref& operator=(const element_node_ref& other) = delete;

		/*
		screen_thingy(const screen_thingy& other) {
			other.me().copy_to(m_data.data());
		}
		*/
		element_node_ref(element_node_ref&& other) noexcept {
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

		element_node_ref& operator=(element_node_ref&& other) noexcept {
			if (this == &other) {
				return *this;
			}
			std::destroy_at(this);
			other.me().move_to(m_data.data());
			return *this;
		}

		~element_node_ref() {
			std::destroy_at(&me());
		}


		void draw(sf::RenderWindow& window) const { }

		void update_states(event_handler_t& e) const {
			/*
			if(e.current_mouse_position().x) {

			}
			*/
		}

		//why optional?
		int request_width(int parent_width, int parent_height) {
			return 0;
		}

		//recursively sets children locations too
		std::optional<std::pair<int, int>> set_location(int parent_width, int parent_height, int x, int y, int width_given, int height_given, std::optional<int> width_requested) {
			return {};
		}

		std::optional<std::string_view> key() const noexcept {
			return std::nullopt;
		}

		std::vector<element_node_ref> children() const noexcept {
			//return *std::launder((children_t*)&m_children);
		}

		display_type display_type() const noexcept {
			//return css_properties().get_display();
			return display_type::none;
		}

	private:

		struct concept_ {
			concept_() = default;

			concept_(const concept_&) = delete;
			concept_& operator=(const concept_&) = delete;

			concept_& operator=(concept_&&) = delete;
			concept_(const concept_&&) = delete;

			virtual ~concept_() = default;

			virtual void draw(sf::RenderWindow& window) const = 0;

			virtual void update_states(event_handler_t& e) const = 0;

			virtual int request_width(int parent_width, int parent_height) = 0;

			//recursively sets children locations too
			virtual std::optional<std::pair<int, int>> set_location(int parent_width, int parent_height, int x, int y, int width_given, int height_given, int width_requested) = 0;

			virtual std::optional<std::string_view> key() const noexcept = 0;

			virtual std::vector<element_node_ref> children() const noexcept = 0;
			virtual ::display_type display_type() const noexcept = 0;

			virtual void move_to(void* ptr) = 0;
		};

		template<typename T>//is sbo
		struct model_t final :concept_ {
			model_t(T* thing) :
				m_me(thing) { }


			void draw(sf::RenderWindow& window) const override {
				m_me->draw(window);
			}

			void update_states(event_handler_t& e) const override {
				m_me->update_states(e);
			}

			int request_width(int parent_width, int parent_height) override {
				return m_me->request_width(parent_width, parent_height);
			}

			std::optional<std::pair<int, int>> set_location(int parent_width, int parent_height, int x, int y, int width_given, int height_given, int width_requested) override {
				return m_me->set_location(parent_width, parent_height, x, y, width_given, height_given, width_requested);
			}

			std::optional<std::string_view> key() const noexcept override {
				return m_me->key();
			}

			std::vector<element_node_ref> children() const noexcept override {
				return m_me->children();
			}

			::display_type display_type() const noexcept override {
				return m_me->display_type();
			}

			void move_to(void* ptr) override {
				new(ptr) model_t(std::move(*this));
			}

		private:
			T* m_me;
		};

		concept_& me() {
			return *std::launder(reinterpret_cast<concept_*>(m_data.data()));
		}

		const concept_& me() const {
			return *std::launder(reinterpret_cast<const concept_*>(m_data.data()));
		}

		alignas(std::max_align_t) std::array<std::byte, 24> m_data = {};

	};

	template<typename T>
	concept element_land = requires(T t)
	{
		{ t.draw(std::declval<sf::RenderWindow>()) };
		{ t.request_width(1, 1) }->std::same_as<int>;
		{ t.update(std::declval<event_handler_t>()) };
	};

	struct text_node {


		void update(event_handler_t& e) const {
			//do nothing
		}

		int request_width(int, int) {
			return 0;
		}

		void draw(sf::RenderWindow& window) {
			//;-;
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
	struct element_node;

	template<typename T>
	struct is_element_node :std::false_type {};

	template<typename ...T>
	struct is_element_node<element_node<T...>> :std::true_type {};

	template<typename children, typename hover, typename click, typename hover_off, typename css>
	struct element_data_types {
		using children_t = children;
		using hover_t = hover;
		using click_t = click;
		using hover_off_t = hover_off;
		using css_t = css;
	};

	template<typename T>
	struct is_css_thing :std::false_type {};

	template<typename ...T>
	struct is_css_thing<css_stuff<T...>> :std::true_type {};

	template<typename F, typename...Ts, typename...children, typename... hover, typename...click, typename...hover_off, typename css>
	auto make_data_stuff(typelist<F, Ts...> rest, typelist<children...>, typelist<hover...>, typelist<click...>, typelist<hover_off...>, typelist<css>) {
		if constexpr (sizeof...(Ts) == 0) {
			if constexpr (is_element_node<F>::value) {
				return element_data_types<std::tuple<children..., F>, std::tuple<hover...>, std::tuple<click...>, std::tuple<hover_off...>, css>();
			}
			else if constexpr (is_css_thing<F>::value) {
				return element_data_types<std::tuple<children...>, std::tuple<hover...>, std::tuple<click...>, std::tuple<hover_off...>,
					decltype(combine_css_f::combine_with(std::declval<css>(), std::declval<F>()))>();
			}
			else if constexpr (std::convertible_to<F, std::string>) {
				return element_data_types<std::tuple<children..., text_node>, std::tuple<hover...>, std::tuple<click...>, std::tuple<hover_off...>, css>();
			}
			else if constexpr (is_specialization<F, on_click>::value) {
				return element_data_types<std::tuple<children...>, std::tuple<hover...>, std::tuple<click..., F>, std::tuple<hover_off...>, css>();
			}
			else if constexpr (is_specialization<F, on_mouseoff>::value) {
				return element_data_types<std::tuple<children...>, std::tuple<hover...>, std::tuple<click...>, std::tuple<hover_off..., F>, css>();
			}
			else if constexpr (is_specialization<F, on_mouseover>::value) {
				return element_data_types<std::tuple<children...>, std::tuple<hover..., F>, std::tuple<click...>, std::tuple<hover_off...>, css>();
			}
			else if constexpr (is_specialization<F, on_click>::value) {
				return element_data_types<std::tuple<children...>, std::tuple<hover...>, std::tuple<click..., F>, std::tuple<hover_off...>, css>();
			}
		}
		else {
			if constexpr (is_element_node<F>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children..., F>(), typelist<hover...>(), typelist<click...>(), typelist<hover_off...>(), typelist<css>());
			}
			else if constexpr (is_css_thing<F>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children...>(), typelist<hover...>(), typelist<click...>(), typelist<hover_off...>(),
					typelist<decltype(combine_css_f::combine_with(std::declval<css>(), std::declval<F>()))>());
			}
			else if constexpr (std::convertible_to<F, std::string>) {
				return make_data_stuff(typelist<Ts...>(), typelist<children..., text_node>(), typelist<hover...>(), typelist<click...>(), typelist<hover_off...>(), typelist<css...>());
			}
			else if constexpr (is_specialization<F, on_click>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children...>(), typelist<hover...>(), typelist<click..., F>(), typelist<hover_off...>(), typelist<css...>());
			}
			else if constexpr (is_specialization<F, on_mouseoff>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children...>(), typelist<hover...>(), typelist<click...>(), typelist<hover_off..., F>(), typelist<css...>());
			}
			else if constexpr (is_specialization<F, on_mouseover>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children...>(), typelist<hover..., F>(), typelist<click...>(), typelist<hover_off...>(), typelist<css...>());
			}
			else if constexpr (is_specialization<F, on_click>::value) {
				return make_data_stuff(typelist<Ts...>(), typelist<children...>(), typelist<hover...>(), typelist<click..., F>(), typelist<hover_off...>(), typelist<css...>());
			}
		}
	}

	template<typename... Ts>
	struct element_node {

		void draw(sf::RenderWindow& window) const { }

		void update_states(event_handler_t& e) const {
			/*
			if(e.current_mouse_position().x) {

			}
			*/
		}

		int request_width(int parent_width, int parent_height) {
			if (m_css_properties.get_display() == display_type::inline_) {
				return compute_wanted_width(parent_width, parent_height);
			}
			if (m_css_properties.get_display() == display_type::inline_block) {
				const auto width = m_css_properties.get_width(parent_width);
				if (width == auto_()) {
					const auto max_width = calculate_value(m_css_properties.get_max_width(parent_width), parent_width);
					const auto min_width = calculate_width_value(m_css_properties.get_min_width(parent_width), parent_width);
					const auto total_width = compute_wanted_width(parent_width, parent_height);
					return std::clamp(total_width, min_width.value_or(0), max_width.value_or(500000000));
				}
				else {
					const auto width_pixels = calculate_width_value(width, parent_width);
					const auto max_width = calculate_value(m_css_properties.get_max_width(parent_width), parent_width);
					const auto min_width = calculate_width_value(m_css_properties.get_min_width(parent_width), parent_width);
					return std::clamp(width_pixels.value(), min_width.value_or(0), max_width.value_or(500000000));
				}
			}

			if (m_css_properties.get_display() == display_type::block || m_css_properties.get_display() == display_type::grid) {
				const auto width = calculate_width_value(m_css_properties.get_width(parent_width), parent_width);
				const auto min_width = calculate_width_value(m_css_properties.get_min_width(parent_width), parent_width);
				const auto max_width = calculate_width_value(m_css_properties.get_max_width(parent_width), parent_width);

				return std::clamp(width.value(), min_width.value_or(0), max_width.value_or(500000000));
			}
			else if (m_css_properties.get_display() == display_type::none) {
				return 0;
			}
			else {
				return 0;
			}
		}

		//recursively sets children locations too
		std::optional<std::pair<int, int>> set_location(int parent_width, int parent_height, int x, int y, int width_given, int height_given, std::optional<int> width_requested) {
			return {};
		}

		std::optional<std::string_view> key() const noexcept {
			return std::nullopt;
		}

		std::vector<element_node_ref> children() const noexcept {

			return {};
		}

		const auto& children2() const noexcept {
			return *std::launder((children_t*)&m_children);
		}

		display_type display_type() const noexcept {
			return css_properties().get_display();
		}

	private:
		using data_stuff = decltype(make_data_stuff(typelist<Ts...>(), typelist<>(), typelist<>(), typelist<>(), typelist<>(), typelist<>()));
		using children_t = typename data_stuff::children_t;
		using hover_t = typename data_stuff::hover_t;
		using click_t = typename data_stuff::click_t;
		using hover_off_t = typename data_stuff::hover_off_t;
		using css_t = typename data_stuff::css_t;

		std::optional<int> calculate_width_value(std::optional<pixel_percent_auto> val, int parent_width) const noexcept {
			if (!val) {
				return std::nullopt;
			}
			if (m_css_properties.get_width(parent_width) == auto_()) {
				return parent_width;
			}
			else {
				if (val.value().catagory() == percent_bit) {
					return val.value().value_part() / 10000.0 * parent_width;
				}
				else {
					return val.value().value_part();
				}
			}
		}

		int compute_wanted_width(int parent_width, int parent_height) {
			int current_iter_width = 0;
			int current_max_width = 0;

			const auto do_thing = [&](const auto& me, const auto& node) {
				if (node.display_type() == display_type::contents) {
					if constexpr (requires  { {node.children2()}; }) {
						std::apply([&](auto&&... child) {
							(me(me, child), ...);
							}, node.children2());
					}
					else {
						//node.children();
						const auto childrenland = node.children();
						for (const auto& child : childrenland) {
							me(me, child);
						}
					}
				}
				else if (is_inline(node.display_type())) {
					current_iter_width += node.request_width(parent_width, parent_height);
				}
				else {//is_block
					current_max_width = std::max(std::exchange(current_iter_width, 0), std::max(current_max_width, node.request_width(parent_width, parent_height)));
				}
			};

			std::apply([&](auto&&... wat) {
				(do_thing(do_thing, wat), ...);
				}, childrens());

			return std::max(current_iter_width, current_max_width);
		}

		bool m_is_hover = false;
		bool m_is_active = false;

		int m_x = 0;
		int m_y = 0;
		int m_width = 0;
		int m_height = 0;

		const auto& hover() const noexcept {
			return *std::launder((hover_t*)&m_hover);
		}

		auto& childrens() noexcept {
			return *std::launder((children_t*)&m_children);
		}

		const auto& click() const noexcept {
			return *std::launder((click_t*)&m_click);
		}

		const auto& hover_off() const noexcept {
			return *std::launder((children_t*)&m_children);
		}

		const auto& css_properties() const noexcept {
			return *std::launder((css_t*)&m_css_properties);
		}

		alignas(children_t) std::array<std::byte, sizeof(children_t)> m_children;
		alignas(hover_t)std::array<std::byte, sizeof(hover_t)> m_hover;
		alignas(click_t)std::array<std::byte, sizeof(click_t)> m_click;
		alignas(hover_off_t)std::array<std::byte, sizeof(hover_off_t)> m_hover_off;
		alignas(css_t)std::array<std::byte, sizeof(css_t)> m_css_properties;

	};

	template<typename... T>
	struct branch { };


	template<typename T>
	struct html_doc {
		draw_stuff_result draw(sf::RenderWindow& window, const ui_stuff thing) const {
			css_properties_stack properties;
		}


		void update(event_handler_t& e) const { }

	private:
	};


	inline void testlanda() {
		auto y = css_stuff().display(display_type::block).width(pixel(500)).display(display_type::grid).height(pixel(203));
	}
}