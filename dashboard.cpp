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

bool dashboard::on_initialize(std::string& error) {
	controls_.minimize(true);
	controls_.resize(false);
	appearance_.theme(themes::light);
	dims_.size({ 800, 600 });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man.add(main_page_name_);

	containers::pane sidebar(page, "sidebar");
	sidebar().rect.set(margin_, 0.f, 200.f, page.size().height - 6 * margin_);
	sidebar().border = 1;
	sidebar().corner_radius_x = 0;
	sidebar().corner_radius_y = 0;
	sidebar().color_fill.alpha = 0;
	sidebar().on_resize = { 0.f, 0.f, 0.f, 100.f };


	// Home widgets.
	widgets::rectangle home(sidebar.get(), "test");
	home().rect.size({ sidebar().rect.width() - (margin_ * 2.f), 35 });
	home().rect.place({ 0, sidebar().rect.width() - margin_, margin_, margin_ * 2.f }, 0.f, 0.f);
	home().color_fill.alpha = 0;
	home().border = 0;
	home().events().action = [&]() { dashboard_handler(*this); };

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
	coupons().events().action = [&]() { coupon_handler(*this); };

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

	// reports widgets
	widgets::rectangle reports(sidebar.get(), "reports");
	reports().rect.size({ sidebar().rect.width() - (margin_ * 2.f), 35 });
	reports().rect.snap_to(coupons().rect, rect::snap_type::bottom, margin_);
	reports().color_fill.alpha = 0;
	reports().border = 0;
	reports().events().action = [&]() { coupon_handler(*this); };

	widgets::image_view reports_icon(sidebar.get(), "reports_icon");
	reports_icon().rect.size({ 30, 30 });
	reports_icon().rect.place(
		{ reports.specs().rect.left + 2.5f
		, 40
		, reports.specs().rect.top + 2.5f
		, reports.specs().rect.top + 2.5f + (margin_ * 2.5f)
		}, 50.f, 0.f);
	reports_icon().file = "assets\\reports.png";
	reports_icon().border = 1;


	widgets::label reports_label(sidebar.get(), "reports_label");
	reports_label().text = "Reports";
	reports_label().rect.snap_to(reports_icon().rect, rect::snap_type::right, 10.f);
	reports_label().center_v = true;

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

	//render dashboard widgets.
	dashboard_handler(*this);

	page_man.show(main_page_name_);
	return true;
}

bool dashboard::dashboard_handler(const page_management& page) {
	widget_man.close(main_page_name_ + "/coupon_content");

	// Dashboard Panel.
	containers::pane dashboard_content(page.get(*this, main_page_name_), "dashboard_content");
	dashboard_content().rect.set
	(220.f
		, 0.f
		, 560.f
		, 510.f
	);
	dashboard_content().border = 1;
	dashboard_content().corner_radius_x = 0;
	dashboard_content().corner_radius_y = 0;
	dashboard_content().color_fill.alpha = 0;
	dashboard_content().on_resize = { 0.f, 0.f, 100.f, 100.f };


	widgets::label dashboard_title(dashboard_content.get(), "dashboard_title");
	dashboard_title().text = "Overview";
	dashboard_title().font_size = 12.f;
	dashboard_title().center_h = true;
	dashboard_title().on_resize = { 50.f, 0.f, 0.f, 0.f };
	dashboard_title().rect.size({ 80, 20 });
	dashboard_title().rect.place(
		{ dashboard_content().rect.width() / 2.f - 40
		, dashboard_content().rect.width() / 2.f + 40
		, dashboard_content().rect.top + 20.f
		, dashboard_content().rect.top + 40.f
		}, 0.f, 0.f);

	containers::pane monthly_content(dashboard_content.get(), "montly_content");
	monthly_content().rect.set
	(margin_
		, dashboard_title().rect.bottom + margin_
		, dashboard_content().rect.width() / 2.f - (margin_ * 2.5f)
		, dashboard_content().rect.height() / 2.f - margin_
	);
	monthly_content().border = 1;
	monthly_content().corner_radius_x = 0;
	monthly_content().corner_radius_y = 0;
	monthly_content().color_fill.alpha = 0;
	monthly_content().on_resize = { -50.f, 0.f, 50.f, 0.f };

	containers::pane content_summary(dashboard_content.get(), "summary_content");
	content_summary().rect.set
	(monthly_content().rect.right + margin_
		, dashboard_title().rect.bottom + margin_
		, dashboard_content().rect.width() / 2.f - (margin_ * 2.5)
		, dashboard_content().rect.height() / 2.f - margin_
	);
	content_summary().border = 1;
	content_summary().corner_radius_x = 0;
	content_summary().corner_radius_y = 0;
	content_summary().color_fill.alpha = 0;
	content_summary().on_resize = { 50.f, 0.f, 50.f, 0.f };
	return true;
}

