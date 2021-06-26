#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/containers/pane.h>
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
	auto& page = page_man.add("dashboard");

	containers::pane panel(page, "sidebar");
	panel().rect.set(margin_, margin_, 200.f, page.size().height - 6 * margin_);
	panel().border = 1;

	widgets::button home(page, "home");
	home().rect.size(80, 25);
	home().rect.place({ margin_, page.size().width - margin_ , margin_, margin_ + (dims_.size().height / 2.f) }, 40.f, 0.f);
	home().text = "Dashboard";
	home().events().action = [this]() {};

	widgets::button coupons(page, "coupons");
	coupons().rect.snap_to(home().rect, rect::snap_type::right, margin_);
	coupons().text = "Coupons";
	coupons().events().action = [this]() {  };

	page_man.show("dashboard");
	return true;
}

