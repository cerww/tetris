#pragma once
#include <SFML/Graphics.hpp>

#include "tetris_stuff.h"
#include <range/v3/all.hpp>

inline sf::Color piece_color(tetris_piece p) {
	static const auto colors = std::array{
		sf::Color(255, 165, 0),
		sf::Color::Blue,
		sf::Color::Yellow,
		sf::Color(100, 100, 255),
		sf::Color::Magenta,
		sf::Color::Red,
		sf::Color::Green
	};
	return colors[(int)p];
}

inline sf::Color ghost_piece_color(tetris_piece p) {
	const auto a = piece_color(p);
	return sf::Color(a.r, a.g, a.b, 100);
}

inline sf::Color mino_to_color(tetris_block mino_color) {
	static const auto colors = std::array{
		sf::Color(220, 220, 220),
		sf::Color(255, 165, 0),
		sf::Color::Blue,
		sf::Color::Yellow,
		sf::Color(100, 100, 255),
		sf::Color::Magenta,
		sf::Color::Red,
		sf::Color::Green,
		sf::Color(100, 100, 100),
	};
	return colors[(int)mino_color];
}

inline void draw_piece(sf::RenderWindow& window, std::span<const std::pair<int8_t, int8_t>> offsets, int x, int y, sf::Color color, double scale = 1.0) {
	for (const auto piece : offsets) {
		sf::RectangleShape block(sf::Vector2f(20, 20) * (float)scale);
		block.setPosition(x + piece.first * 20.f * (float)scale, y - piece.second * 20.f * (float)scale);
		block.setFillColor(color);
		block.setOutlineThickness(-0.8f);
		block.setOutlineColor(sf::Color(200, 200, 200, 180));
		window.draw(block);
	}
}

inline void draw_tetris_board(sf::RenderWindow& window, const tetris_game_update& stuff, int x, int y) {
	constexpr int board_height = 400;
	constexpr int board_width = 200;


	sf::RectangleShape board_outline(sf::Vector2f{ 200, board_height });
	board_outline.setPosition((float)x, (float)y - board_height + 20);
	board_outline.setOutlineColor(sf::Color::Green);
	board_outline.setOutlineThickness(1);
	window.draw(board_outline);

	for (const auto [i, piece] : ranges::views::enumerate(stuff.game_state.preview_pieces | ranges::views::take(5))) {
		draw_piece(
			window, piece_offsets[(int)piece][0],
			x + 200 + 50, y + 60 * i - 350,
			piece_color(piece),
			0.9
		);
	}

	for (int i = 0; i < stuff.game_state.board.minos.size(); ++i) {
		for (int j = 0; j < std::min((int)stuff.game_state.board.minos[i].size(), 32); ++j) {
			if (j >= 20 && stuff.game_state.board.minos[i][j] == tetris_block::empty) {
				continue;
			}
			sf::RectangleShape block(sf::Vector2f(20, 20));
			block.setPosition(x + i * 20, y - j * 20);
			block.setFillColor(mino_to_color(stuff.game_state.board.minos[i][j]));
			block.setOutlineThickness(-0.8f);
			block.setOutlineColor(sf::Color(200, 200, 200, 180));
			window.draw(block);
		}
	}

	if (stuff.game_state.current_piece != tetris_piece::no_piece) {
		draw_piece(window, piece_offsets[(int)stuff.game_state.current_piece][stuff.game_state.orientation],
			stuff.game_state.piece_center_x * 20 + x, y - stuff.game_state.piece_center_y * 20,
			piece_color(stuff.game_state.current_piece));
		draw_piece(window, piece_offsets[(int)stuff.game_state.current_piece][stuff.game_state.orientation],
			stuff.game_state.piece_center_x * 20 + x, y - stuff.game_state.get_ghost_piece_y() * 20,
			ghost_piece_color(stuff.game_state.current_piece));
	}

	if (stuff.game_state.held_piece.has_value()) {
		draw_piece(
			window, piece_offsets[(int)stuff.game_state.held_piece.value()][0],
			x - 70, y - board_height + 60,
			piece_color(stuff.game_state.held_piece.value()),
			0.9
		);
	}
}