bool dashboard::coupon_handler(const page_management& page)
{
	widget_man.close(main_page_name_+"/dashboard_content");


	containers::pane coupon_content(page.get(*this, main_page_name_), "coupon_content");
	coupon_content().rect.set
		( 220.f
		, 0.f
		, 560.f
		, 510.f
		);
	coupon_content().border = 1;
	coupon_content().corner_radius_x = 0;
	coupon_content().corner_radius_y = 0;
	coupon_content().color_fill.alpha = 100;
	coupon_content().on_resize = { 0.f, 0.f, 100.f, 100.f };
	coupon_content().color_fill.alpha = 0;

	widgets::table_view coupons_table(coupon_content.get(), "coupons_table");
	coupons_table().rect.set
	(margin_
		, margin_
		, coupon_content().rect.width() / 2.f - (margin_ * 2.5)
		, coupon_content().rect.height() - (3.f * margin_)
	);
	coupons_table().border = 1;
	coupons_table().corner_radius_x = 0;
	coupons_table().corner_radius_y = 0;
	coupons_table().color_fill.alpha = 0;
	coupons_table().on_resize = { -50.f, 0.f, 50.f, 0.f };
	std::vector<table_column> coupons_table_cols = { { "#", 50}, { "Date", 100}, {"Issued to", 100}, {"Serial Number", 100} };
	coupons_table().columns = coupons_table_cols;
	coupons_table().data = {
		{ {"#", "1"}, {"Date", "10-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "2"}, {"Date", "10-June-20"}, {"Issued to", "Accounts"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "3"}, {"Date", "10-June-20"}, {"Issued to", "Admini"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "4"}, {"Date", "10-June-20"}, {"Issued to", "ICT"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "5"}, {"Date", "10-June-20"}, {"Issued to", "Transport"}, {"Serial Number", "fhdskfhasdfhasi34"} },
		{ {"#", "6"}, {"Date", "10-June-20"}, {"Issued to", "Ambulance"}, {"Serial Number", "fhdskfhasdfhasi34"} }
	};

	containers::pane coupon_details_pane(coupon_content.get(), "coupon_details_pane");
	coupon_details_pane().rect.set
	(coupons_table().rect.right + margin_
		, coupons_table().rect.top 
		, coupon_content().rect.width() / 2.f - (margin_ * 2.5)
		, coupon_content().rect.height() - (3.f * margin_)
	);
	coupon_details_pane().border = 1;
	coupon_details_pane().corner_radius_x = 0;
	coupon_details_pane().corner_radius_y = 0;
	coupon_details_pane().color_fill.alpha = 0;
	coupon_details_pane().on_resize = { 50.f, 0.f, 50.f, 0.f };

	//widgets on the pane.
	widgets::label date_label(coupon_details_pane.get(), "date_label");
	date_label().text = "Date";
	date_label().color_text = caption_color_;
	date_label().color_fill = { 32, 34, 244 };
	date_label().rect.size({ 200, 20 });
	date_label().rect.place(
		{ margin_, 200, margin_, 20 }, 0.f, 0.f);

	widgets::label date_details(coupon_details_pane.get(), "date_details");
	date_details().text = "28-June-2021";
	date_details().color_fill = { 32, 34, 244 };
	date_details().rect.size({ 200, 20 });
	date_details().rect.snap_to(date_label().rect, rect::snap_type::bottom, 2.f);

	widgets::label coupon_serialno_caption(coupon_details_pane.get(), "coupon_serialno_caption");
	coupon_serialno_caption().text = "Serial Number";
	coupon_serialno_caption().color_text = caption_color_;
	coupon_serialno_caption().color_fill = { 32, 34, 244 };
	coupon_serialno_caption().rect.size({ 200, 20 });
	coupon_serialno_caption().rect.snap_to(date_details().rect, rect::snap_type::bottom, margin_);

	widgets::label coupon_serialno_details(coupon_details_pane.get(), "coupon_serialno_details");
	coupon_serialno_details().text = "403859177-4038591780";
	coupon_serialno_details().color_fill = { 32, 34, 244 };
	coupon_serialno_details().rect.size({ 200, 20 });
	coupon_serialno_details().rect.snap_to(coupon_serialno_caption().rect, rect::snap_type::bottom, 2.f);

	widgets::label quantity_issued_caption(coupon_details_pane.get(), "quantity_issued_caption");
	quantity_issued_caption().text = "Quantity issued";
	quantity_issued_caption().color_text = caption_color_;
	quantity_issued_caption().color_fill = { 32, 34, 244 };
	quantity_issued_caption().rect.size({ 200, 20 });
	quantity_issued_caption().rect.snap_to(coupon_serialno_details().rect, rect::snap_type::bottom, margin_);

	widgets::label quantity_issued_details(coupon_details_pane.get(), "quantity_issued_details");
	quantity_issued_details().text = std::to_string(80) + " Litres";
	quantity_issued_details().color_fill = { 32, 34, 244 };
	quantity_issued_details().rect.size({ 200, 20 });
	quantity_issued_details().rect.snap_to(quantity_issued_caption().rect, rect::snap_type::bottom, 2.f);

	widgets::label issuedto_caption(coupon_details_pane.get(), "issuedto_caption");
	issuedto_caption().text = "Issued to";
	issuedto_caption().color_text = caption_color_;
	issuedto_caption().color_fill = { 32, 34, 244 };
	issuedto_caption().rect.size({ 200, 20 });
	issuedto_caption().rect.snap_to(quantity_issued_details().rect, rect::snap_type::bottom, margin_);

	widgets::label issuedto_details(coupon_details_pane.get(), "issuedto_details");
	issuedto_details().text = "Transport";
	issuedto_details().color_fill = { 32, 34, 244 };
	issuedto_details().rect.size({ 200, 20 });
	issuedto_details().rect.snap_to(issuedto_caption().rect, rect::snap_type::bottom, 2.f);

	widgets::label coupon_recvby_caption(coupon_details_pane.get(), "coupon_recvby_caption");
	coupon_recvby_caption().text = "Coupon received by";
	coupon_recvby_caption().color_text = caption_color_;
	coupon_recvby_caption().color_fill = { 32, 34, 244 };
	coupon_recvby_caption().rect.size({ 200, 20 });
	coupon_recvby_caption().rect.snap_to(issuedto_details().rect, rect::snap_type::bottom, margin_);

	widgets::label coupon_recvby_details(coupon_details_pane.get(), "coupon_recvby_details");
	coupon_recvby_details().text = "Tawanda M. Nyoni";
	coupon_recvby_details().color_fill = { 32, 34, 244 };
	coupon_recvby_details().rect.size({ 200, 20 });
	coupon_recvby_details().rect.snap_to(coupon_recvby_caption().rect, rect::snap_type::bottom, 2.f);

	widgets::label comments_caption(coupon_details_pane.get(), "comments_caption");
	comments_caption().text = "Comments";
	comments_caption().color_text = caption_color_;
	comments_caption().color_fill = { 32, 34, 244 };
	comments_caption().rect.size({ 200, 20 });
	comments_caption().rect.snap_to(coupon_recvby_details().rect, rect::snap_type::bottom, margin_);

	widgets::label comments_details(coupon_details_pane.get(), "comments_details");
	comments_details().text = "It was an emergency";
	comments_details().color_fill = { 32, 34, 244 };
	comments_details().rect.size({ 200, 20 });
	comments_details().rect.snap_to(comments_caption().rect, rect::snap_type::bottom, 2.f);

	return true;
}

