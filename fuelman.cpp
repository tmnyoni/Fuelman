#include <iostream>
#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/image_view.h>

#include "dashboard.h"

#pragma comment(linker, "/ENTRY:mainCRTStartup")

using namespace liblec::lecui;

class login : public form{
	const float margin_ = 10.f;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_management page_man{ *this };

	bool is_logged_in_;
	const std::string username_ = "us";
	const std::string password_ = "pass";

	bool on_initialize(std::string& error) override {
		controls_.minimize(false);
		controls_.resize(false);
		appearance_.theme(themes::light);
		dims_.size({ 280, 320 });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = page_man.add("login_page");

		widgets::image_view avatar(page, "avatar");
		avatar().rect.size(130, 130);
		avatar().rect.place({ margin_, page.size().width - margin_, margin_, margin_ + (dims_.size().height / 2.f) }, 50.f, 0.f);
		avatar().quality = image_quality::high;
		avatar().file = "assets\\pump.png";
		avatar().border = 1;
		avatar().color_border = { 2,3,4 };

		widgets::text_field username(page, "username");
		username().rect.snap_to(avatar().rect, rect::snap_type::bottom, 3.f * margin_);
		username().events().action = [&]() { on_login(); };

		widgets::password_field password(page, "password");
		password().rect.snap_to(username().rect, rect::snap_type::bottom, margin_);
		password().events().action = [&]() { on_login(); };

		widgets::button signin(page, "signin");
		signin().rect.snap_to(password().rect, rect::snap_type::bottom, 1.5f * margin_);
		signin().text = "sign in";
		signin().events().action = [this]() {  on_login(); };

		page_man.show("login_page");
		return true;
	}

	void on_login() {
		try
		{
			auto& username = widgets::text_field::specs(*this, "login_page/username").text;
			auto& password = widgets::password_field::specs(*this, "login_page/password").text;

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
	login(const std::string& caption, bool& is_logged_in):
		form(caption), is_logged_in_(is_logged_in) {}

};

int main(){
	//bool is_logged_in = false;
	//login login_window("FuelMan", is_logged_in);

	//// Todo: fix the is_login issue.
	std::string error;
	//if (login_window.show(error))
	//	if (!is_logged_in) {
	//		login_window.message("signin successful");
			dashboard dashboard_page("FuelMan");
			dashboard_page.show(error);
	//	}
	//	else
	//		login_window.message("Error: " + error);
	return 0;
}                                                                                                    