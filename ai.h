#pragma once
#include <set>
#include <unordered_set>
#include <tuple>

#include "tetris_stuff.h"
#include <compare>
#include <ranges>


constexpr int get_col_height(std::span<const tetris_block> col) {
	int ret = 20;
	while (ret >= 1 && col[ret] != tetris_block::empty) {
		--ret;
	}
	return ret;
}

constexpr auto is_garbage_row = [](auto&& row) {
	return ranges::contains(row, tetris_block::garbage);
};

constexpr int garbage_height(const tetris_game& game) {
	const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
		return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
			return game.board.minos[x][y];
		});
	});
	return (int)(std::ranges::find_if(rows,is_garbage_row) - rows.begin());
}


struct place_next_piece_entry {
	bool swap_piece = false;
	rotate_info last_rotate_info;
	int orientation = 0;
	int x = 0;
	int y = 0;
};

struct flatstacking_ai {

	struct data_i_care_about {
		bool is_alive = true;
		int num_holes = 0;
		int height = 0;
		int garbage_height = 0;
		int varience = 0;
		
		auto operator<=>(const data_i_care_about& other) const noexcept = default;
		
	};

	std::vector<place_next_piece_entry> next_pieces(tetris_game& game, int garbage_receiving/*ignore this var for this fn*/) {
		//hmmmm


		const auto rows = ranges::views::iota(0, (int)game.board.minos[0].size()) | ranges::views::transform([&](int y) {
			return ranges::views::iota(0, (int)game.board.minos.size()) | ranges::views::transform([&, y_ = y](int x) {
				return game.board.minos[x][y];
			});
		});

		const std::vector<int> heights = game.board.minos | std::views::transform([](auto& a) { return get_col_height(a); }) | ranges::to<std::vector>();

		const auto lowest_height_idx = std::ranges::min_element(heights) - heights.begin();


	}

	data_i_care_about best_of_metric(tetris_game game, int ply_depth, std::array<bool,10> cols_to_avoid,std::span<const int> heights) {
		if(game.try_spawn_new_piece()) {
			return { false,0,0,0,0 };
		}


		data_i_care_about ret;
		
		if(game.held_piece != game.current_piece) {
			
		}
		
		if (!ply_depth) {
			
		}

	}

};



