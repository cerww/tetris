#pragma once
#include <coroutine>

struct resume_coroutine {
	resume_coroutine() = default;
	resume_coroutine(std::coroutine_handle<> t_h):h(t_h){}

	static bool await_ready() noexcept{
		return false;
	}

	auto await_suspend(auto)const noexcept {
		return h;
	}

	static void await_resume()noexcept{}
	
	std::coroutine_handle<> h = nullptr;
};
