#pragma once

#include <any>
#include <algorithm>
#include <map>
#include <vector>

#include <liblec/leccore/database.h>

using namespace liblec::leccore;
using db_get = database::get;

class fuelman_db {
	fuelman_db() = delete;
	database::connection& _connection;

public:
	fuelman_db(database::connection& connection);
	bool connect(std::string& error);
	bool on_dispatch_coupons(database::row& table, std::string& error);
	bool on_save_coupons(const std::vector<database::row>& table, std::string& error);
	bool on_get_coupons(std::vector<database::row>& table, std::string& error);
	bool on_get_coupon(const std::any& serial_number, std::vector<database::row>& table_, std::string& error);
	bool on_get_dispatched_coupons(std::vector<database::row>& table, std::string& error);
	bool add_department(const std::string& name, std::string& error);
	bool get_departments(std::vector<std::string>& departments, std::string& error);

	//// This function is unfinished.
	bool on_departments_stats(std::vector<database::row>& departmnets_stats, std::string& error);
	int on_get_petrol_volume();
	int on_get_diesel_volume();
};