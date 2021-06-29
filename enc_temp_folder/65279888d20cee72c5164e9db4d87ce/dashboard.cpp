#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
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

#include "dashboard.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

bool dashboard::on_initialize(std::string& error) {
	controls_.allow_minimize(true);
	controls_.allow_resize(false);
	appearance_.theme(themes::light);
	dims_.set_size({ 800, 600 });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man_.add(main_page_name_);

	containers::pane_builder sidebar(page, "sidebar");
	sidebar()
		.border(1.f)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 0.f, 0.f, 0.f, 100.f })
		.rect().set(margin_, 0.f, 200.f, page.size().height - 6 * margin_);

	// Home widgets.
	widgets::rectangle_builder home(sidebar.get(), "test");
	home()
		.border(0)
		.color_fill({ 255, 255, 255, 0 })
		.rect().size({ sidebar().rect().width() - (margin_ * 2.f), 35 })
		.place(
			{
				0,
				sidebar().rect().width() - margin_,
				margin_,
				margin_ * 2.f
			}, 0.f, 0.f
		);
	home().events().action = [&]() { dashboard_handler(); };

	widgets::image_view_builder dasboard_icon(sidebar.get(), "icon");
	dasboard_icon()
		.border(1)
		.file("assets\\dashboard.png")
		.rect().size({ 30, 30 })
		.place(
			{
				home.specs().rect().left() + 2.5f,
				40,
				home.specs().rect().top() + 2.5f,
				home.specs().rect().top() + 2.5f + (margin_ * 2.5f)
			}, 50.f, 0.f
		);


	widgets::label_builder dashboard_label(sidebar.get(), "dashboard_label");
	dashboard_label()
		.text("Dashboard")
		.center_v(true)
		.rect().snap_to(dasboard_icon().rect(), snap_type::right, margin_);


	// Coupons widgets
	widgets::rectangle_builder coupons(sidebar.get(), "coupons");
	coupons()
		.border(0)
		.color_fill(rgba(255, 255, 255, 0))
		.rect().size({ sidebar().rect().width() - (margin_ * 2.f), 35 })
		.snap_to(home().rect(), snap_type::bottom, margin_);
	coupons().events().action = [&]() { coupon_handler(); };


	widgets::image_view_builder coupons_icon(sidebar.get(), "coupons_icon");
	coupons_icon()
		.border(1)
		.file("assets\\pump.png")
		.rect().size({ 30, 30 })
		.place(
			{
				coupons.specs().rect().left() + 2.5f,
				40,
				coupons.specs().rect().top() + 2.5f,
				coupons.specs().rect().top() + 2.5f + (margin_ * 2.5f)
			}, 50.f, 0.f
		);


	widgets::label_builder coupons_label(sidebar.get(), "coupon_label");
	coupons_label()
		.text("Coupons")
		.center_v(true)
		.rect().snap_to(coupons_icon().rect(),snap_type::right, 10.f);


	// reports widgets
	widgets::rectangle_builder reports(sidebar.get(), "reports");
	reports()
		.border(0)
		.color_fill(rgba(255, 255, 255, 0))
		.rect().size({ sidebar().rect().width() - (margin_ * 2.f), 35 })
		.snap_to(coupons().rect(),snap_type::bottom, margin_);
	reports().events().action = [&]() { coupon_handler(); };

	widgets::image_view_builder reports_icon(sidebar.get(), "reports_icon");
	reports_icon()
		.border(1)
		.file("assets\\reports.png")
		.rect().size({ 30, 30 })
		.place(
			{
				reports.specs().rect().left() + 2.5f,
				40,
				reports.specs().rect().top() + 2.5f,
				reports.specs().rect().top() + 2.5f + (margin_ * 2.5f)
			}, 50.f, 0.f);

	widgets::label_builder reports_label(sidebar.get(), "reports_label");
	reports_label()
		.text("Reports")
		.center_v(true)
		.rect().snap_to(reports_icon().rect(),snap_type::right, margin_);


	// Settings widgets
	widgets::rectangle_builder settings(sidebar.get(), "settings");
	settings()
		.border(0)
		.color_fill({ 255, 255, 255, 0 })
		.on_resize({ 0.f, 100.f, 0.f, 0.f })
		.rect().size({ sidebar().rect().width() - (margin_ * 2.f), 35 })
		.place(
			{
				0,
				sidebar().rect().width() - margin_,
				margin_ + (sidebar().rect().height() - 70),
				(margin_ * 2.f) + (sidebar().rect().height() - 70)
			}, 0.f, 0.f
		);
	settings().events().action = [&]() {};

	widgets::image_view_builder settings_icon(sidebar.get(), "settings_icon");
	settings_icon()
		.border(1)
		.file("assets\\settings.png")
		.on_resize({ 0.f, 100.f, 0.f, 0.f })
		.rect().size({ 30, 30 })
		.place(
			{ settings.specs().rect().left() + 2.5f
			, 40
			, settings.specs().rect().top() + 2.5f
			, settings.specs().rect().top() + 2.5f + (margin_ * 2.5f)
			}, 50.f, 0.f);

	widgets::label_builder settings_label(sidebar.get(), "settings_label");
	settings_label()
		.text("Settings")
		.center_v(true)
		.on_resize({ 0.f, 100.f, 0.f, 0.f })
		.rect().snap_to(settings_icon().rect(),snap_type::right, margin_);

	//render dashboard widgets.
	dashboard_handler();

	page_man_.show(main_page_name_);
	return true;
}

