#pragma once
#include <array>
#include <utility>
//piece,old,new
static constexpr std::array<std::array<std::array<std::array<std::pair<int8_t, int8_t>, 4>, 4>, 4>, 7> rotation_table = {
	{
		{//L
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, 1},
								std::pair{0, -2},
								std::pair{1, -2},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//J
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, 1},
								std::pair{0, -2},
								std::pair{1, -2},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//O
			{
				{
					{//0
						{
							{//0
								std::pair{1, 0},
								std::pair{1, 0},
								std::pair{1, 0},
								std::pair{1, 0},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//0
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//I
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-2, 0},
								std::pair{1, 0},
								std::pair{-2, -1},
								std::pair{1, 2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{-1, 0},
								std::pair{2, 0},
								std::pair{-1, 2},
								std::pair{2, -1},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{2, 0},
								std::pair{-1, 0},
								std::pair{2, 1},
								std::pair{-1, -2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{2, 0},
								std::pair{-1, 2},
								std::pair{2, -1},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{1, 0},
								std::pair{-2, 0},
								std::pair{1, -2},
								std::pair{-2, 1},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{2, 0},
								std::pair{-1, 0},
								std::pair{2, 1},
								std::pair{-1, -2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{-2, 0},
								std::pair{1, -2},
								std::pair{-2, 1},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-2, 0},
								std::pair{1, 0},
								std::pair{-2, -1},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//T
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, 1},
								std::pair{0, -2},
								std::pair{1, -2},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//Z
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, 1},
								std::pair{0, -2},
								std::pair{1, -2},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		},
		{//S
			{
				{
					{//0
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, 1},
								std::pair{0, -2},
								std::pair{1, -2},
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//r
								std::pair{-1, 0},
								std::pair{-1, 1},
								std::pair{0, -2},
								std::pair{-1, -2},
							}
						},
						{
							{//2
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//l
								std::pair{1, 0},
								std::pair{1, -1},
								std::pair{0, 2},
								std::pair{1, 2},
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//r
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
						{
							{//2
								std::pair{-1, 0},
								std::pair{-1, -1},
								std::pair{0, 2},
								std::pair{-1, 2},
							}
						},
						{
							{//l
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
								std::pair{0, 0},
							}
						},
					}
				},
			}
		}
	}
};

static constexpr std::array<std::array<std::array<std::pair<int8_t, int8_t>, 5>, 4>, 4> rotation_kicks_normal = {
	{
		{//0
			{
				{//0
					{
						std::pair(0, 0),
						std::pair(0, 0),
						std::pair(0, 0),
						std::pair(0, 0),
						std::pair(0, 0)
					}
				},
				{//R
					std::pair(0, 0),
					std::pair(0, 0),
					std::pair(0, 0),
					std::pair(0, 0),
					std::pair(0, 0)
				}
			}
		}
	}
};

std::array<std::pair<int, int>, 5> rotations_offsets_to_test(int piece, int orientation, int new_orientation) {

	if (piece == 3) {
		if (orientation == new_orientation) {
			return {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
		}
		if (orientation == 0 && new_orientation == 1) {
			//return 
		}


	} else if (piece == 2) {
		return {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
	} else {
		if (orientation == new_orientation) {
			return {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
		}


	}

	return {};
}
