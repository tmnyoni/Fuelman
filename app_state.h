#pragma once
#include "fuelman_db.h"

class state {
	fuelman_db db_;

	std::vector<std::string> selected_items_;
public:
	fuelman_db get_db(){
		return db_;
	}

	std::vector<std::string> get_selected_items(){
		return selected_items_;
	}

	void set_selected_items(std::vector<std::string>& items){
		selected_items_ = items;
	}
};