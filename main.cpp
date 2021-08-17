#include "gui/main_ui.h"
#include "appstate/app_state.h"
#include "version_info/version_info.h"

#pragma comment(linker, "/ENTRY:mainCRTStartup")

using namespace liblec::lecui;

int main() {
	std::string error;
	state app_state;

	// Creating main window.
	main_window main_wind(appname, app_state);

	if (!app_state.get_db().connect(error))
		return 1;

	
	if (!main_wind.create(error))
		main_wind.message("Error: " + error);


	return 0;
}