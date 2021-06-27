#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/containers/pane.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/line.h>
#include <liblec/lecui/widgets/icon.h>
#include <liblec/lecui/widgets/rectangle.h>

#include "dashboard.h"

using namespace liblec::lecui;

bool dashboard::on_initialize(std::string& error) {
	controls_.minimize(true);
	controls_.resize(true);
	appearance_.theme(themes::light);
	dims_.size({ 800, 600 });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man.add("dashboard");

	containers::pane sidebar(page, "sidebar");
	sidebar().rect.set(margin_, margin_, 200.f, page.size().height - 6 * margin_);
	sidebar().border = 1;
	sidebar().corner_radius_x = 0;
	sidebar().corner_radius_y = 0;
	sidebar().color_fill.alpha = 0;
	sidebar().on_resize = { 0.f, 0.f, 0.f, 100.f };

	widgets::line vertical_line(page, "vertical_line");
	vertical_line().points = { {sidebar().rect.right + 1.f, 30}, {sidebar().rect.right + 1.f, sidebar().rect.bottom } };
	vertical_line().on_resize = { 0.f, 0.f, 0.f, 100.f };


	// Home widgets.
	widgets::rectangle home(sidebar.get(), "test");
	home().rect.size({ sidebar().rect.width() - (margin_ * 2.f), 35 });
	home().rect.place({ 0, sidebar().rect.width() - margin_, margin_, margin_ * 2.f }, 0.f, 0.f);
	home().color_fill.alpha = 0;
	home().border = 0;
	home().events().action = [&]() {};

	widgets::image_view dasboard_icon(sidebar.get(), "icon");
	dasboard_icon().rect.size({ 30, 30 });
	dasboard_icon().rect.place(
		{ home.specs().rect.left + 2.5f
		, 40
		, home.specs().rect.top + 2.5f
		, home.specs().rect.top + 2.5f + (margin_ * 2.5f)
		}, 50.f, 0.f);
	dasboard_icon().file = "assets\\dashboard.png";
	dasboard_icon().border = 1;
	
	widgets::label dashboard_label(sidebar.get(), "dashboard_label");
	dashboard_label().text = "Dashboard";
	dashboard_label().rect.snap_to(dasboard_icon().rect, rect::snap_type::right, 10.f);
	dashboard_label().center_v = true;


	// Coupons widgets
	widgets::rectangle coupons(sidebar.get(), "coupons");
	coupons().rect.size({ sidebar().rect.width() - (margin_ * 2.f), 35 });
	coupons().rect.snap_to(home().rect, rect::snap_type::bottom, margin_);
	coupons().color_fill.alpha = 0;
	coupons().border = 0;
	coupons().events().action = [&]() {};

	widgets::image_view coupons_icon(sidebar.get(), "coupons_icon");
	coupons_icon().rect.size({ 30, 30 });
	coupons_icon().rect.place(
		{ coupons.specs().rect.left + 2.5f
		, 40
		, coupons.specs().rect.top + 2.5f
		, coupons.specs().rect.top + 2.5f + (margin_ * 2.5f)
		}, 50.f, 0.f);
	coupons_icon().file = "assets\\pump.png";
	coupons_icon().border = 1;

	widgets::label coupons_label(sidebar.get(), "coupon_label");
	coupons_label().text = "Coupons";
	coupons_label().rect.snap_to(coupons_icon().rect, rect::snap_type::right, 10.f);
	coupons_label().center_v = true;

	// Settings widgets
	widgets::rectangle settings(sidebar.get(), "settings");
	settings().rect.size({ sidebar().rect.width() - (margin_ * 2.f), 35 });
	settings().color_fill.alpha = 0;
	settings().border = 0;
	settings().on_resize = { 0.f, 100.f, 0.f, 0.f };
	settings().rect.place(
		{ 0, sidebar().rect.width() - margin_, margin_ + (sidebar().rect.height() - 70), (margin_ * 2.f) + (sidebar().rect.height() - 70) }, 0.f, 0.f);
	settings().events().action = [&]() {};

	widgets::image_view settings_icon(sidebar.get(), "settings_icon");
	settings_icon().rect.size({ 30, 30 });
	settings_icon().rect.place(
		{ settings.specs().rect.left + 2.5f
		, 40
		, settings.specs().rect.top + 2.5f
		, settings.specs().rect.top + 2.5f + (margin_ * 2.5f)
		}, 50.f, 0.f);
	settings_icon().file = "assets\\settings.png";
	settings_icon().on_resize = { 0.f, 100.f, 0.f, 0.f };
	settings_icon().border = 1;

	widgets::label settings_label(sidebar.get(), "settings_label");
	settings_label().text = "Settings";
	settings_label().rect.snap_to(settings_icon().rect, rect::snap_type::right, 10.f);
	settings_label().on_resize = { 0.f, 100.f, 0.f, 0.f };
	settings_label().center_v = true;

	// Dashboard Panel.
	containers::pane dashboard_content(page, "dashboard_content");
	dashboard_content().rect.set
		( sidebar().rect.right + margin_
		, margin_
		, page.size().width - (sidebar().rect.right + (2*margin_))
		, page.size().height - 6 * margin_
		);
	dashboard_content().border = 1;
	dashboard_content().corner_radius_x = 0;
	dashboard_content().corner_radius_y = 0;
	dashboard_content().color_fill.alpha = 0;
	dashboard_content().on_resize = { 0.f, 0.f, 100.f, 100.f };

	widgets::label dashboard_title(dashboard_content.get());
	dashboard_title().text = "Overview";
	dashboard_title().font_size = 12.f;
	dashboard_title().center_h = true;
	dashboard_title().on_resize = { 50.f, 0.f, 0.f, 0.f };
	dashboard_title().rect.size({ 80, 20 });
	dashboard_title().rect.place(
		{ dashboard_content().rect.width() / 2.f - 40
		, dashboard_content().rect.width() / 2.f + 40
		, 30.f
		, 60.f
		}, 0.f, 0.f);

	
	page_man.show("dashboard");
	return true;
}