bool dashboard::dashboard_handler() {
	widget_man_.close(main_page_name_ + "/coupon_content");

	// Dashboard Panel.
	rect sidebar_rect;
	try
	{
		sidebar_rect = containers::pane_builder::specs(*this, (main_page_name_ + "/sidebar")).rect();
	}
	catch (const std::exception& ex)
	{
		message("Error: " + std::string(ex.what()));
	}

	containers::pane_builder dashboard_content(page_man_.get(*this, main_page_name_), "dashboard_content");
	dashboard_content()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 250, 0))
		.on_resize({ 0.f, 0.f, 100.f, 100.f })
		.rect().size({ 560.f, 510.f })
		.snap_to(sidebar_rect,snap_type::right, margin_);


	widgets::label_builder dashboard_title(dashboard_content.get());
	dashboard_title()
		.text("Overview")
		.font_size(12.f)
		.center_h(true)
		.on_resize({ 50.f, 0.f, 0.f, 0.f })
		.rect().size({ 80, 20 })
		.place(
			{ dashboard_content().rect().width() / 2.f - 40
			, dashboard_content().rect().width() / 2.f + 40
			, dashboard_content().rect().top() + 20.f
			, dashboard_content().rect().top() + 40.f
			}, 0.f, 0.f);

	containers::pane_builder monthly_content(dashboard_content.get());
	monthly_content()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ -50.f, 0.f, 50.f, 0.f })
		.rect().set(
			margin_,
			dashboard_title().rect().bottom() + margin_,
			dashboard_content().rect().width() / 2.f - (margin_ * 2.5f),
			dashboard_content().rect().height() / 2.f - margin_
		);

	containers::pane_builder content_summary(dashboard_content.get());
	content_summary()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 50.f, 0.f, 50.f, 0.f })
		.rect().set
		(monthly_content().rect().right() + margin_
			, dashboard_title().rect().bottom() + margin_
			, dashboard_content().rect().width() / 2.f - (margin_ * 2.5f)
			, dashboard_content().rect().height() / 2.f - margin_
		);

	return true;
}

