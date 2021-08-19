/*
** MIT License
**
** Copyright(c) 2021 Tawanda M. Nynoi
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

//#pragma once
//#include <map>
//#include <vector>
//#include <liblec/lecui/appearance.h>
//#include <liblec/lecui/controls.h>
//
//#include <liblec/lecui/containers/page.h>
//#include <liblec/lecui/containers/pane.h>
//#include <liblec/lecui/widgets/button.h>
//#include <liblec/lecui/widgets/label.h>
//#include <liblec/lecui/widgets/text_field.h>
//#include <liblec/lecui/widgets/password_field.h>
//#include <liblec/lecui/widgets/table_view.h>
//#include <liblec/lecui/widgets/line.h>
//#include <liblec/lecui/widgets/icon.h>
//#include <liblec/lecui/widgets/rectangle.h>
//#include <liblec/lecui/widgets/image_view.h>
//#include <liblec/lecui/containers/tab_pane.h>
//
//class login : public form {
//	const float _margin = 10.f;
//
//	appearance _appearance{ *this };
//	controls _controls{ *this };
//	dimensions _dimensions{ *this };
//	page_manager _page_manager{ *this };
//
//	bool is_logged_in_;
//	const std::string username_ = "us";
//	const std::string password_ = "pass";
//
//	bool on_initialize(std::string& error) override {
//		_controls.allow_minimize(false);
//		_controls.allow_resize(false);
//		_appearance.theme(themes::light);
//		_dimensions.set_size({ 280, 320 });
//		return true;
//	}
//
//	bool on_layout(std::string& error) override {
//		auto& page = _page_manager.add("login_page");
//
//		widgets::image_view_builder avatar(page, "avatar");
//		avatar()
//			.file("assets\\pump.png")
//			.quality(image_quality::high)
//			.rect().size(130, 130)
//			.place({ _margin, page.size().width - _margin, _margin, _margin + (_dimensions.get_size().height / 2.f) }, 50.f, 0.f);
//
//		widgets::text_field_builder username(page, "username");
//		username().rect().snap_to(avatar().rect(), rect::snap_type::bottom, 3.f * _margin);
//		username().events().action = [&]() { on_login(); };
//
//		widgets::password_field_builder password(page, "password");
//		password().rect().snap_to(username().rect(), rect::snap_type::bottom, _margin);
//		password().events().action = [&]() { on_login(); };
//
//		widgets::button_builder signin(page, "signin");
//		signin()
//			.text("sign in")
//			.rect().snap_to(password().rect(), rect::snap_type::bottom, 1.5f * _margin);
//		signin().events().action = [this]() {  on_login(); };
//
//		_page_manager.show("login_page");
//		return true;
//	}
//
//	void on_login() {
//		try
//		{
//			auto& username = get_text_field_specs("login_page/username").text();
//			auto& password = get_password_field_specs("login_page/password").text();
//
//			if (username.empty() || password.empty())
//				return;
//
//			if (username == username_ && password == password_) {
//				is_logged_in_ = true;
//				close();
//			}
//			else
//				message("invalid username or password");
//		}
//		catch (const std::exception& ex) {
//			message("Error: " + std::string(ex.what()));
//		}
//	}
//public:
//	login(const std::string& caption, bool& is_logged_in) :
//		form(caption), is_logged_in_(is_logged_in) {}
//
//};