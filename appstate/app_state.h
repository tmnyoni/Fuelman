#pragma once
#include "../db/database.h"

class state {
	fuelman_db db_;

public:
	fuelman_db& get_db(){
		return db_;
	}
};