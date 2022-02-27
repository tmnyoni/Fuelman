#include "database.h"

fuelman_db::fuelman_db(database::connection& connection) :
	_connection(connection)
{}

bool fuelman_db::connect(std::string& error) {
	if (!_connection.connect(error))
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Coupons "
		"(Date REAL, 'Serial Number' TEXT, Fuel TEXT, Volume REAL, 'Issued By' TEXT, PRIMARY KEY('Serial Number'));",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS DispatchedStatus "
		"(Date REAL, 'Serial Number' TEXT, PRIMARY KEY(Date));",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS DispatchedTo "
		"('Serial Number' TEXT, Department TEXT, Receiver TEXT);",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Departments "
		"(ID TEXT NOT NULL, Name TEXT NOT NULL, PRIMARY KEY(ID));",
		{}, error)
		)

		return false;
	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Users "
		"(Username TEXT, Password TEXT, UserType TEXT, FirstName TEXT, LastName TEXT, IDNo TEXT)",
		{}, error)
		)
		return false;

	if (!_connection.execute("CREATE TABLE IF NOT EXISTS Vouchers "
		"(VoucherNumber TEXT, FuelVolume TEXT, ReceivedDate REAL, ReceivedFrom TEXT)",
		{}, error)
		)
		return false;

	return true;
}

