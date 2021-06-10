#pragma once
#include "tetris_stuff.h"
#include "ref_count_ptr.h"
#include <tuple>
#include "ai.h"

#include "eager_task.h"
#include <boost/asio.hpp>

#include "task_completion_handler.h"


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

		while (m_game.preview_pieces.size() <= 6) {
			m_game.generate_new_pieces(m_random_engine);
		}
		//m_game.try_spawn_new_piece();
		m_game.current_piece = tetris_piece::no_piece;
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
				m_garbage_sent_since_last_update += m_garbage_sent_since_last_update_next;
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
				while (m_game.preview_pieces.size() <= 6) {
					m_game.generate_new_pieces(m_random_engine);
				}
				m_has_next = false;
				if (!m_ai_is_running) {
					m_ai_is_running = true;
					boost::asio::post(m_executor, [this]() {
						auto thing = m_ai(m_game, m_garbage_calculator, std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0));
						std::cout << "watland" << std::endl;
						m_game_next = std::move(thing.game);
						m_garbage_calculator_next = thing.garb_state;
						m_garbage_sent_since_last_update_next += thing.garbage_sent;
						m_dead_next = thing.dead;
						m_ai_is_running = false;
						m_has_next = true;
					});
				}
			}
		}
	}

	void start_doing_stuff() {
		m_ai_is_running = true;
		boost::asio::post(m_executor, [this]() {
			auto thing = m_ai(m_game, m_garbage_calculator, std::accumulate(m_garbage_recieving.begin(), m_garbage_recieving.end(), 0));
			std::cout << "watland" << std::endl;
			m_game_next = std::move(thing.game);
			m_garbage_calculator_next = thing.garb_state;
			m_garbage_sent_since_last_update_next += thing.garbage_sent;
			m_dead_next = thing.dead;
			m_ai_is_running = false;
			m_has_next = true;
		});
	}

	void stop_doing_stuff() {
		//wat
	}

private:


	boost::asio::any_io_executor m_executor;
	std::mt19937 m_random_engine;
	ai_settings m_settings;

	std::atomic<boost::asio::steady_timer*> m_timer = nullptr;

	std::mutex m_mut;
	tetris_game m_game;
	int m_garbage_sent_since_last_update = 0;
	garbage_calculator m_garbage_calculator;
	bool m_dead = false;
	sbo_vector<int8_t, 20> m_garbage_recieving = {};

	flatstacking_ai m_ai;
	std::chrono::milliseconds m_time_till_next_piece = m_settings.piece_delay;
	std::atomic<bool> m_ai_is_running = false;

	std::vector<int> m_do_not_place_cols;


	std::atomic<bool> m_has_next = false;
	tetris_game m_game_next;
	int m_garbage_sent_since_last_update_next = 0;
	garbage_calculator m_garbage_calculator_next;
	bool m_dead_next = false;

};
