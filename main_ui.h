#pragma once
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>

#include "appstate/app_state.h"

using namespace liblec::lecui;

class dashboard : public form {
	state& state_;
	const float margin_ = 10.f;
	const std::string main_page_name_ = "fuelman";

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_manager page_man_{ *this };
	widget_manager widget_man_{ *this };

	const color caption_color_ = rgba(135, 133, 133, 255);
	const color transparent = rgba(255, 255, 255, 0);

	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

	bool dashboard_handler();
	bool coupon_handler();
	bool report_handler();

	bool on_select_coupon(const std::vector<table_row>& rows);
	bool on_dispatch_coupon();
	bool on_add_coupons(std::string& error);
	bool on_edit_coupons();
	bool on_delete_coupon();

	static color rgba(const unsigned short& r,
		const unsigned short& g,
		const unsigned short& b,
		const unsigned short& a);
public:
	dashboard(const std::string& caption, state& app_state_) :
		form(caption), state_(app_state_) {}

};
