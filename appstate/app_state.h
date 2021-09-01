#pragma once
#include "../db/database.h"

class state {
	state() = delete;
	fuelman_db db_;

public:
	state(database::connection& connection) :
		db_(connection) {}

	fuelman_db& get_db(){
		return db_;
	}
};
