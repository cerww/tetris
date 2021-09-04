#pragma once
#include <optional>
#include <string>
#include <unordered_map>


struct imbutton {
	
};


struct imbuttongroup {


	
private:
	
	struct button_state {
		bool hover = false;
		bool clicked = false;
		std::optional<std::string> key;
	};
	std::unordered_map<std::string, int> m_key_idxs;
	std::vector<button_state> m_prev_button_states;
	std::string m_last_tag;
};




