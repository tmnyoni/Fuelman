#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/image_view.h>

#include "gui/main_ui.h"
#include "appstate/app_state.h"
#include "version_info/version_info.h"

#pragma comment(linker, "/ENTRY:mainCRTStartup")
#include <vector>


using namespace liblec::lecui;

int main() {
	std::string error;
	state app_state;

	// Creating main window.
	main_window main_wind(appname, app_state);

	if (!app_state.get_db().connect(error))
		return 1;

	
	if (!main_wind.show(error))
		main_wind.message("Error: " + error);


	return 0;
}