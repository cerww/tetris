#pragma once
#include <array>
#include <utility>
//piece,old,new


struct small_pair {
	small_pair() = default;
	small_pair(int8_t a,int8_t b) {
		
	}

	
};

static constexpr std::array<std::array<std::array<std::array<std::pair<int8_t, int8_t>, 4>, 4>, 4>, 7> rotation_table = {
	{
		{//L
			{
				{
					{//0
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, 1),
								std::pair(0, -2),
								std::pair(1, -2),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, 1),
								std::pair(0, -2),
								std::pair(1, -2),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(1, 0),
								std::pair(1, 0),
								std::pair(1, 0),
								std::pair(1, 0),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//0
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-2, 0),
								std::pair(1, 0),
								std::pair(-2, -1),
								std::pair(1, 2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(-1, 0),
								std::pair(2, 0),
								std::pair(-1, 2),
								std::pair(2, -1),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(2, 0),
								std::pair(-1, 0),
								std::pair(2, 1),
								std::pair(-1, -2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(2, 0),
								std::pair(-1, 2),
								std::pair(2, -1),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(1, 0),
								std::pair(-2, 0),
								std::pair(1, -2),
								std::pair(-2, 1),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(2, 0),
								std::pair(-1, 0),
								std::pair(2, 1),
								std::pair(-1, -2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(-2, 0),
								std::pair(1, -2),
								std::pair(-2, 1),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-2, 0),
								std::pair(1, 0),
								std::pair(-2, -1),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, 1),
								std::pair(0, -2),
								std::pair(1, -2),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, 1),
								std::pair(0, -2),
								std::pair(1, -2),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, 1),
								std::pair(0, -2),
								std::pair(1, -2),
							}
						},
					}
				},
				{//r
					{
						{
							{//0
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
					}
				},
				{//2
					{
						{
							{//0
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//r
								std::pair(-1, 0),
								std::pair(-1, 1),
								std::pair(0, -2),
								std::pair(-1, -2),
							}
						},
						{
							{//2
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//l
								std::pair(1, 0),
								std::pair(1, -1),
								std::pair(0, 2),
								std::pair(1, 2),
							}
						},
					}
				},
				{//l
					{
						{
							{//0
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//r
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
							}
						},
						{
							{//2
								std::pair(-1, 0),
								std::pair(-1, -1),
								std::pair(0, 2),
								std::pair(-1, 2),
							}
						},
						{
							{//l
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
								std::pair(0, 0),
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

constexpr auto wat = sizeof(rotation_table);

inline std::array<std::pair<int8_t, int8_t>, 4> rotations_offsets_to_test(int piece, int orientation, int new_orientation) {
	return rotation_table[piece][orientation][new_orientation];
}
