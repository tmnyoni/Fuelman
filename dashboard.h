#pragma once
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>

using namespace liblec::lecui;

class dashboard : public form {
	const float margin_ = 10.f;
	const std::string main_page_name_ = "fuelman";

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_management page_man_{ *this };
	widget_management widget_man_{ *this };

	const color caption_color_ = rgba(135, 133, 133, 255);
	const color transparent = rgba(255, 255, 255, 0);


	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

	bool dashboard_handler();
	bool coupon_handler();

	bool on_select_coupon(const std::vector<std::map<std::string, std::string>>& rows);


	static color rgba(const unsigned short& r,
		const unsigned short& g,
		const unsigned short& b,
		const unsigned short& a);
public:
	dashboard(const std::string& caption) :
		form(caption) {}

};
