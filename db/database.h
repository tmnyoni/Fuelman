/*
** MIT License
**
** Copyright(c) 2021 Tawanda M. Nynoi
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files(the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions :
**
** The above copyright noticeand this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

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
	bool on_departments_stats(std::vector<database::row>& departmnets_stats, std::string& error);
	int on_get_petrol_volume();
	int on_get_diesel_volume();
};