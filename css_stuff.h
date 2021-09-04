#pragma once
#include <tuple>
#include <optional>

constexpr int64_t percent_bit = 1ull << 60;
constexpr int64_t fr_bit = 1ull << 61;
constexpr int64_t auto_bit = 1ull << 62;
constexpr int64_t data_bits = (1ull << 50) - 1;

struct pixel {
	pixel() = default;

	explicit pixel(int64_t a) :
		m_amount(a & data_bits) {}

	int64_t value() const noexcept {
		return m_amount;
	}

	int64_t catagory() const noexcept {
		return m_amount & ~data_bits;
	}

	int64_t value_part() const noexcept {
		return m_amount & data_bits;
	}


private:
	int64_t m_amount = 0;
};

struct percent {
	percent() = default;

	explicit percent(int64_t a) :
		m_amount((a * 10000) | percent_bit) {}

	explicit percent(double a) :
		m_amount(static_cast<int>(a * 10000) | percent_bit) {}

	int64_t value() const noexcept {
		return m_amount;
	}

	int64_t catagory() const noexcept {
		return m_amount & ~data_bits;
	}

	int64_t value_part() const noexcept {
		return m_amount & data_bits;
	}

private:
	int64_t m_amount = percent_bit;//value * 10000
};

struct auto_ {
	int64_t value() const noexcept {
		return auto_bit;
	}

	int64_t catagory() const noexcept {
		return auto_bit;
	}

	int64_t value_part() const noexcept {
		return 0;
	}

};

struct fr {
	fr() = default;

	explicit fr(int64_t a) :
		m_amount((a) | fr_bit) {}

	int64_t value() const noexcept {
		return m_amount;
	}

	int64_t catagory() const noexcept {
		return m_amount & ~data_bits;
	}

	int64_t value_part() const noexcept {
		return m_amount & data_bits;
	}

private:
	int64_t m_amount = 0;
};

template<typename T, typename...U>
concept type_in_list = (std::same_as<T, U> || ...);

template<typename...T>
struct pixel_value_template {

	pixel_value_template() = default;

	template<type_in_list<T...> U>
	pixel_value_template(U thing) :
		m_amount(thing.value()) {}

	template<type_in_list<T...> ...Us>
	pixel_value_template(pixel_value_template<Us...> other) :
		m_amount(other.value()) { }

	int64_t value() const noexcept {
		return m_amount;
	}

	int64_t catagory() const noexcept {
		return m_amount & ~data_bits;
	}

	int64_t value_part() const noexcept {
		return m_amount & data_bits;
	}

	template<typename...U>
	bool operator==(const pixel_value_template<U...>& other) const noexcept {
		return value() == other.value();
	}

	bool operator==(const auto_& other) const noexcept {
		return value() == other.value();
	}

	bool operator==(const pixel& other) const noexcept {
		return value() == other.value();
	}

	bool operator==(const fr& other) const noexcept {
		return value() == other.value();
	}

	bool operator==(const percent& other) const noexcept {
		return value() == other.value();
	}

private:
	int64_t m_amount = 0;
};

using pixel_percent_auto = pixel_value_template<pixel, percent, auto_>;
using pixel_percent_auto_fr = pixel_value_template<pixel, percent, auto_, fr>;
using pixel_percent = pixel_value_template<pixel, percent>;

inline pixel operator""_px(uint64_t wat) noexcept {
	return pixel(wat);
}

static_assert(std::convertible_to<pixel_percent, pixel_percent_auto>);
static_assert(std::convertible_to<pixel, pixel_percent_auto>);


constexpr bool operator==(std::convertible_to<pixel_percent_auto_fr> auto a, std::convertible_to<pixel_percent_auto_fr> auto b) {
	return pixel_percent_auto_fr(a) == pixel_percent_auto_fr(b);
}

enum struct display_type :uint8_t {
	inline_ = (1 << 7),
	block,
	grid,
	inline_block,
	none,
	contents
};

constexpr bool is_inline(display_type d)noexcept {
	return d == display_type::inline_ || d == display_type::inline_block;
}

struct default_land {};

template<typename T>
struct with_colour {};

template<typename T>
struct with_font_size {};

template<typename T>
struct with_margin_left {};

template<typename T>
struct with_margin_right {};

