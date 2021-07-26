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

#pragma comment(linker, "/ENTRY:mainCRTStartup")

using namespace liblec::lecui;

int main(){
	//bool is_logged_in = false;
	//login login_window("FuelMan", is_logged_in);

	//// Todo: fix the is_login issue.
	std::string error;
	state _state;

	// Connect to the database.
	if (!_state.get_db().connect(error))
		return 1;
	
	//if (login_window.show(error))
	//	if (!is_logged_in) {
	//		login_window.message("signin successful");
			dashboard dashboard_page("FuelMan", _state);
			if (!dashboard_page.show(error))
				dashboard_page.message("Error: " + error);

	//	}
	//	else
	//		login_window.message("Error: " + error);
	return 0;
}                                                                                                    