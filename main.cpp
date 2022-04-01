/*
** MIT License
**
** Copyright(c) 2021 Tawanda M. Nyoni
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

#include "gui/main_ui.h"
#include "appstate/app_state.h"
#include "version_info/version_info.h"
#include <liblec/leccore/system.h>
#include <liblec/leccore/file.h>

#pragma comment(linker, "/ENTRY:mainCRTStartup")

using namespace liblec::lecui;

int main() {
	bool restart = false;

	do {
		std::string error;
		
		// define database directory
		const std::string database_directory = liblec::leccore::user_folder::documents() + "\\Fuelman\\";

		// create the path
		if (!liblec::leccore::file::create_directory(database_directory, error))
			return 1;

		// create a database connection
		database::connection connection{ "sqlcipher", database_directory + "fuelman.db", "pass@123" };

		// create application state object
		state app_state(connection);

		// Creating main window.
		main_window main_wind(appname, app_state);

		if (!app_state.get_db().connect(error))
			return 1;

		if (!main_wind.create(error))
			main_wind.message("Error: " + error);

		restart = main_wind.restart_now();
	} while (restart);

	std::string error;

	return 0;
}