template<typename T>
struct with_margin_top {};

template<typename T>
struct with_margin_bottom {};

template<typename T>
struct with_padding_left {};

template<typename T>
struct with_padding_right {};

template<typename T>
struct with_padding_top {};

template<typename T>
struct with_padding_bottom {};

template<typename T>
struct with_min_width {
	using value_type = std::optional<pixel_percent_auto>;

	value_type value(std::optional<int> parent_width) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return std::nullopt;
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_width));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};

template<typename T>
struct with_width {
	using value_type = pixel_percent_auto;

	value_type value(std::optional<int> parent_width) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return auto_();
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_width));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};

template<typename T>
struct with_max_width {
	using value_type = std::optional<pixel_percent_auto>;

	value_type value(std::optional<int> parent_width) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return std::nullopt;
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_width));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};

template<typename T>
struct with_min_height {
	using value_type = pixel_percent_auto;

	value_type value(std::optional<int> parent_height) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return auto_();
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_height));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};

template<typename T>
struct with_height {
	using value_type = pixel_percent_auto;

	value_type value(std::optional<int> parent_height) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return auto_();
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_height));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};

template<typename T>
struct with_max_height {
	using value_type = pixel_percent_auto;

	value_type value(std::optional<int> parent_height) const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return auto_();
		} else {
			if constexpr (std::is_invocable_v<T, std::optional<int>>) {
				return value_type(thing(parent_height));
			} else {
				return value_type(thing());
			}
		}
	}

	T thing;
};


template<typename T>
struct with_display {
	using value_type = display_type;

	value_type value() const {
		if constexpr (std::convertible_to<T, value_type>) {
			return value_type(thing);
		} else if constexpr (std::same_as<default_land, T>) {
			return display_type::block;
		} else {
			return value_type(thing());
		}
	}

	T thing;
};

//;-;
template<typename T, typename F>
struct test_with_thingy {
	test_with_thingy() = default;

	explicit test_with_thingy(T a) :
		m_value(std::move(a)) {}

	using value_type = T;


private:
	T m_value;
};

using wataaaaa = test_with_thingy<int, decltype([]() { return 1; })>;

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

static_assert(same_template<with_max_height<int>, with_max_height<double>>::value);

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

	auto width(pixel_percent_auto wat) {
		return combine_with(with_width<pixel_percent_auto>(wat));
	}

	auto min_width(pixel_percent_auto wat) {
		return combine_with(with_min_width<pixel_percent_auto>(wat));
	}

	auto max_width(pixel_percent_auto wat) {
		return combine_with(with_max_width<pixel_percent_auto>(wat));
	}

	auto height(pixel_percent_auto wat) {
		return combine_with(with_height<pixel_percent_auto>(wat));
	}


	display_type get_display() const noexcept {
		const auto hmm = []<typename me, typename F, typename...R>(const me& a, const F& f, const R&... r)->auto&& {
			if constexpr (is_specialization_v<F, with_display>) {
				return f;
			} else {
				if constexpr (sizeof...(R) == 0) {
					return with_display<default_land>(default_land());
				} else {
					return a(a, r...);
				}
			}
		};

		return std::apply([&](const auto&... properties) {
			return hmm(hmm, properties...);
		}, m_properties).value();
	}

	pixel_percent_auto get_width(std::optional<int> parent_width = std::nullopt) const noexcept {
		const auto hmm = []<typename me, typename F, typename...R>(const me& a, const F& f, const R&... r)->auto&& {
			if constexpr (is_specialization_v<F, with_width>) {
				return f;
			} else {
				if constexpr (sizeof...(R) == 0) {
					return with_width<default_land>(default_land());
				} else {
					return a(a, r...);
				}
			}
		};

		return std::apply([&](const auto&... properties) {
			return hmm(hmm, properties...);
		}, m_properties).value();
	}

	std::optional<pixel_percent_auto> get_min_width(std::optional<int> parent_width = std::nullopt) const noexcept {
		const auto hmm = []<typename me, typename F, typename...R>(const me& a, const F& f, const R&... r)->auto&& {
			if constexpr (is_specialization_v<F, with_min_width>) {
				return f;
			} else {
				if constexpr (sizeof...(R) == 0) {
					return with_min_width<default_land>(default_land());
				} else {
					return a(a, r...);
				}
			}
		};

		return std::apply([&](const auto&... properties) {
			return hmm(hmm, properties...);
		}, m_properties).value();
	}

	std::optional<pixel_percent_auto> get_max_width(std::optional<int> parent_width = std::nullopt) const noexcept {
		const auto hmm = []<typename me, typename F, typename...R>(const me& a, const F& f, const R&... r)->auto&& {
			if constexpr (is_specialization_v<F, with_max_width>) {
				return f;
			} else {
				if constexpr (sizeof...(R) == 0) {
					return with_max_width<default_land>(default_land());
				} else {
					return a(a, r...);
				}
			}
		};

		return std::apply([&](const auto&... properties) {
			return hmm(hmm, properties...);
		}, m_properties).value();
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
		} else {
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
		alignas(std::tuple<U...>) std::array<std::byte, sizeof(std::tuple<U...>)> thing;

		css_stuff<U...> to_thing() {
			return css_stuff<U...>(as_thing());
		}

		std::tuple<U...>& as_thing() {
			return *std::launder((std::tuple<U...>*)&thing);
		}

		template<typename... Ts, typename O>
		void steal_stuff(std::tuple<Ts...>& other, O& wat) {
			new(&std::get<O>(as_thing())) O(std::move(wat));
			((new(&std::get<Ts>(as_thing())) Ts(std::move(std::get<Ts>(other)))), ...);
		}
	};


	template<typename U, typename F, typename...R, typename...Current>
	static auto combined_type(typelist<U>, typelist<F, R...>, typelist<Current...>) {
		if constexpr (same_template<U, F>::value) {
			return css_stuff_fake<Current..., U, R...>();
		} else {
			if constexpr (sizeof...(R) == 0) {
				return css_stuff_fake<Current..., F, U>();
			} else {
				return combined_type(typelist<U>(), typelist<R...>(), typelist<Current..., F>());
			}
		}
	}


	friend struct combine_css_f;

	std::tuple<T...> m_properties;
};

