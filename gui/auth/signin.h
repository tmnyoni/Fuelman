#pragma once
#include <map>
#include <vector>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/controls.h>

#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/containers/pane.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/table_view.h>
#include <liblec/lecui/widgets/line.h>
#include <liblec/lecui/widgets/icon.h>
#include <liblec/lecui/widgets/rectangle.h>
#include <liblec/lecui/widgets/image_view.h>
#include <liblec/lecui/containers/tab_pane.h>

class login : public form {
	const float margin_ = 10.f;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_manager page_man{ *this };

	bool is_logged_in_;
	const std::string username_ = "us";
	const std::string password_ = "pass";

	bool on_initialize(std::string& error) override {
		controls_.allow_minimize(false);
		controls_.allow_resize(false);
		appearance_.theme(themes::light);
		dims_.set_size({ 280, 320 });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = page_man.add("login_page");

		widgets::image_view_builder avatar(page, "avatar");
		avatar()
			.file("assets\\pump.png")
			.quality(image_quality::high)
			.rect().size(130, 130)
			.place({ margin_, page.size().width - margin_, margin_, margin_ + (dims_.get_size().height / 2.f) }, 50.f, 0.f);

		widgets::text_field_builder username(page, "username");
		username().rect().snap_to(avatar().rect(), rect::snap_type::bottom, 3.f * margin_);
		username().events().action = [&]() { on_login(); };

		widgets::password_field_builder password(page, "password");
		password().rect().snap_to(username().rect(), rect::snap_type::bottom, margin_);
		password().events().action = [&]() { on_login(); };

		widgets::button_builder signin(page, "signin");
		signin()
			.text("sign in")
			.rect().snap_to(password().rect(), rect::snap_type::bottom, 1.5f * margin_);
		signin().events().action = [this]() {  on_login(); };

		page_man.show("login_page");
		return true;
	}

	void on_login() {
		try
		{
			auto& username = get_text_field_specs("login_page/username").text();
			auto& password = get_password_field_specs("login_page/password").text();

			if (username.empty() || password.empty())
				return;

			if (username == username_ && password == password_) {
				is_logged_in_ = true;
				close();
			}
			else
				message("invalid username or password");
		}
		catch (const std::exception& ex) {
			message("Error: " + std::string(ex.what()));
		}
	}
public:
	login(const std::string& caption, bool& is_logged_in) :
		form(caption), is_logged_in_(is_logged_in) {}

};