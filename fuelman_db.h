#pragma once

#include <vector>
#include <map>
#include <any>

#include <liblec/leccore/database.h>

using namespace liblec::leccore;
using table_ = std::vector<std::map<std::string, std::any>>;

class fuelman_db{
	database::connection con{ "sqlcipher", "fuelman.db", "pass@123" };

public:
	bool connect(std::string& error) {
		if (!con.connect(error))
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Coupons"
			"( Date TEXT, SerialNo TEXT, Quantity TEXT, Issuedto TEXT, Recvby TEXT, Comments TEXT)",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Users"
			"( Username TEXT, Password TEXT, Usertype TEXT, Firstname TEXT, Lastname TEXT, IDNo TEXT)",
			{}, error)
			)
			return false;

		if (!con.execute("CREATE TABLE IF NOT EXISTS Vouchers"
			"( VoucherNo TEXT, FuelVolume TEXT, RecvDate TEXT, RecvFrom TEXT)",
			{}, error)
			)
			return false;

		return true;
	}

	bool on_dispatch_coupons(const table_& table, std::string& error){
		
		for (const auto& row : table)
			if (!con.execute("INSERT INTO Coupons VALUES(?, ?, ?, ?, ?, ?);",
				{ row.at("date"), row.at("serialno"), row.at("quantity"), row.at("issuedto"), row.at("recvby"), row.at("comments") },
				error))
				return false;

		return true;
	}
	
	bool on_get_coupons(table_& table, std::string& error){
		
		database::table results_table;
		if (!con.execute_query("SELECT * FROM Coupons;", results_table, error))
			return false;

		table = results_table.data;

		return true;
	}

	table_ coupons_data = {
		{ {"Id", "1"}, {"Date", "10-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"Id", "2"}, {"Date", "11-June-20"}, {"Issued to", "Accounts"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"Id", "3"}, {"Date", "12-June-20"}, {"Issued to", "Admini"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"Id", "4"}, {"Date", "13-June-20"}, {"Issued to", "ICT"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"Id", "5"}, {"Date", "14-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"Id", "6"}, {"Date", "15-June-20"}, {"Issued to", "Ambulance"}, {"Serial Number", "fhdskfhasdfhasi34"} }
	};

public:
	bool get_coupons(table_& coupons, std::string& error){
		coupons = coupons_data;
		return true;
	}
};