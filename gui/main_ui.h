#pragma once

// include std headers.
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>

// include local headers.
#include "../appstate/app_state.h"

using namespace liblec::lecui;

class main_window : public form {

	// form attributes.
	appearance _appearance{ *this };
	controls _controls{ *this };
	dimensions _dims{ *this };
	page_manager _page_man{ *this };
	widget_manager _widget_man{ *this };

	const float _margin = 10.f;
	const std::string _page_name = "fuelman";
	const std::string _main_tab_name = "main tabs";
	const std::string main_tab_pane_path () 
		{ return _page_name + "/" + _main_tab_name; }

	// more definitions.
	const color _caption_color = color().red(135).green(133).blue(133);

	// form overrides.
	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

	// class methods.
	bool on_dispatch_coupon(const std::vector<table_row>& rows, std::string& error);
	bool on_add_coupons(std::string& error);
	bool on_delete_coupon(std::string& error);

	// application state.
	state& _state;

public:
	main_window(const std::string& caption,
		state& app_state
	) :
		form(caption), 
		_state(app_state) {}
};
