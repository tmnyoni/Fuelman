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

using namespace liblec::lecui;

bool dashboard::on_initialize(std::string& error) {
	controls_.minimize(false);
	controls_.resize(false);
	appearance_.theme(themes::light);
	dims_.size({ 800, 600 });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man.add("dashboard_page");

	widgets::button home(page, "home");
	home().rect.size(130, 130);
	home().rect.place({ margin_, page.size().width - margin_, margin_, margin_ + (dims_.size().height / 2.f) }, 50.f, 0.f);
	home().text = "Dashboard";

	widgets::button coupons(page, "coupons");
	coupons().rect.snap_to(home().rect, rect::snap_type::right, margin_);
	coupons().text = "sign in";
	coupons().events().action = [this]() {  };

	page_man.show("dashboard_page");
	return true;
}

