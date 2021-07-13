#pragma once

#include <vector>
#include <map>
#include <any>
#include <algorithm>

#include <liblec/leccore/database.h>

using namespace liblec::leccore;
using row = std::map<std::string, std::string>;
using db_get = database::get;

class fuelman_db {
	database::connection con{ "sqlcipher", "fuelman.db", "pass@123" };

public:
	bool connect(std::string& error) {
		if (!con.connect(error))
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Coupons"
			"( Date TEXT, 'Serial Number' TEXT, Fuel TEXT, Volume TEXT, 'Issued By' TEXT);",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS DispatchedStatus"
			"( Date TEXT, 'Serial Number' TEXT);",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS DispatchedTo"
			"( 'Serial Number' TEXT, Department TEXT, Receiver TEXT );",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Departments"
			"( ID TEXT, Department TEXT);",
			{}, error)
			)

			return false;
		if (!con.execute("CREATE TABLE IF NOT EXISTS Users"
			"( Username TEXT, Password TEXT, UserType TEXT, FirstName TEXT, LastName TEXT, IDNo TEXT)",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Vouchers"
			"( VoucherNumber TEXT, FuelVolume TEXT, ReceivedDate TEXT, ReceivedFrom TEXT)",
			{}, error)
			)
			return false;

		return true;
	}

	bool on_dispatch_coupons(database::row& table, std::string& error) {

		if (!con.execute("INSERT INTO DispatchedStatus VALUES(?, ?);",
			{
				db_get::text(table.at("Date")).c_str(),
				db_get::text(table.at("Serial Number")).c_str()
			},
			error))
			return false;

		if (!con.execute("INSERT INTO DispatchedTo VALUES(?, ?, ?);",
			{
				db_get::text(table.at("Serial Number")).c_str(),
				db_get::text(table.at("Issued To")).c_str(),
				db_get::text(table.at("Receiver")).c_str()
			}, 
			error))
			return false;

		return true;
	}

	bool on_save_coupons(const std::vector<database::row>& table, std::string& error) {

		for (auto& row : table) {
			if (!con.execute("INSERT INTO Coupons VALUES(?, ?, ?, ?, ?);",
				{
					db_get::text(row.at("Date")).c_str(),
					db_get::text(row.at("Serial Number")).c_str(),
					db_get::text(row.at("Fuel")).c_str(),
					db_get::text(row.at("Volume")).c_str(),
					db_get::text(row.at("Issued By")).c_str(),
				},
				error))
				return false;
		}

		return true;
	}

	bool on_get_coupons(std::vector<database::row>& table, std::string& error) {

		database::table results_table;
		if (!con.execute_query("SELECT * FROM Coupons WHERE \"Serial Number\" NOT IN (SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\"); ", results_table, error))
			return false;

		table = results_table.data;
		return true;
	}

	bool on_get_coupon(const std::any& serial_number, std::vector<database::row>& table_, std::string& error) {

		database::table results_table;
		if (!con.execute_query("SELECT * FROM Coupons WHERE \"Serial Number\" = '" + database::get::text(serial_number) + "';", results_table, error))
			return false;

		table_ = results_table.data;
		return true;
	}

	bool on_get_total_volume(double& total_volume, std::string& error){
		std::vector<database::row> table_;

		on_get_coupons(table_, error);

		double total_volume_ = 0;

		for (auto& row_ : table_){
			if (std::all_of(database::get::text(row_.at("Volume")).begin(), database::get::text(row_.at("Volume")).end(), ::isdigit))
				total_volume_ += database::get::real(row_.at("Volume"));
		}

		total_volume = total_volume_;
		return true;
	}
};