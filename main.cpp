#include <iostream>

#include "tetris_stuff.h"
#include "sfml_event_handler.h"
#include "sfml_event_handler_extensions.h"

struct game_settings {
	std::vector<std::pair<action, sf::Keyboard::Key>> keybinds = {
		{action::move_left, sf::Keyboard::Left},
		{action::soft_drop, sf::Keyboard::Down},
		{action::hard_drop, sf::Keyboard::Space},
		{action::move_right, sf::Keyboard::Right},
		{action::swap_held_piece, sf::Keyboard::LShift},
		{action::rotate_left, sf::Keyboard::Z},
		{action::rotate_right, sf::Keyboard::X},
	};
	std::vector<std::pair<action, int>> joystick_binds;


};

sf::Color piece_color(tetris_piece p) {
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

sf::Color ghost_piece_color(tetris_piece p) {
	const auto a = piece_color(p);
	return sf::Color(a.a, a.g, a.b, 100);
}

sf::Color mino_to_color(tetris_block mino_color) {
	static const auto colors = std::array{
		sf::Color(255, 165, 0),
		sf::Color::Blue,
		sf::Color::Yellow,
		sf::Color(100, 100, 255),
		sf::Color::Magenta,
		sf::Color::Red,
		sf::Color::Green,
		sf::Color(100, 100, 100),
	};
	return colors[(int)mino_color - 1];
}

void draw_piece(sf::RenderWindow& window, std::span<const std::pair<int, int>> offsets, int x, int y, sf::Color color, double scale = 1.0) {
	for (const auto piece : offsets) {
		sf::RectangleShape block(sf::Vector2f(20, 20) * (float)scale);
		block.setPosition(x + piece.first * 20 * scale, y - piece.second * 20 * scale);
		block.setFillColor(color);
		window.draw(block);
	}

}


void draw_tetris_board(sf::RenderWindow& window, const tetris_game_update& stuff, int x, int y) {
	constexpr int board_height = 400;
	constexpr int board_width = 200;


	sf::RectangleShape board_outline(sf::Vector2f{200, board_height});
	board_outline.setPosition((float)x, (float)y - board_height + 20);
	board_outline.setOutlineColor(sf::Color::Green);
	board_outline.setOutlineThickness(1);
	window.draw(board_outline);
	//draw lines

	for (int i = 0; i < 10; ++i) {
		sf::RectangleShape gridline_vertical(sf::Vector2f(1, 400));
		gridline_vertical.setPosition(x + i * 20, y - 400 + 20);
		gridline_vertical.setFillColor(sf::Color(90, 90, 90, 180));
		window.draw(gridline_vertical);
	}

	for (int i = 0; i < 20; ++i) {
		sf::RectangleShape gridline_horizontal(sf::Vector2f(200, 1));
		gridline_horizontal.setPosition(x, y - i * 20 + 20);
		gridline_horizontal.setFillColor(sf::Color(90, 90, 90, 180));
		window.draw(gridline_horizontal);
	}

	if (stuff.game_state.current_piece != tetris_piece::no_piece) {
		draw_piece(window, piece_offsets[(int)stuff.game_state.current_piece][stuff.game_state.orientation],
				   stuff.game_state.piece_center_x * 20 + x, y - stuff.game_state.piece_center_y * 20,
				   piece_color(stuff.game_state.current_piece));
		draw_piece(window, piece_offsets[(int)stuff.game_state.current_piece][stuff.game_state.orientation],
			stuff.game_state.piece_center_x * 20 + x, y - stuff.game_state.get_ghost_piece_y()* 20 - 20,
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

	for (const auto [i,piece] : ranges::views::enumerate(stuff.game_state.preview_pieces | ranges::views::take(5))) {
		draw_piece(
			window, piece_offsets[(int)piece][0],
			x + 200 + 50, y + 60 * i - 350,
			piece_color(piece),
			0.9
		);
	}

	for (int i = 0; i < stuff.game_state.board.minos.size(); ++i) {
		for (int j = 0; j < stuff.game_state.board.minos[i].size(); ++j) {
			if (stuff.game_state.board.minos[i][j] != tetris_block::empty) {
				sf::RectangleShape block(sf::Vector2f(20, 20));
				block.setPosition(x + i * 20, y - j * 20);
				block.setFillColor(mino_to_color(stuff.game_state.board.minos[i][j]));
				window.draw(block);
			}
		}
	}


}

enum struct game_state {
	starting,
	playing,
	dying,
	dead,
};

struct starting {
	std::chrono::milliseconds time_to_start = 3s;
};

struct dying_state {
	std::chrono::milliseconds time_till_dead_for_real = 2s;
};

struct dead { };


int main() {
	sf::RenderWindow window(sf::VideoMode(1500, 800), "wat");

	sfml_event_handler<track_hold_times<>> event_handler(window);

	game_settings settings;

	window.setFramerateLimit(60);
	static constexpr int awasdasd = sizeof(track_hold_times<>);
	auto player = tetris_game_keyboard_player(tetris_game_settings{
	.das_time = 70ms,
	.arr_time = 14ms,
	.delay_between_drops = 20ms,
	.garbage_delay = 30ms,
	.max_soft_dropping_time = 5s,
	.soft_drop_multiplier = 10
	}
	);

	std::array<bool, (int)action::size> actions = {};

	player.start_doing_stuff_now();

	while (window.isOpen()) {
		event_handler.poll_stuff();
		std::array<bool, (int)action::size> actions_this_frame = {};
		std::ranges::fill(actions_this_frame, false);

		for (auto [action, key] : settings.keybinds) {
			if (event_handler.is_held(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		std::array<bool, (int)action::size> new_actions_this_frame = {};

		for (auto [a,b,c] : ranges::views::zip(actions, actions_this_frame, new_actions_this_frame)) {
			c = b && !a;
		}

		actions = actions_this_frame;

		const auto game_update = player.get_update();
		if(game_update.died) {
			player.stop_doing_stuff();
		}
		player.recieve_update({{actions}, {new_actions_this_frame}, event_handler.time_since_last_poll()}, 0);

		window.clear(sf::Color(100,100,100));
		draw_tetris_board(window, game_update, 200, 600);
		window.display();
	}

	return 0;
}