bool fuelman_db::on_dispatch_coupons(database::row& table, std::string& error) {

	if (!_connection.execute("INSERT INTO DispatchedStatus VALUES(?, ?);",
		{
			db_get::real(table.at("Date")),
			db_get::text(table.at("Serial Number"))
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
				row.at("Date"),
				row.at("Serial Number"),
				row.at("Fuel"),
				row.at("Volume"),
				row.at("Issued By"),
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
	if (!_connection.execute_query(sql_query, {}, results_table, error))
		return false;

	table = results_table.data;
	return true;
}

bool fuelman_db::on_get_coupon(const std::any& serial_number, std::vector<database::row>& table_, std::string& error) {
	database::table results_table;
	if (!_connection.execute_query("SELECT * FROM Coupons WHERE \"Serial Number\" = '" + database::get::text(serial_number) + "';", {}, results_table, error))
		return false;

	table_ = results_table.data;
	return true;
}

bool fuelman_db::on_get_dispatched_coupons(std::vector<dispatched_coupon_struct>& coupons, std::string& error) {
	coupons.clear();

	// get dispatched coupons in descending time
	std::vector<std::string> serial_numbers_order;
	std::map<std::string, double> serial_numbers;

	{
		database::table results;
		if (!_connection.execute_query("SELECT * FROM DispatchedStatus ORDER BY Date DESC;", {}, results, error))
			return false;

		for (auto& row : results.data) {
			std::string serial_number;
			double date = 0;

			try {
				if (row.at("Serial Number").has_value()) {
					serial_number = liblec::leccore::database::get::text(row.at("Serial Number"));
				}
				if (row.at("Date").has_value()) {
					date = liblec::leccore::database::get::real(row.at("Date"));
				}

				serial_numbers_order.push_back(serial_number);
				serial_numbers.insert(std::make_pair(serial_number, date));
			}
			catch (const std::exception& e) {
				error = e.what();
				return false;
			}
		}
	}

	for (const auto& serial_number : serial_numbers_order) {
		double volume = 0;
		std::string fuel;
		{
			// get the details
			std::vector<database::row> table;
			if (!on_get_coupon(serial_number, table, error))
				return false;

			// we already have the serial number and the dispatch date, now we need the volume of the coupon
			for (auto& row : table) {
				try {
					if (row.at("Volume").has_value()) {
						volume = liblec::leccore::database::get::real(row.at("Volume"));
					}
					if (row.at("Fuel").has_value()) {
						fuel = liblec::leccore::database::get::text(row.at("Fuel"));
					}

					break;	// expecting one value anyway
				}
				catch (const std::exception& e) {
					error = e.what();
					return false;
				}
			}
		}

		std::string receiving_department;
		{
			// get the details
			database::table results;
			if (!_connection.execute_query("SELECT * FROM DispatchedTo WHERE \"Serial Number\" = ? ; ", { serial_number }, results, error))
				return false;

			for (auto& row : results.data) {

				try {
					if (row.at("Department").has_value()) {
						receiving_department = liblec::leccore::database::get::text(row.at("Department"));
					}

					break;	// expecting one value anyway
				}
				catch (const std::exception& e) {
					error = e.what();
					return false;
				}
			}
		}

		dispatched_coupon_struct coupon;
		coupon.date = serial_numbers.at(serial_number);
		coupon.serial_number = serial_number;
		coupon.receiving_department = receiving_department;
		coupon.volume = volume;
		coupon.fuel = fuel;

		coupons.push_back(coupon);
	}

	return true;
}

bool fuelman_db::add_department(const std::string& name, std::string& error)
{
	if (name.empty()) {
		error = "Name not supplied";
		return false;
	}

	if (!_connection.execute("INSERT INTO Departments VALUES(?, ?);",
		{
			name,	// use the name as the ID for simplicity
			name
		},
		error))
		return false;

	return true;
}

bool fuelman_db::get_departments(std::vector<std::string>& departments, std::string& error)
{
	database::table results;
	if (!_connection.execute_query("SELECT ID, Name FROM Departments;", {}, results, error))
		return false;

	for (auto& row : results.data) {
		std::string department;

		try {
			if (row.at("Name").has_value()) {
				department = liblec::leccore::database::get::text(row.at("Name"));
				departments.push_back(department);
			}
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}

	return true;
}

//// This function is unfinished.
bool fuelman_db::on_departments_stats(std::vector<database::row>& departmental_stats, std::string& error) {
	const std::string sql_query =
		"SELECT Department, Volume FROM Coupons INNER JOIN DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\""
		"INNER JOIN DispatchedTo ON DispatchedTo.\"Serial Number\" = DispatchedStatus.\"Serial Number\";";

	database::table results_table;
	if (!_connection.execute_query(sql_query, {}, results_table, error))
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

double fuelman_db::available_petrol() {
	double total = 0.0;

	// get available coupons
	const std::string sql_query =
		"SELECT * FROM Coupons WHERE \"Serial Number\" NOT IN "
		"(SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN "
		"DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\");";

	std::string error;
	database::table results;
	if (!_connection.execute_query(sql_query, {}, results, error))
		return total;

	for (const auto& row : results.data) {
		if (db_get::text(row.at("Fuel")).compare("Petrol") == 0) {
			const auto volume = db_get::real(row.at("Volume"));
			total += volume;
		}
	}

	return total;
}

double fuelman_db::available_diesel() {
	double total = 0.0;

	// get available coupons
	const std::string sql_query =
		"SELECT * FROM Coupons WHERE \"Serial Number\" NOT IN "
		"(SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN "
		"DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\");";

	std::string error;
	database::table results;
	if (!_connection.execute_query(sql_query, {}, results, error))
		return total;

	for (const auto& row : results.data) {
		if (db_get::text(row.at("Fuel")).compare("Diesel") == 0) {
			const auto volume = db_get::real(row.at("Volume"));
			total += volume;
		}
	}

	return total;
}

double fuelman_db::dispatched_petrol() {
	double total = 0.0;

	// get available coupons
	const std::string sql_query =
		"SELECT * FROM Coupons WHERE \"Serial Number\" IN "
		"(SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN "
		"DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\");";

	std::string error;
	database::table results;
	if (!_connection.execute_query(sql_query, {}, results, error))
		return total;

	for (const auto& row : results.data) {
		if (db_get::text(row.at("Fuel")).compare("Petrol") == 0) {
			const auto volume = db_get::real(row.at("Volume"));
			total += volume;
		}
	}

	return total;
}

double fuelman_db::dispatched_diesel() {
	double total = 0.0;

	// get available coupons
	const std::string sql_query =
		"SELECT * FROM Coupons WHERE \"Serial Number\" IN "
		"(SELECT DispatchedStatus.\"Serial Number\" FROM Coupons INNER JOIN "
		"DispatchedStatus ON DispatchedStatus.\"Serial Number\" = Coupons.\"Serial Number\");";

	std::string error;
	database::table results;
	if (!_connection.execute_query(sql_query, {}, results, error))
		return total;

	for (const auto& row : results.data) {
		if (db_get::text(row.at("Fuel")).compare("Diesel") == 0) {
			const auto volume = db_get::real(row.at("Volume"));
			total += volume;
		}
	}

	return total;
}
