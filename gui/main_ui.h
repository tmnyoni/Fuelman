#pragma once
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>

#include "../appstate/app_state.h"

using namespace liblec::lecui;

class dashboard : public form {
	state& _state;
	const float _margin = 10.f;
	const std::string _page_name = "fuelman";

	appearance _appearance{ *this };
	controls _controls{ *this };
	dimensions _dims{ *this };
	page_manager _page_man{ *this };
	widget_manager _widget_man{ *this };

	const color _caption_color = color().red(135).green(133).blue(133);

	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

	bool on_select_coupon(const std::vector<table_row>& rows);
	bool on_dispatch_coupon(const std::vector<table_row>& rows, std::string& error);
	bool on_add_coupons(std::string& error);
	bool on_delete_coupon();

	static color rgba(const unsigned short& r,
		const unsigned short& g,
		const unsigned short& b,
		const unsigned short& a);
public:
	dashboard(const std::string& caption, state& app_state) :
		form(caption), _state(app_state) {}

};
