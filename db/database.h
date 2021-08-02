#pragma once

#include <any>
#include <algorithm>
#include <map>
#include <vector>


#include <liblec/leccore/database.h>

using namespace liblec::leccore;
using db_get = database::get;

class fuelman_db {
	database::connection _connection{ "sqlcipher", "fuelman.db", "pass@123" };

public:
	bool connect(std::string& error);
	bool on_dispatch_coupons(database::row& table, std::string& error);
	bool on_save_coupons(const std::vector<database::row>& table, std::string& error);
	bool on_get_coupons(std::vector<database::row>& table, std::string& error);
	bool on_get_coupon(const std::any& serial_number, std::vector<database::row>& table_, std::string& error);

	//// This function is unfinished.
	std::vector<database::row> on_update_departmental_stats();
	int on_get_petrol_volume();
	int on_get_diesel_volume();
};