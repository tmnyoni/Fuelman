#pragma once

#include <vector>
#include <map>
#include <any>

#include <liblec/leccore/database.h>

using namespace liblec::leccore;
using row = std::map<std::string, std::string>;

class fuelman_db {
	database::connection con{ "sqlcipher", "fuelman.db", "pass@123" };

public:
	bool connect(std::string& error) {
		if (!con.connect(error))
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Coupons"
			"( Date TEXT, FuelType TEXT, SerialNumber TEXT, QuantityIssued TEXT, IssuedTo TEXT, ReceivedBy TEXT, Comments TEXT)",
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

	bool on_dispatch_coupons(row& table, std::string& error) {

		if (!con.execute("INSERT INTO Coupons VALUES(?, ?, ?, ?, ?, ?, ?);",
			//"'" + table.at("date") + "','" + table.at("serialno") + "', '" + table.at("quantity") + "','" + table.at("issuedto") + "', '" + table.at("recvby") + "', '" + table.at("comments") + "');",
			{
				table.at("Date").c_str(),
				table.at("FuelType").c_str(),
				table.at("SerialNumber").c_str(),
				table.at("QuantityIssued").c_str(),
				table.at("IssuedTo").c_str(),
				table.at("ReceivedBy").c_str(),
				table.at("Comments").c_str()
			},
			error))
			return false;
		return true;
	}

	bool on_get_coupons(std::vector<database::row>& table, std::string& error) {

		database::table results_table;
		if (!con.execute_query("SELECT * FROM Coupons;", results_table, error))
			return false;

		table = results_table.data;
		return true;
	}

	bool on_get_coupon(const std::string& serial_number, std::vector<database::row>& table_, std::string& error) {

		database::table results_table;
		if (!con.execute_query("SELECT * FROM Coupons WHERE SerialNumber = '" + serial_number + "';", results_table, error))
			return false;

		table_ = results_table.data;
		return true;
	}

};