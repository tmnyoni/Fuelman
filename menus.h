#pragma once
#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/image_view.h>


using namespace liblec::lecui;

class menus : public form {
	const float margin_ = 10.f;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_management page_man{ *this };

	bool is_logged_in_;
	const std::string username_ = "username";
	const std::string password_ = "password";

	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;

public:
	menus(const std::string& caption) :
		form(caption) {}

};
