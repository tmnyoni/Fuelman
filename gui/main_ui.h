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
** IMPLIED, INCLUDING BUT NOT LIMITED TO x`THE WARRANTIES OF MERCHANTABILITY,
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
#include <liblec/leccore/web_update.h>
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
	bool on_select_theme(const std::string& selected_theme, std::string& error);

	// application state.
	state& _state;

	// settings
	const std::string _instance_guid = "{C053EA26-EC40-4E23-8C9E-8174C7D3320F}";
	const std::string _install_guid_32 = "{C6B21117-3D7D-4487-A6DE-DD4288385D1D}";
	const std::string _install_guid_64 = "{F9394A11-8440-42E3-A725-1478261700A4}";
	const std::string _update_xml_url = "https://raw.githubusercontent.com/tmnyoni/fuelman/master/latest_update.xml";

	bool _setting_darktheme = false;
	const bool _cleanup_mode;
	const bool _update_mode;
	const bool _recent_update_mode;

	std::string _update_directory;

	leccore::settings& _settings; // app settings.
	leccore::registry_settings _registry_settings{ leccore::registry::scope::current_user };

	leccore::check_update _check_update{ _update_xml_url };
	leccore::check_update::update_info _update_info;
	leccore::download_update download_update_;

	std::string _install_location_32, _install_location_64;
	std::string _updates_directory;

	bool _autocheck_updates_setting = true;
	bool _autodownload_updates_setting = false;
	bool _update_check_manaully_initiated = false;

	bool _restart_now = false;
public:
	bool on_restart_now();

	main_window(const std::string& caption,
		state& app_state
	);
};
