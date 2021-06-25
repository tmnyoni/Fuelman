#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/image_view.h>

#include "menus.h"

using namespace liblec::lecui;

bool menus::on_initialize(std::string& error) {
	controls_.minimize(false);
	controls_.resize(false);
	appearance_.theme(themes::light);
	dims_.size({ 800, 600 });
	return true;
}

bool menus::on_layout(std::string& error) {
	auto& page = page_man.add("login_page");

	widgets::image_view avatar(page, "avatar");
	avatar().rect.size(130, 130);
	avatar().rect.place({ margin_, page.size().width - margin_, margin_, margin_ + (dims_.size().height / 2.f) }, 50.f, 0.f);
	avatar().quality = image_quality::high;
	avatar().file = "assets\\pump.png";

	widgets::text_field username(page, "username");
	username().rect.snap_to(avatar().rect, rect::snap_type::bottom, 3.f * margin_);

	widgets::password_field password(page, "password");
	password().rect.snap_to(username().rect, rect::snap_type::bottom, margin_);

	widgets::button signin(page, "signin");
	signin().rect.snap_to(password().rect, rect::snap_type::bottom, 1.5f * margin_);
	signin().text = "sign in";
	signin().border = 0;

	page_man.show("login_page");
	return true;
}

