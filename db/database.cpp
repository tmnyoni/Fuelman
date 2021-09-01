#include "database.h"

fuelman_db::fuelman_db(database::connection& connection) :
	_connection(connection)
{}

bool fuelman_db::connect(std::string& error) {
	if (!_connection.connect(error))
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Coupons "
		"(Date TEXT, 'Serial Number' TEXT, Fuel TEXT, Volume TEXT, 'Issued By' TEXT);",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS DispatchedStatus "
		"(Date TEXT, 'Serial Number' TEXT);",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS DispatchedTo "
		"('Serial Number' TEXT, Department TEXT, Receiver TEXT);",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Departments "
		"(ID TEXT, Department TEXT);",
		{}, error)
		)

		return false;
	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Users "
		"(Username TEXT, Password TEXT, UserType TEXT, FirstName TEXT, LastName TEXT, IDNo TEXT)",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Vouchers "
		"(VoucherNumber TEXT, FuelVolume TEXT, ReceivedDate TEXT, ReceivedFrom TEXT)",
		{}, error)
		)
		return false;

	return true;
}

bool fuelman_db::on_dispatch_coupons(database::row& table, std::string& error) {

	if (!_connection.execute("INSERT INTO DispatchedStatus VALUES(?, ?);",
		{
			db_get::text(table.at("Date")).c_str(),
			db_get::text(table.at("Serial Number")).c_str()
		},
		error))
		return false;

	if (!_connection.execute("INSERT INTO DispatchedTo VALUES(?, ?, ?);",
		{
			db_get::text(table.at("Serial Number")).c_str(),
			db_get::text(table.at("Issued To")).c_str(),
			db_get::text(table.at("Receiver")).c_str()
		},
		error))
		return false;

	return true;
}

bool fuelman_db::on_save_coupons(const std::vector<database::row>& table, std::string& error) {

	for (auto& row : table) {
		if (!_connection.execute("INSERT INTO Coupons VALUES(?, ?, ?, ?, ?);",
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

bool fuelman_db::on_get_coupons(std::vector<database::row>& table, std::string& error) {
	const std::string sql_query =
		"SELECT * FROM Coupons WHERE \"Serial Number\" NOT IN "
		"(SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN "
		"DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\");";

	database::table results_table;
	if (!_connection.execute_query(sql_query, results_table, error))
		return false;

	table = results_table.data;
	return true;
}

bool fuelman_db::on_get_coupon(const std::any& serial_number, std::vector<database::row>& table_, std::string& error) {
	const std::string sql_query = "SELECT * FR";

	database::table results_table;
	if (!_connection.execute_query("SELECT * FROM Coupons WHERE \"Serial Number\" = '" + database::get::text(serial_number) + "';", results_table, error))
		return false;

	table_ = results_table.data;
	return true;
}

//// This function is unfinished.
bool fuelman_db::on_departments_stats(std::vector<database::row>& departmental_stats, std::string& error) {
	const std::string sql_query =
		"SELECT Department, Volume FROM Coupons INNER JOIN DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\""
		"INNER JOIN DispatchedTo ON DispatchedTo.\"Serial Number\" = DispatchedStatus.\"Serial Number\";";

	database::table results_table;
	if (!_connection.execute_query(sql_query, results_table, error))
		return false;

	bool is_found = false;
	for (const auto& row : results_table.data) {
		if (!departmental_stats.empty()) {

			for (const auto& _row : departmental_stats) {
				if (db_get::text(_row.at("Department")) == db_get::text(row.at("Department"))) {
					is_found = true;

					/*int volume = db_get::integer(_row.at("Volume"));
					volume += db_get::integer(row.at("Volume"));*/

					//_row.at("Volume") = volume;
					break;

				}
			}
		}

		if (!is_found)
			departmental_stats.push_back({
				{ "Department", db_get::text(row.at("Department"))},
				{ "Volume", db_get::text(row.at("Volume"))}
				});

		is_found = false;
	}
	return true;
}

int fuelman_db::on_get_petrol_volume() {

	std::string error;
	std::vector<database::row> table_;
	if (!on_get_coupons(table_, error))
		throw error;

	int petrol_volume_ = 0;
	try
	{
		for (auto& row_ : table_) {
			if (db_get::text(row_.at("Fuel")).compare("Petrol") == 0) {

				const auto volume = db_get::text(row_.at("Volume"));
				if (std::all_of(volume.begin(), volume.end(), ::isdigit))
					petrol_volume_ += std::atoi(volume.c_str());
			}
		}
	}
	catch (const std::exception& ex)
	{
		throw std::string(ex.what());
	}


	return petrol_volume_;
}

int fuelman_db::on_get_diesel_volume() {

	std::string error;
	std::vector<database::row> table_;
	if (!on_get_coupons(table_, error))
		throw error;

	int diesel_volume_ = 0;
	try
	{
		for (auto& row_ : table_) {
			if (db_get::text(row_.at("Fuel")).compare("Diesel") == 0) {

				const auto volume = database::get::text(row_.at("Volume"));
				if (std::all_of(volume.begin(), volume.end(), ::isdigit))
					diesel_volume_ += std::atoi(volume.c_str());
			}
		}
	}
	catch (const std::exception& ex) {
		throw std::string(ex.what());
	}

	return diesel_volume_;
}