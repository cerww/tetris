#pragma once
#include <array>
#include <optional>
#include <tuple>
#include "tetris_stuff.h"

struct match_pattern {
	int width = 0;
	int height = 0;
	std::array<std::array<int8_t, 5>, 5> pattern;//0 empty, 1 there,2 doesn't matter
	std::tuple<int, int, int> outcome;

	std::optional<std::tuple<int, int, int>> match(const tetris_board& board, int start_x, int end_x, int start_y, int end_y) const noexcept {
		for (int x = start_x; x < end_x - width; ++x) {
			for (int y = start_y; y < end_y - height; ++y) {
				for (int j = 0; j < height; ++j) {
					for (int i = 0; i < width; ++i) {
						//
						if ((pattern[j][i] == 0 && board.minos[x + i][y + j] != tetris_block::empty) ||
							(pattern[j][i] == 1 && board.minos[x + i][y + j] == tetris_block::empty)) {
							goto break_inner;
						}
					}
				}
				goto wat;
			break_inner:;
				continue;
			wat:;
				const auto [x_s, y_s, o] = outcome;
				return std::tuple(x_s + x, y_s + y, o);
			}
		}

		return std::nullopt;
	}

};

constexpr match_pattern create_match_pattern(int w, int h, std::array<std::array<int8_t, 5>, 5> pattern, std::tuple<int, int, int> o) {
	std::reverse(pattern.begin(), pattern.begin() + h);
	return {w, h, pattern, o};
}

const auto tspin1 = create_match_pattern(
	3, 3,
	{
		{
			{{0, 0, 1}},
			{{0, 0, 0}},
			{{1, 0, 1}},
		}
	},
	{1, 1, 2}
);

const auto tspin2 = create_match_pattern(
	3, 3,
	{
		{
			{{1, 0, 0}},
			{{0, 0, 0}},
			{{1, 0, 1}},
		}
	},
	{1, 1, 2}
);

static inline const std::array<match_pattern, 2> tspindouble_matches = {tspin1, tspin2};


static const inline std::vector<match_pattern> bad_l_spots = {
	create_match_pattern(
		3, 3,
		{
			{
				{{0, 0, 1}},
				{{0, 0, 0}},
				{{1, 0, 1}},
			}
		},
		{1, 1, 2}
	),
};
