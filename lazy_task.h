#pragma once
#include <variant>
#include <optional>
#include <coroutine>
#include "better_conditional.h"
#include <type_traits>
#include "resume_coroutine.h"
#include <stdexcept>

namespace cerwy {

template<typename T>
struct [[nodiscard]] lazy_task {
	constexpr lazy_task() = default;
	lazy_task(const lazy_task&) = delete;
	lazy_task& operator=(const lazy_task&) = delete;

	constexpr lazy_task(lazy_task&&) = default;
	constexpr lazy_task& operator=(lazy_task&&) = default;

	~lazy_task() {
		if (m_coro) {
			m_coro.destroy();
		}
	}

	struct promise_type {
		// ReSharper disable CppMemberFunctionMayBeStatic
		lazy_task get_return_object() noexcept {
			return lazy_task(std::coroutine_handle<promise_type>::from_promise(*this));
		}

		void unhandled_exception() noexcept {
			result = std::current_exception();
		}

		void return_value(T& thing) {
			result = std::move(thing);
		}

		void return_value(T&& thing) {
			result = std::move(thing);
		}

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		auto final_suspend() const noexcept {
			return resume_coroutine(waiter);
		}

		// ReSharper restore CppMemberFunctionMayBeStatic
		std::coroutine_handle<> waiter = nullptr;
		std::variant<T, std::exception_ptr> result;
	};

	bool await_ready() const noexcept {
		return !m_coro || m_coro.done();
	}

	std::coroutine_handle<promise_type> await_suspend(std::coroutine_handle<> h) const {
		m_coro.promise().waiter = h;
		return m_coro;
	}

	T&& await_resume() const {
		if (!m_coro) {
			throw std::runtime_error(";-;");
		}
		if (std::holds_alternative<T>(m_coro.promise().result)) {
			return std::move(std::get<T>(m_coro.promise().result));
		} else {
			std::rethrow_exception(std::move(std::get<std::exception_ptr>(m_coro.promise().result)));
		}
	}

private:
	std::coroutine_handle<promise_type> m_coro = nullptr;

	explicit lazy_task(std::coroutine_handle<promise_type> thing):
		m_coro(thing) {}

	friend promise_type;

};

template<>
struct lazy_task<void> {
	constexpr lazy_task() = default;
	lazy_task(const lazy_task&) = delete;
	lazy_task& operator=(const lazy_task&) = delete;

	constexpr lazy_task(lazy_task&&) = default;
	constexpr lazy_task& operator=(lazy_task&&) = default;

	~lazy_task() {
		if (m_coro) {
			m_coro.destroy();
		}
	}

	struct promise_type {
		// ReSharper disable CppMemberFunctionMayBeStatic
		lazy_task get_return_object() noexcept {
			return lazy_task(std::coroutine_handle<promise_type>::from_promise(*this));
		}

		void unhandled_exception() noexcept {
			error = std::current_exception();
		}

		void return_void() const noexcept { }

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		auto final_suspend() const noexcept {
			return resume_coroutine(waiter);
		}

		// ReSharper restore CppMemberFunctionMayBeStatic
		std::coroutine_handle<> waiter = nullptr;
		std::optional<std::exception_ptr> error;
	};

	bool await_ready() const noexcept {
		return !m_coro || m_coro.done();
	}

	std::coroutine_handle<promise_type> await_suspend(std::coroutine_handle<> h) const {
		m_coro.promise().waiter = h;
		return m_coro;
	}

	void await_resume() const {
		if (!m_coro) {
			return;
		}
		if (m_coro.promise().error) {
			std::rethrow_exception(std::move(m_coro.promise().error.value()));
		}
	}

private:
	std::coroutine_handle<promise_type> m_coro = nullptr;

	explicit lazy_task(std::coroutine_handle<promise_type> thing) :
		m_coro(thing) {}

	friend promise_type;
};

template<typename T>
struct [[nodiscard]] lazy_task<T&> {
	lazy_task() = default;
	lazy_task(const lazy_task&) = delete;
	lazy_task& operator=(const lazy_task&) = delete;

	lazy_task(lazy_task&&) = default;
	lazy_task& operator=(lazy_task&&) = default;

	~lazy_task() {
		if (m_coro) {
			m_coro.destroy();
		}
	}

	struct promise_type {
		// ReSharper disable CppMemberFunctionMayBeStatic
		lazy_task get_return_object() noexcept {
			return lazy_task(std::coroutine_handle<promise_type>::from_promise(*this));
		}

		void unhandled_exception() noexcept {
			result = std::current_exception();
		}

		void return_value(T& thing) {
			result.template emplace<T*>(&thing);
		}

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		auto final_suspend() const noexcept {
			return resume_coroutine(waiter);
		}

		// ReSharper restore CppMemberFunctionMayBeStatic
		std::coroutine_handle<> waiter = nullptr;
		std::variant<T*, std::exception_ptr> result;
	};

	bool await_ready() const noexcept {
		return !m_coro || m_coro.done();
	}

	std::coroutine_handle<promise_type> await_suspend(std::coroutine_handle<> h) const {
		m_coro.promise().waiter = h;
		return m_coro;
	}

	T& await_resume() const {
		if (!m_coro) {
			throw std::runtime_error(";-;");
		}
		if (std::holds_alternative<T>(m_coro.promise().result)) {
			return *std::get<T*>(m_coro.promise().result);
		} else {
			std::rethrow_exception(std::move(std::get<std::exception_ptr>(m_coro.promise().result)));
		}
	}

private:
	std::coroutine_handle<promise_type> m_coro = nullptr;

	explicit lazy_task(std::coroutine_handle<promise_type> thing) :
		m_coro(thing) {}

	friend promise_type;

};

inline lazy_task<void> finished_task() noexcept {
	return lazy_task<void>();
}

}