bool dashboard::coupon_handler()
{
	widget_man_.close(main_page_name_ + "/dashboard_content");

	rect sidebar_rect;
	try
	{
		sidebar_rect = containers::pane_builder::specs(*this, (main_page_name_ + "/sidebar")).rect();
	}
	catch (const std::exception& ex)
	{
		message("Error: " + std::string(ex.what()));
	}

	containers::pane_builder coupon_content(page_man_.get(*this, main_page_name_), "coupon_content");
	coupon_content().border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 0.f, 0.f, 100.f, 100.f })
		.rect().size({ 560.f, 510.f })
		.snap_to(sidebar_rect,snap_type::right, margin_);

	std::vector<table_column> coupons_table_cols =
	{
		{ "#", 50},
		{ "Date", 100},
		{"Issued to", 100}, {"Serial Number", 100}
	};

	std::vector<std::map<std::string, std::string>> coupons_data = {
		{ {"#", "1"}, {"Date", "10-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "2"}, {"Date", "10-June-20"}, {"Issued to", "Accounts"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "3"}, {"Date", "10-June-20"}, {"Issued to", "Admini"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "4"}, {"Date", "10-June-20"}, {"Issued to", "ICT"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "5"}, {"Date", "10-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "6"}, {"Date", "10-June-20"}, {"Issued to", "Ambulance"}, {"Serial Number", "fhdskfhasdfhasi34"} }
	};

	widgets::table_view_builder coupons_table(coupon_content.get(), "coupons_table");
	coupons_table()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ -50.f, 0.f, 50.f, 0.f })
		.columns(coupons_table_cols)
		.data(coupons_data)
		.rect().set(
			margin_,
			margin_,
			coupon_content().rect().width() / 2.f - (margin_ * 2.5f),
			coupon_content().rect().height() - (3.f * margin_)
		);
	coupons_table().events().action = [&]() { on_select_coupon(); };

	containers::pane_builder coupon_details_pane(coupon_content.get(), "coupon_details_pane");
	coupon_details_pane()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 50.f, 0.f, 50.f, 0.f })
		.rect().set(
			coupons_table().rect().right() + margin_,
			coupons_table().rect().top(),
			coupon_content().rect().width() / 2.f - (margin_ * 2.5f),
			coupon_content().rect().height() - (3.f * margin_)
		);


	//widgets on the pane.
	widgets::label_builder date_label(coupon_details_pane.get());
	date_label()
		.text("Date")
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.place(
			{
				margin_,
				200,
				margin_,
				20
			}, 0.f, 0.f);

	widgets::label_builder date_details(coupon_details_pane.get(), "date_details");
	date_details()
		.text("28-June-2021")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(date_label().rect(),snap_type::bottom, 2.f);

	widgets::label_builder coupon_serialno_caption(coupon_details_pane.get());
	coupon_serialno_caption()
		.text("Serial Number")
		.color_text(caption_color_)
		.color_fill({ 32, 34, 244, 0 })
		.rect().size({ 200, 20 })
		.snap_to(date_details().rect(),snap_type::bottom, margin_);

	widgets::label_builder coupon_serialno_details(coupon_details_pane.get(), "coupon_serialno_details");
	coupon_serialno_details()
		.text("403859177-4038591780")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(coupon_serialno_caption().rect(),snap_type::bottom, 2.f);

	widgets::label_builder quantity_issued_caption(coupon_details_pane.get());
	quantity_issued_caption()
		.text("Quantity issued")
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(coupon_serialno_details().rect(),snap_type::bottom, margin_);

	widgets::label_builder quantity_issued_details(coupon_details_pane.get(), "quantity_issued_details");
	quantity_issued_details()
		.text(std::to_string(80) + " Litres")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(quantity_issued_caption().rect(),snap_type::bottom, 2.f);

	widgets::label_builder issuedto_caption(coupon_details_pane.get());
	issuedto_caption()
		.text("Issued to")
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(quantity_issued_details().rect(),snap_type::bottom, margin_);

	widgets::label_builder issuedto_details(coupon_details_pane.get(), "issuedto_details");
	issuedto_details()
		.text("Transport")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(issuedto_caption().rect(),snap_type::bottom, 2.f);

	widgets::label_builder coupon_recvby_caption(coupon_details_pane.get());
	coupon_recvby_caption()
		.text("Coupon received by")
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(issuedto_details().rect(),snap_type::bottom, margin_);

	widgets::label_builder coupon_recvby_details(coupon_details_pane.get(), "coupon_recvby_details");
	coupon_recvby_details()
		.text("Tawanda M. Nyoni")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(coupon_recvby_caption().rect(),snap_type::bottom, 2.f);

	widgets::label_builder comments_caption(coupon_details_pane.get());
	comments_caption()
		.text("Comments")
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(coupon_recvby_details().rect(),snap_type::bottom, margin_);

	widgets::label_builder comments_details(coupon_details_pane.get(), "comments_details");
	comments_details()
		.text("It was an emergency")
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(comments_caption().rect(),snap_type::bottom, 2.f);

	return true;
}

bool dashboard::on_select_coupon()
{
	try
	{
		auto& data = widgets::table_view_builder::specs(*this, main_page_name_ + "/coupon_content/coupons_table").data();
		return true;
	}
	catch (const std::exception& ex)
	{
		message("Error: " + std::string(ex.what()));
		return false;
	}
}

color dashboard::rgba(const unsigned short& r,
	const unsigned short& g,
	const unsigned short& b,
	const unsigned short& a)
{
	return color({ r, g, b, a });
}

