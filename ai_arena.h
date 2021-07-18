#pragma once
#include "good_ai.h"
#include <fstream>
#include <boost/asio.hpp>


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

		auto [next_game, lines_cleared, garbage_sent, next_garb_state, dead, rotate_stuff] = m_ai(m_game, m_garbage_calculator, 0);
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

		while (m_game.preview_pieces.size() <= 6) {
			m_game.generate_new_pieces(m_engine);
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

struct bot_fight_result {
	int p1_wins = 0;
	int p2_wins = 0;
	int draws = 0;
};

template<typename T, typename U>
int bot_fight1(const T& ai1, const U& ai2) {
	thread_local std::mt19937 engine = []() {
		std::random_device r;
		std::seed_seq seq{r(), r(), r(), r()};
		return std::mt19937(seq);
	}();

	local_ai<const T&> player1(ai1, engine);
	local_ai<const U&> player2(ai2, engine);

	std::array<int, 2> player1_garbage_queue = {};
	std::array<int, 2> player2_garbage_queue = {};
	for (int _ = 0; _ < 300; ++_) {
		auto [dead1, garbage_sent1] = player1.get_update();
		auto [dead2, garbage_sent2] = player2.get_update();

		if (dead1 && dead2) {
			return 2;
		}
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
	return 2;
}

//0 => player 0 wins, 1 => player 1 wins, 2 => draw
template<typename T, typename U>
bot_fight_result bot_fight(const T& ai1, const U& ai2) {
	bot_fight_result ret = {};

	for (int i = 0; i < 5; ++i) {
		const auto r = bot_fight1(ai1, ai2);
		if (r == 0) {
			++ret.p1_wins;
		} else if (r == 1) {
			++ret.p2_wins;
		} else {
			++ret.draws;
		}
	}

	return ret;
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
		.height_std = (double)distM10to0(engine),
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
			distM25to25(engine)
		},
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

std::vector<flatstacking_ai> modify_ais(const flatstacking_ai& ai, int n, std::mt19937& engine) {
	std::vector<flatstacking_ai> ret;
	ret.reserve(n);

	const std::uniform_int_distribution<int> small_step(-5, 5);
	const std::uniform_int_distribution<int> big_step(-15, 15);

	const auto next_number = [&]() {
		return engine() & 1 ? small_step(engine) : big_step(engine);
	};

	for (int i = 0; i < n; ++i) {
		ret.push_back(flatstacking_ai{
			.max_height_diff = next_number() + ai.max_height_diff,
			.avg_height_between_constant = next_number() + ai.avg_height_between_constant,
			.depth_diff_below = next_number() + ai.depth_diff_below,
			.depth_diff_above = next_number() + ai.depth_diff_above,
			.height_std = next_number() + ai.height_std,
			.garbage_sent_squared = next_number() + ai.garbage_sent_squared,
			.garbage_sent = next_number() + ai.garbage_sent,
			.useless_line_clear = next_number() + ai.useless_line_clear,
			.inefficient_line_clear = next_number() + ai.inefficient_line_clear,
			.top_quarter = next_number() + ai.top_quarter,
			.top_half = next_number() + ai.top_half,
			.well_idx = {
				next_number() + ai.well_idx[0],
				next_number() + ai.well_idx[1],
				next_number() + ai.well_idx[2],
				next_number() + ai.well_idx[3],
				next_number() + ai.well_idx[4],
				next_number() + ai.well_idx[5],
				next_number() + ai.well_idx[6],
				next_number() + ai.well_idx[7],
				next_number() + ai.well_idx[8],
				next_number() + ai.well_idx[9]
			},
			.wasted_line_clear = next_number() + ai.wasted_line_clear,
			.wasted_line_clear_squared = next_number() + ai.wasted_line_clear_squared,
			.well_dep = next_number() + ai.well_dep,
			.is_b2b = next_number() + ai.is_b2b,
			.in_optimal_height = next_number() + ai.in_optimal_height,
			.height_dist_from_optimal = next_number() + ai.height_dist_from_optimal,
			.efficent_line_clear = next_number() + ai.efficent_line_clear,
			.bumpiness_ = next_number() + ai.bumpiness_,
			.bumpiness_squared = next_number() + ai.bumpiness_squared,
			.covered = next_number() + ai.covered,
			.covered_squared = next_number() + ai.covered_squared,
			.clear_efficency = next_number() + ai.clear_efficency,
		});
	}

	return ret;
}

void do_ai_thingy(std::optional<std::string_view> file_name = std::nullopt) {

	struct ai_wins_stat {
		std::atomic<int> wins = 0;
		std::atomic<int> losses = 0;
		std::atomic<int> draws = 0;
		//games played = wins + losses + draws
	};

	struct ai_wins_stat_no_atomic {
		ai_wins_stat_no_atomic() = default;

		ai_wins_stat_no_atomic(const ai_wins_stat& o):
			wins(o.wins), losses(o.losses), draws(o.draws) { }

		int wins = 0;
		int losses = 0;
		int draws = 0;
		//games played = wins + losses + draws
	};

	std::random_device r;
	const auto seq = std::seed_seq{r(), r(), r(), r(), r()};
	std::mt19937 engine(seq);

	std::vector<flatstacking_ai> ais;

	if (file_name.has_value()) {
		std::ifstream f(std::string(file_name.value()), std::ios::in);
		std::string file_contents;
		f.seekg(std::ios::end);
		file_contents.resize(f.tellg());
		f.seekg(std::ios::beg);
		f.read(file_contents.data(), file_contents.size());

		ais = nlohmann::json::parse(file_contents).get<std::vector<flatstacking_ai>>();
	} else {
		for (int i = 0; i < 100 - 1; ++i) {
			ais.push_back(make_ai(engine));
		}
		ais.emplace_back();
	}
	constexpr int threads_count = 12;

	while (true) {

		{
			std::ofstream o1("wat", std::ios::trunc);
			o1 << nlohmann::json(ais).dump(4);
			o1.close();

			std::ofstream o2("wat2", std::ios::trunc);
			o2 << nlohmann::json(ais).dump(4);
			o2.close();
		}

		std::unique_ptr<ai_wins_stat[]> ai_win_stats = std::make_unique<ai_wins_stat[]>(ais.size());
		std::cout << "start" << std::endl;
		{
			boost::asio::io_context executor(threads_count);

			std::vector<std::jthread> threads;

			for (int i = 0; i < 700; ++i) {
				const auto idx1 = engine() % ais.size();
				const auto idx2 = engine() % ais.size();
				if (idx1 == idx2) {
					--i;
					continue;
				}
				boost::asio::post(executor, [&, i1 = idx1, i2 = idx2]() {
					const auto result = bot_fight(ais[i1], ais[i2]);
					fmt::print("{:>3} vs {:<3}: {}-{}-{}\n",i1,i2, result.p1_wins,result.draws,result.p2_wins);
					ai_win_stats[i1].wins += result.p1_wins;
					ai_win_stats[i1].losses += result.p2_wins;
					ai_win_stats[i2].wins += result.p2_wins;
					ai_win_stats[i2].losses += result.p1_wins;
					ai_win_stats[i1].draws += result.draws;
					ai_win_stats[i2].draws += result.draws;
				});
			}

			for (int i = 0; i < ais.size(); ++i) {
				boost::asio::post(executor, [&,i_=i]() {
					flatstacking_ai base_ai;					
					const auto result = bot_fight(ais[i_], base_ai);
					fmt::print("{:>3} vs {:<3}: {}-{}-{}\n", i_, "I", result.p1_wins, result.draws, result.p2_wins);
					ai_win_stats[i_].wins += result.p1_wins;
					ai_win_stats[i_].losses += result.p2_wins;
					ai_win_stats[i_].draws += result.draws;
				});
			}

			for (int i = 0; i < threads_count - 1; ++i) {
				threads.emplace_back([i = i, &executor]() {
					executor.run();
				});
			}
			executor.run();
		}

		std::span<ai_wins_stat> ai_wins_temp = std::span(ai_win_stats.get(), ais.size());
		auto ai_wins = ai_wins_temp | ranges::views::transform([](const auto& a) {return ai_wins_stat_no_atomic(a); }) | ranges::to<std::vector>();

		for (auto&& [idx,stats] : ranges::views::enumerate(ai_wins)) {
			fmt::print("{:4<}: {}\n", idx, (stats.wins + 0.5 * stats.draws) / (stats.losses + stats.wins + stats.draws));
		}

		const auto ais_and_wins = ranges::views::zip(ais, ai_wins);

		const auto tenth_percentile = (int)(ais.size() * 0.1);
		const auto twentyth_percentile = (int)(ais.size() * 0.2);
		std::ranges::partial_sort(ais_and_wins, ais_and_wins.begin() + tenth_percentile, std::greater(), [](const auto& a)->double {
			const auto& [ai, stats] = a;
			return (stats.wins + 0.5 * stats.draws) / (stats.losses + stats.wins + stats.draws);
		});

		ranges::shuffle(ais_and_wins.begin() + tenth_percentile, ais_and_wins.end(), engine);

		ais.erase(ais.begin() + twentyth_percentile, ais.end());

		std::vector<flatstacking_ai> next_ais;

		for (int i = 0; i < tenth_percentile; ++i) {
			ranges::push_back(next_ais, modify_ais(ais[i], 6, engine));
			next_ais.push_back(std::move(ais[i]));
		}

		for (int i = tenth_percentile; i < twentyth_percentile; ++i) {
			ranges::push_back(next_ais, modify_ais(ais[i], 2, engine));
			next_ais.push_back(std::move(ais[i]));
		}

		ais = std::move(next_ais);

	}
}
