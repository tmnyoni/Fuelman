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

#pragma once

// include std headers.
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>

#include <liblec/lecui/utilities/splash.h>

// include leccore
#include <liblec/leccore/settings.h>

// include local headers.
#include "../appstate/app_state.h"


using namespace liblec;

class main_window : public lecui::form {

	// app attributes.
	lecui::appearance _appearance{ *this };
	lecui::controls _controls{ *this };
	lecui::dimensions _dimensions{ *this };
	lecui::page_manager _page_manager{ *this };
	lecui::widget_manager _widget_manager{ *this };
	lecui::splash  _splash_screen{ *this };

	// window attributes.
	const float _margin = 10.f;
	const lecui::size _window_size = lecui::size::size().width(800.f).height(700.f);
	const std::string _page_name = "fuelman";
	const std::string _main_tab_name = "main tabs";
	const std::string _main_tab_pane_path =_page_name + "/" + _main_tab_name; 

	const lecui::color _caption_color = lecui::color().red(135).green(133).blue(133);

	// form overrides.
	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

	// class methods.
	bool on_dispatch_coupon(const std::vector<lecui::table_row>& rows, std::string& error);
	bool on_add_coupons(std::string& error);
	bool on_delete_coupon(std::string& error);

	// application state.
	state& _state;

	// settings
	bool _setting_darktheme = true;
	const bool _cleanup_mode;

	leccore::settings& _settings; // app settings.
	leccore::registry_settings _registry_settings{ leccore::registry::scope::current_user };



public:
	main_window(const std::string& caption,
		state& app_state
	);
};
