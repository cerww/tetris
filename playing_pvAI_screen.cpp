#include "playing_pvAI_screen.h"
#include "start_screen.h"



std::optional<screen_thingy> playing_pVai_state::update(event_handler_t& event_handler, game_keybinds& settings) {

	if(m_back_button.update(event_handler,event_handler.time_since_last_poll())) {
		return start_screen(game_data);
	}
	
	auto& window = event_handler.window();
	auto& player = *player_ptr;
	auto& ai_player = *ai_player_ptr;


	const auto [actions_this_frame, new_actions_this_frame] = next_actions_pressed(actions, event_handler, settings);

	actions = actions_this_frame;

	const auto game_update = player.get_update();
	const auto ai_game_update = ai_player.get_update();
	if (game_update.died || ai_game_update.died) {
		player.stop_doing_stuff();
		ai_player.stop_doing_stuff();
		//state = game_state::open;

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		draw_tetris_board(window, ai_game_update, 800, 600);

		return dead_state(std::move(player_ptr), std::move(ai_player_ptr),game_data,std::move(m_custom_bag));
	}
	else {
		if (game_update.garbage_sent) {
			std::cout << game_update.garbage_sent << std::endl;
			int garbage_left = game_update.garbage_sent;
			std::ranges::sort(garbage_to_player, std::greater<>(), [](auto& a) { return a.second; });
			while (!garbage_to_player.empty() && garbage_left) {
				const int garb_to_cancel = std::min((int)garbage_left, (int)garbage_to_player.back().first);
				garbage_to_player.back().first -= garb_to_cancel;
				garbage_left -= garb_to_cancel;
				if (garbage_to_player.back().first == 0) {
					garbage_to_player.pop_back();
				}
			}

			if (garbage_left) {
				garbage_to_ai.emplace_back(garbage_left, 1s);
			}
		}

		if (ai_game_update.garbage_sent) {
			int garbage_left = ai_game_update.garbage_sent;
			std::ranges::sort(garbage_to_ai, std::greater<>(), [](auto& a) { return a.second; });
			while (!garbage_to_ai.empty() && garbage_left) {
				const int garb_to_cancel = std::min((int)garbage_left, (int)garbage_to_ai.back().first);
				garbage_to_ai.back().first -= garb_to_cancel;
				garbage_left -= garb_to_cancel;
				if (garbage_to_ai.back().first == 0) {
					garbage_to_ai.pop_back();
				}
			}

			if (garbage_left) {
				garbage_to_player.emplace_back(garbage_left, 1s);
			}
		}

		int garbage_to_ai_this_update = 0;
		int garbage_to_player_this_update = 0;
		if (!garbage_to_ai.empty()) {
			const auto res = std::ranges::partition(garbage_to_ai, [&](auto a) { return a.second > event_handler.time_since_last_poll(); });
			for (auto& a : std::ranges::subrange(garbage_to_ai.begin(), res.begin())) {
				a.second -= event_handler.time_since_last_poll();
			}

			garbage_to_ai_this_update = ranges::accumulate(res | ranges::views::transform([](auto a) { return a.first; }), 0);
			garbage_to_ai.erase(res.begin(), res.end());
		}

		if (!garbage_to_player.empty()) {
			const auto res = std::ranges::partition(garbage_to_player, [&](auto a) { return a.second > event_handler.time_since_last_poll(); });
			for (auto& a : std::ranges::subrange(garbage_to_player.begin(), res.begin())) {
				a.second -= event_handler.time_since_last_poll();
			}

			garbage_to_player_this_update = ranges::accumulate(res | ranges::views::transform([](auto a) { return a.first; }), 0);
			garbage_to_player.erase(res.begin(), res.end());
		}


		player.recieve_update({ {actions}, {new_actions_this_frame}, event_handler.time_since_last_poll() }, garbage_to_player_this_update);
		//ai_player2.receive_update(event_handler.time_since_last_poll(), garbage_to_ai_this_update + 2 * new_actions_this_frame[7]);
		ai_player.receive_update(event_handler.time_since_last_poll(), garbage_to_ai_this_update + 0 * new_actions_this_frame[7]);

		window.clear(sf::Color(100, 100, 100));
		draw_tetris_board(window, game_update, 200, 600);
		draw_tetris_board(window, ai_game_update, 800, 600);
		m_back_button.draw(window);

		const auto heights = col_heights(game_update.game_state.board.minos);
		//std::cout << flatstacking_ai::covered_sections(game_update.game_state.board, heights)<<std::endl;
	}
	return std::nullopt;
}








