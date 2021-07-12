#pragma once
#include "good_ai.h"

struct tetris_game_update2 {//for this file only
	bool dead = false;
	int garbage_sent = 0;
};

template<typename T>
struct local_ai {

	explicit local_ai(T ai, std::mt19937& engine) :
		m_ai(std::forward<T>(ai)), m_engine(engine) {
		while (m_game.preview_pieces.size() <= 6) {
			m_game.generate_new_pieces(engine);
		}
	}

	tetris_game_update2 get_update() {
		return tetris_game_update2{
			m_dead,
			std::exchange(m_garbage_sent_since_last_update, 0),
		};
	}

	void receive_update(int garbage_received) {
		if (garbage_received) {
			m_garbage_recieving.push_back(garbage_received);
		}

		auto [next_game, lines_cleared, garbage_sent, next_garb_state, dead, rotate_stuff] = m_ai(m_game, m_garbage_sent_since_last_update, 0);
		m_game = std::move(next_game);
		m_garbage_calculator = next_garb_state;
		m_dead = dead;
		m_rotate_stuff = rotate_stuff;

		while (!m_garbage_recieving.empty() && garbage_sent) {
			const auto n = std::min((int)garbage_sent, (int)m_garbage_recieving.back());
			garbage_sent -= n;
			m_garbage_recieving.back() -= n;
			if (m_garbage_recieving.back() == 0) {
				m_garbage_recieving.pop_back();
			}
		}

		if (!m_garbage_recieving.empty()) {
			sbo_vector<int, 32> garbage_holes;

			for (const auto garbage_amount : m_garbage_recieving) {
				std::uniform_int_distribution<int> dist(0, 9);
				const auto hole_idx = dist(m_engine);
				garbage_holes.resize(garbage_holes.size() + garbage_amount, hole_idx);

			}

			m_game.spawn_garbage(std::span(garbage_holes.begin(), std::min(32, (int)garbage_holes.size())));
		}


	}


	T m_ai;


	tetris_game m_game = {};
	int m_garbage_sent_since_last_update = 0;
	garbage_calculator m_garbage_calculator;
	bool m_dead = false;
	sbo_vector<int8_t, 20> m_garbage_recieving = {};
	rotate_info m_rotate_stuff;

	std::mt19937& m_engine;

};

template<typename T, typename U>
int bot_fight(const T& ai1, const U& ai2) {

	thread_local std::mt19937 engine = []() {
		std::random_device r;
		std::seed_seq seq{r(), r(), r(), r()};
		return std::mt19937(seq);
	}();

	local_ai<const T&> player1(ai1, engine);
	local_ai<const U&> player2(ai2, engine);

	std::array<int, 3> player1_garbage_queue;
	std::array<int, 3> player2_garbage_queue;


	for (int _ = 0; _ < 500; ++_) {
		auto [dead1,garbage_sent1] = player1.get_update();
		auto [dead2,garbage_sent2] = player2.get_update();

		if (dead1) {
			return 1;
		}
		if (dead2) {
			return 0;
		}

		//garbage cancel
		for (auto& garb_in_queue : player1_garbage_queue) {
			const auto amount_to_sub = std::min(garb_in_queue, garbage_sent1);
			garb_in_queue -= amount_to_sub;
			garbage_sent1 -= amount_to_sub;
		}

		for (auto& garb_in_queue : player2_garbage_queue) {
			const auto amount_to_sub = std::min(garb_in_queue, garbage_sent2);
			garb_in_queue -= amount_to_sub;
			garbage_sent2 -= amount_to_sub;
		}

		//garbage cancel both thingys sent this update

		const auto amount_to_cancel_from_both = std::min(garbage_sent1, garbage_sent2);
		garbage_sent1 -= amount_to_cancel_from_both;
		garbage_sent2 -= amount_to_cancel_from_both;

		//send garbage
		const auto garb_to_send_to_player1 = player1_garbage_queue.front();
		const auto garb_to_send_to_player2 = player2_garbage_queue.front();
		std::shift_left(player1_garbage_queue.begin(), player1_garbage_queue.end(), 1);
		std::shift_left(player2_garbage_queue.begin(), player2_garbage_queue.end(), 1);
		player1_garbage_queue.back() = garbage_sent2;
		player2_garbage_queue.back() = garbage_sent1;

		player1.receive_update(garb_to_send_to_player1);
		player2.receive_update(garb_to_send_to_player2);
	}

	auto [dead1, garbage_sent1] = player1.get_update();
	auto [dead2, garbage_sent2] = player2.get_update();

	if (dead1) {
		return 1;
	}
	if (dead2) {
		return 0;
	}

	return 2;
}

flatstacking_ai make_ai(std::mt19937& engine) {
	const std::uniform_int_distribution<int> dist0to50(0, 50);
	const std::uniform_int_distribution<int> dist0to25(0, 25);
	const std::uniform_int_distribution<int> dist0to10(0, 10);
	
	const std::uniform_int_distribution<int> distM50to50(-50, 50);
	const std::uniform_int_distribution<int> distM25to25(-25, 25);
	const std::uniform_int_distribution<int> distM10to10(-10, 10);
	
	const std::uniform_int_distribution<int> distM100to0(-100, 0);
	const std::uniform_int_distribution<int> distM50to0(-50, 0);
	const std::uniform_int_distribution<int> distM25to0(-25, 0);
	const std::uniform_int_distribution<int> distM10to0(-10, 0);

	return flatstacking_ai{
		.max_height_diff = dist0to25(engine),
		.avg_height_between_constant = dist0to50(engine),
		.depth_diff_below = dist0to10(engine),
		.depth_diff_above = distM10to0(engine),
		.height_std = distM10to0(engine),
		.garbage_sent_squared = dist0to25(engine),
		.garbage_sent = dist0to50(engine),
		.useless_line_clear = distM50to0(engine),
		.inefficient_line_clear = distM50to0(engine),
		.top_quarter = distM100to0(engine),
		.top_half = distM50to0(engine),
		.well_idx = {
			distM25to25(engine),
			distM25to0(engine),
			distM25to25(engine),
			distM25to25(engine),
			distM25to25(engine),
			distM25to25(engine),
			distM25to25(engine),
			distM25to25(engine),
			distM25to0(engine),
			distM25to25(engine)},
		.wasted_line_clear = distM25to0(engine),
		.wasted_line_clear_squared = distM10to0(engine),
		.well_dep = dist0to10(engine),
		.is_b2b = dist0to50(engine),
		.in_optimal_height = dist0to50(engine),
		.height_dist_from_optimal = distM25to0(engine),
		.efficent_line_clear = dist0to50(engine),
		.bumpiness_ = distM10to0(engine),
		.bumpiness_squared = distM10to0(engine),
		.covered = distM25to0(engine),
		.covered_squared = distM10to0(engine),
		.clear_efficency = dist0to10(engine),
	};
}
