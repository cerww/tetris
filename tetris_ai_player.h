#pragma once
#include "tetris_stuff.h"
#include "ref_count_ptr.h"
#include <tuple>

#include "eager_task.h"
#include <boost/asio.hpp>

#include "task_completion_handler.h"

constexpr int get_col_height(std::span<const tetris_block> col) {
	int ret = 20;
	while (ret >= 1 && col[ret] != tetris_block::empty) {
		--ret;
	}
	return ret;
}

constexpr bool is_garbage_row(auto&& row) {
	return ranges::contains(row, tetris_block::garbage);
}


struct place_next_piece_entry {
	bool swap_piece = false;
	rotate_info last_rotate_info;
	int orientation = 0;
	int x = 0;
	int y = 0;
};

struct flatstacking_ai {
	


	std::vector<place_next_piece_entry> next_pieces(tetris_game& game,int garbage_receiving/*ignore this var for this fn*/) {
		//hmmmm


		const auto row = ranges::views::iota(0, game.board.minos[0].size()) | ranges::views::transform([&](int y) {
			return ranges::views::iota(0, game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
				return game.board.minos[x][y];
				});
			});

		const std::vector<int> heights = game.board.minos | std::views::transform([](auto& a) {return get_col_height(a); }) | ranges::to<std::vector>();

		const auto lowest_height_idx = std::ranges::min_element(heights) - heights.begin();




	}

};

struct ai_settings {
	std::chrono::milliseconds piece_delay = 1000ms;
};

struct tetris_ai_player :ref_counted {

	explicit tetris_ai_player(boost::asio::any_io_executor e,ai_settings settings):
		m_executor(std::move(e)),
		m_random_engine(([]() {
			std::random_device r;
			return r();
		})()),
		m_settings(std::move(settings)) {
		
		while (m_game.preview_pieces.size() <= 6) {
			m_game.generate_new_pieces(m_random_engine);
		}
		m_game.try_spawn_new_piece();
	}

	tetris_game_update get_update() {
		
	}

	void receive_update(std::chrono::milliseconds time_since_last_update, int garbage_recieved) {
		m_garbage_recieving.push_back((int8_t)garbage_recieved);
		m_time_till_next_piece -= time_since_last_update;
		if(m_time_till_next_piece <=0ms) {
			m_time_till_next_piece += m_settings.piece_delay;

			
		}
	}

	void start_doing_stuff() {
		//wat
	}

	void stop_doing_stuff() {
		
	}

private:
	


	
	boost::asio::any_io_executor m_executor;
	std::mt19937 m_random_engine;
	ai_settings m_settings;

	std::atomic<boost::asio::steady_timer*> m_timer = nullptr;

	std::mutex m_mut;
	tetris_game m_game;
	int m_garbage_sent_since_last_update = 0;
	int m_current_combo = -1;
	sbo_vector<int8_t,20> m_garbage_recieving = {};
	bool m_dead = false;

	garbage_calculator m_garbage_calculator;

	std::chrono::milliseconds m_time_till_next_piece = m_settings.piece_delay;
	std::vector<place_next_piece_entry> m_thing_queue;
	std::atomic<bool> m_has_piece_in_queue = false;

	std::vector<int> m_do_not_place_cols;
	

	

	
};
