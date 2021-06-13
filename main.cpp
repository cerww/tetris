#include <iostream>
#include <unordered_set>


#include "tetris_stuff.h"
#include "sfml_event_handler.h"
#include "sfml_event_handler_extensions.h"
#include "keyboard_tetris_player.h"
#include <boost/asio.hpp>
#include "ai.h"
#include "tetris_ai_player.h"

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
	std::vector<std::pair<action, int>> joystick_binds = {
		{action::move_left, 14},
		{action::soft_drop, 13},
		{action::hard_drop, 12},
		{action::move_right, 15},
		{action::swap_held_piece, 4},
		{action::rotate_left, 1},
		{action::rotate_right, 2},
	};

	std::vector<std::tuple<action, sf::Joystick::Axis, float>> joy_stick_axis = {
		{action::move_left, sf::Joystick::Axis::PovX, -0.5f},
		{action::move_right, sf::Joystick::Axis::PovX, 0.5f},
		{action::soft_drop, sf::Joystick::Axis::PovY, -0.5f},
		{action::hard_drop, sf::Joystick::Axis::PovY, 0.5f},
	};


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
	return sf::Color(a.r, a.g, a.b, 100);
}

sf::Color mino_to_color(tetris_block mino_color) {
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

void draw_piece(sf::RenderWindow& window, std::span<const std::pair<int, int>> offsets, int x, int y, sf::Color color, double scale = 1.0) {
	for (const auto piece : offsets) {
		sf::RectangleShape block(sf::Vector2f(20, 20) * (float)scale);
		block.setPosition(x + piece.first * 20.f * (float)scale, y - piece.second * 20.f * (float)scale);
		block.setFillColor(color);
		block.setOutlineThickness(-0.8f);
		block.setOutlineColor(sf::Color(200, 200, 200, 180));
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

	for (const auto [i,piece] : ranges::views::enumerate(stuff.game_state.preview_pieces | ranges::views::take(5))) {
		draw_piece(
			window, piece_offsets[(int)piece][0],
			x + 200 + 50, y + 60 * i - 350,
			piece_color(piece),
			0.9
		);
	}

	for (int i = 0; i < stuff.game_state.board.minos.size(); ++i) {
		for (int j = 0; j < std::min((int)stuff.game_state.board.minos[i].size(), 20); ++j) {
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

enum struct game_state {
	open,
	trasition_to_playing,
	playing,
	dying,
	dead,
	settings,

};

struct starting {
	std::chrono::milliseconds time_to_start = 3s;
};

struct dying_state {
	std::chrono::milliseconds time_till_dead_for_real = 2s;
};

struct dead { };


int main() {
	boost::asio::io_context executer;
	auto y = std::jthread([&]() {
		auto work_guard = boost::asio::make_work_guard(executer);
		executer.run();
	});
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
			.soft_drop_multiplier = 50
		}
	);

	
	auto ai_player = tetris_ai_player(executer.get_executor(),ai_settings{
		.piece_delay = 500ms
	});
	
	std::array<bool, (int)action::size> actions = {};

	player.start_doing_stuff_now();
	ai_player.start_doing_stuff();

	while (window.isOpen()) {
		event_handler.poll_stuff();
		std::array<bool, (int)action::size> actions_this_frame = {};
		std::ranges::fill(actions_this_frame, false);

		for (auto [action, key] : settings.keybinds) {
			if (event_handler.is_held(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, key] : settings.joystick_binds) {
			if (event_handler.is_any_pressed(key)) {
				actions_this_frame[(int)action] = true;
			}
		}

		for (auto [action, axis,amount] : settings.joy_stick_axis) {
			if (amount < 0) {
				if (sf::Joystick::getAxisPosition(0, axis) <= amount) {
					actions_this_frame[(int)action] = true;
				}
			} else if (amount > 0) {
				if (sf::Joystick::getAxisPosition(0, axis) >= amount) {
					actions_this_frame[(int)action] = true;
				}
			}
		}
		std::array<bool, (int)action::size> new_actions_this_frame = {};

		for (auto [a,b,c] : ranges::views::zip(actions, actions_this_frame, new_actions_this_frame)) {
			c = b && !a;
		}

		actions = actions_this_frame;

		const auto game_update = player.get_update();
		const auto ai_game_update = ai_player.get_update();
		if (game_update.died) {
			player.stop_doing_stuff();
		}
		if (game_update.garbage_sent) {
			std::cout << game_update.garbage_sent << std::endl;
		}
		player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, ai_game_update.garbage_sent);
		ai_player.receive_update(event_handler.time_since_last_poll(), game_update.garbage_sent);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		draw_tetris_board(window, ai_game_update, 600, 600);
		window.display();
	}

	return 0;
}