template<typename...T>
css_stuff(std::tuple<T...>)->css_stuff<T...>;

struct combine_css_f {
	template<typename...T, typename ...U>
	static auto combine_with(css_stuff<T...> a, css_stuff<U...> b) {
		auto wat = [&]<typename me, typename...F, typename...R>(me& fn, F& first, R&... rest) {
			if constexpr (std::apply([]<typename... B_prop_types>(B_prop_types&...) {
				return (same_template<F, B_prop_types>::value || ...);
			}, b.m_properties)) {
				if constexpr (sizeof...(R) == 0) {
					return std::tuple();
				} else {
					return fn(fn, rest...);
				}
			} else {
				if constexpr (sizeof...(R) == 0) {
					return std::make_tuple(std::move(first));
				} else {
					auto next = fn(fn, rest...);
					return std::tuple_cat(std::move(next), std::make_tuple(std::move(first)));
				}
			}
		};
		const auto a_only_props = [&](auto&... props) {
			return wat(wat, props...);
		};
		return std::tuple_cat(std::apply(a_only_props, a.m_properties), std::move(b.m_properties));

	}

	template<typename ...U>
	static auto combine_with(css_stuff<> a, css_stuff<U...> b) {
		return std::move(b);
	}

	template<typename ...U>
	static auto combine_with(css_stuff<U...> a, css_stuff<> b) {
		return std::move(a);
	}
};

struct css_properties_stack {
	//colour:color
	//bg colour:color
	//font-size:px
	//margins:%,px,auto
	//padding:%,px
	//border:px
	//border-color:color
	//border style:solid,dotted
	//border radius:px,%
	//display: grid,inline,block;no inline-grid
	//(max/min)width:%,px,auto
	//(max/min)height:%,px,auto
	//grid row gap:px,%
	//grid column gap:px,%
	//grid template columns:vector<varient<px,%,auto,fr>>
	//grid template rows:vector<varient<px,%,auto,fr>>
	//grid row/column span:int>=1
	//grid column gap background:color
	//grid row gap background:color
	//cursor:stuff
	//place items:left,right,center
	//box-sizing
	//pointer events:none,auto
	//hover properties
	//active properties
};
