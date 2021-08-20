#pragma once
#include <iostream>

#include "tetris_stuff.h"
#include "ref_count_ptr.h"
#include <tuple>
#include "ai.h"

#include "eager_task.h"
#include <boost/asio.hpp>

#include "task_completion_handler.h"
#include "good_ai.h"


struct ai_settings {
	std::chrono::milliseconds piece_delay = 1000ms;
};

struct tetris_ai_player :ref_counted {

	explicit tetris_ai_player(boost::asio::any_io_executor e, ai_settings settings):
		m_executor(std::move(e)),
		m_random_engine(([]() {
			std::random_device r;
			return r();
		})()),
		m_settings(std::move(settings)) {

		while (m_game.preview_pieces.size() <= 7) {
			m_game.generate_new_pieces(m_random_engine);
		}
		//m_game.try_spawn_new_piece();
		m_game.current_piece = tetris_piece::no_piece;
	}

	~tetris_ai_player() {
		stop_doing_stuff();
		//keep vars alive while it updates vars, shuold be like just copying bytes here
		while (m_ai_is_updating);//wat
	}

	tetris_game_update get_update() {
		std::lock_guard lock(m_mut);//????
		auto g = m_game;
		g.try_spawn_new_piece();
		return {
			std::move(g),
			std::exchange(m_garbage_sent_since_last_update, 0),
			m_garbage_calculator.current_combo,
			std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0),
			m_dead
		};
	}

	void receive_update(std::chrono::milliseconds time_since_last_update, int garbage_recieved) {
		m_garbage_recieving.push_back((int8_t)garbage_recieved);
		m_time_till_next_piece -= time_since_last_update;
		if (m_time_till_next_piece <= 0ms) {
			m_time_till_next_piece += m_settings.piece_delay;
			if (m_has_next) {
				m_game = std::move(m_game_next);
				m_garbage_calculator = m_garbage_calculator_next;
				m_garbage_sent_since_last_update += std::exchange(m_garbage_sent_since_last_update_next, 0);
				m_dead = m_dead_next;
				if (!m_garbage_recieving.empty()) {
					sbo_vector<int, 20> stuff;
					stuff.reserve(std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0));
					const std::uniform_int_distribution<> dist(0, 9);
					for (auto num_garb : m_garbage_recieving) {
						const auto hole_idx = dist(m_random_engine);
						stuff.resize(stuff.size() + num_garb, hole_idx);
					}
					m_garbage_recieving.clear();
					m_game.spawn_garbage(std::span(stuff.data(), std::min(32, (int)stuff.size())));
				}
				while (m_game.preview_pieces.size() <= 7) {
					m_game.generate_new_pieces(m_random_engine);
				}
				m_has_next = false;
				if (m_lines_cleared) {
					m_time_till_next_piece += 000ms;
				}
				if (!m_ai_is_running) {
					m_ai_is_running = true;
					boost::asio::post(m_executor, [this, ai_should_stop = m_ai_should_stop, gm = m_game_number_thingy.load(),ai = m_ai]() {
						auto thing = (*ai)(m_game, m_garbage_calculator, std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0));
						std::cout << "watland" << std::endl;
						if (!ai_should_stop->load() && gm == m_game_number_thingy) {
							m_ai_is_updating = true;
							m_game_next = std::move(thing.game);
							m_garbage_calculator_next = thing.garb_state;
							m_garbage_sent_since_last_update_next += thing.garbage_sent;
							m_dead_next = thing.dead;
							m_ai_is_running = false;
							m_has_next = true;
							m_lines_cleared = thing.lines_cleared;
							m_game_number_next = gm;
							m_ai_is_updating = false;
						}
					});
				}
			}
		}
	}

	void start_doing_stuff() {
		m_ai_is_running = true;
		boost::asio::post(m_executor, [this, ai_should_stop = m_ai_should_stop, gm = m_game_number_thingy.load(),ai = m_ai]() {
			auto thing = (*ai)(m_game, m_garbage_calculator, std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0));
			std::cout << "watland" << std::endl;
			if (!*ai_should_stop && gm == m_game_number_thingy) {
				m_ai_is_updating = true;
				m_game_next = std::move(thing.game);
				m_garbage_calculator_next = thing.garb_state;
				m_garbage_sent_since_last_update_next += thing.garbage_sent;
				m_dead_next = thing.dead;
				m_ai_is_running = false;
				m_has_next = true;
				m_lines_cleared = thing.lines_cleared;
				m_game_number_next = gm;
				m_ai_is_updating = false;
			}
			int iu = 0;
		});
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void stop_doing_stuff() {
		*m_ai_should_stop = true;
		m_ai_should_stop = std::make_shared<std::atomic<bool>>(false);
		++m_game_number_thingy;
	}

	void reset() {
		m_has_next = false;
		m_garbage_calculator = garbage_calculator();
		m_game = tetris_game();
		while (m_game.preview_pieces.size() <= 7) {
			m_game.generate_new_pieces(m_random_engine);
		}
		//m_game.try_spawn_new_piece();
		m_game.current_piece = tetris_piece::no_piece;
		m_dead = false;
		m_garbage_sent_since_last_update = 0;
		m_garbage_recieving = {};
		m_ai_should_stop = std::make_shared<std::atomic<bool>>(false);
	}

	const ai_settings& settings() const noexcept {
		return m_settings;
	}

	const auto& get_executor() const noexcept {
		return m_executor;
	}

	auto& get_executor() noexcept {
		return m_executor;
	}


private:


	boost::asio::any_io_executor m_executor;
	std::mt19937 m_random_engine;
	ai_settings m_settings;

	std::mutex m_mut;
	tetris_game m_game;
	int m_garbage_sent_since_last_update = 0;
	garbage_calculator m_garbage_calculator;
	bool m_dead = false;
	sbo_vector<int8_t, 20> m_garbage_recieving = {};

	std::shared_ptr<flatstacking_ai> m_ai = std::make_shared<flatstacking_ai>();
	std::chrono::milliseconds m_time_till_next_piece = m_settings.piece_delay;
	std::atomic<bool> m_ai_is_running = false;

	std::shared_ptr<std::atomic<bool>> m_ai_should_stop = std::make_shared<std::atomic<bool>>(false);

	std::atomic<int> m_game_number_thingy = 0;
	std::atomic<bool> m_ai_is_updating = false;

	std::atomic<bool> m_has_next = false;
	tetris_game m_game_next;
	int m_garbage_sent_since_last_update_next = 0;
	garbage_calculator m_garbage_calculator_next;
	bool m_dead_next = false;
	int m_lines_cleared = 0;
	int m_game_number_next = 0;
};
