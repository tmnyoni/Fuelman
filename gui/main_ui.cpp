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
#include <liblec/lecui/widgets/toggle.h>
#include <liblec/lecui/widgets/progress_bar.h>
#include <liblec/lecui/widgets/progress_indicator.h>
#include <liblec/lecui/containers/tab_pane.h>

#include "main_ui.h"
#include "forms/dispatch.h"
#include "forms/addcoupons.h"
#include "forms/edit_coupon.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

bool dashboard::on_initialize(std::string& error) {
	controls_
		.allow_minimize(true)
		.allow_resize(false);
	appearance_.theme(themes::dark);
	dims_.set_size({ 800.f, 700.f });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man_.add(main_page_name_);

	containers::tab_pane_builder tabs(page, "main_tab");
	tabs()
		.border(1.f)
		.tabs_border(0.f)
		.color_tabs({ 255, 255, 255, 0 })
		.tab_side(containers::tab_pane_specs::side::left)
		.caption_reserve({ "Dashboard", "Coupons", "Reports", "Settings" })
		.rect().set(margin_, 0, page.size().width - (margin_ * 2), page.size().height - margin_);

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Dashboard.
	containers::tab_builder dashboard_tab(tabs, "Dashboard");

	///////////////////// Left Pane
	containers::pane_builder top_left_pane(dashboard_tab.get(), "left_pane");
	top_left_pane()
		.border(0.f)
		.corner_radius_x(0.f)
		.corner_radius_y(0.f)
		.color_fill(rgba(255, 0, 0, 0))
		.rect(rect()
			.left(margin_ / 2.f)
			.top(margin_)
			.width((dashboard_tab.get().size().width / 2.f) - margin_)
			.height((dashboard_tab.get().size().height / 2.f) - margin_));

	try {
		widgets::label_builder fuel_consumption_caption(top_left_pane.get());
		fuel_consumption_caption()
			.text("Fuel Consumption (%)")
			.center_h(true)
			.font_size(12)
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ top_left_pane.get().size().width, 25.f })
			.place(
				{
					margin_,
					200.f,
					margin_,
					20.f
				}, 0.f, 0.f
			);

		// Petrol summary.
		widgets::label_builder total_petrol_caption(top_left_pane.get());
		total_petrol_caption()
			.text("Petrol")
			.center_h(true)
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(margin_)
				.top(fuel_consumption_caption().rect().bottom() + margin_ * 2)
				.width((top_left_pane.get().size().width / 2) - 5)
				.height(20.f));

		widgets::progress_indicator_builder petrol_progress(top_left_pane.get(), "petrol-progress");
		petrol_progress()
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(total_petrol_caption().rect(), snap_type::bottom, margin_);

		widgets::label_builder total_petrol_details(top_left_pane.get(), "petrol_details");
		total_petrol_details().text(std::to_string(state_.get_db().on_get_petrol_volume()) + "  Litres");
		total_petrol_details()
			.center_h(true)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(margin_)
				.top(petrol_progress().rect().bottom() + margin_)
				.width(top_left_pane.get().size().width / 2.f)
				.height(20.f));

		// diesel summary.
		widgets::label_builder total_diesel_caption(top_left_pane.get());
		total_diesel_caption()
			.text("Diesel")
			.center_h(true)
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ (top_left_pane.get().size().width / 2.f) - margin_, 20.f })
			.snap_to(total_petrol_caption().rect(), snap_type::right, margin_);

		widgets::progress_indicator_builder diesel_progress(top_left_pane.get(), "diesel-progress");
		diesel_progress()
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(total_diesel_caption().rect(), snap_type::bottom, margin_);


		widgets::label_builder total_diesel_details(top_left_pane.get(), "diesel_details");
		total_diesel_details().text(std::to_string(state_.get_db().on_get_diesel_volume()) + "<span style = 'font-size: 10.0pt;'>" + " Litres  </span> ");
		total_diesel_details()
			.center_h(true)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ top_left_pane.get().size().width / 2.f, 20.f })
			.snap_to(diesel_progress().rect(), snap_type::bottom, margin_);
	}
	catch (const std::exception& ex) {
		message(std::string(ex.what()));
	}

	try{
		///////////////////////// Right pane.
		containers::pane_builder right_pane(dashboard_tab.get(), "right_pane");
		right_pane()
			.border(0.f)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.color_fill(rgba(255, 0, 0, 0))
			.rect().size((dashboard_tab.get().size().width / 2.f) - margin_, (dashboard_tab.get().size().height / 2.f) - margin_)
			.snap_to(top_left_pane().rect(), snap_type::right, margin_);

		widgets::label_builder fuel_consumption_caption(right_pane.get());
		fuel_consumption_caption()
			.text("Fuel Consumption (Departments)")
			.center_h(true)
			.font_size(12)
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(margin_)
				.top(margin_)
				.width(right_pane.get().size().width - margin_)
				.height(20.f));

		std::vector<database::row> fuel_stats;
		widgets::table_view_builder fuel_stats_table(right_pane.get(), "fuel-stats-departments");
		{
			std::vector<table_column> fuel_stats_columns =
			{
				{ "Issued To", 180 },
				{ "Volume", 100 },
			};

			fuel_stats_table()
				.border(0.f)
				.fixed_number_column(true)
				.corner_radius_x(0.f)
				.corner_radius_y(0.f)
				.user_sort(true)
				.color_fill(rgba(255, 255, 255, 0))
				.columns(fuel_stats_columns)
				.rect(rect()
					.left(margin_)
					.right(right_pane.get().size().width - margin_)
					.top(fuel_consumption_caption().rect().bottom() + margin_)
					.bottom(280.f))
				.events().selection = [&]
			(const std::vector<table_row>& rows) {
				on_select_coupon(rows);
			};
		}
	}
	catch (const std::exception& ex) {
		message(std::string(ex.what()));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Coupons tab.

	containers::tab_builder coupons_tab(tabs, "Coupons");

	widgets::button_builder add_coupons(coupons_tab.get());
	add_coupons()
		.text("New Coupons")
		.rect().size({ 89.f, 20.f })
		.place(rect()
			.left(margin_)
			.right(margin_ * 10.f)
			.top(margin_ / 4.f)
			.bottom(margin_ * 2.f)
			, 0.f, 0.f
		);
	add_coupons().events().action = [&]() { on_add_coupons(error);  };

	std::vector<database::row> coupons_data;
	widgets::table_view_builder coupons_table(coupons_tab.get(), "coupons_table");
	{
		std::vector<table_column> coupons_table_cols =
		{
			{ "Serial Number", 100 },
			{ "Fuel", 90 },
			{ "Volume", 80 },
			{ "Date", 90 },
		};

		if (!state_.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		coupons_table()
			.border(1)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.color_fill(rgba(255, 255, 255, 0))
			.columns(coupons_table_cols)
			.data(coupons_data)
			.rect(rect()
				.left(margin_)
				.right(coupons_tab.get().size().width / 2.f - margin_)
				.top(add_coupons().rect().bottom() + margin_)
				.bottom(coupons_tab.get().size().height - margin_))
			.events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};
	}

	//////////////////////// Coupons details pane.

	containers::pane_builder coupon_details_pane(coupons_tab.get(), "coupon_details_pane");
	coupon_details_pane()
		.border(1.f)
		.corner_radius_x(0.f)
		.corner_radius_y(0.f)
		.color_fill(rgba(255, 255, 255, 0))
		.rect(rect()
			.left(coupons_table().rect().right() + margin_)
			.right(coupons_tab.get().size().width - margin_)
			.top(coupons_table().rect().top())
			.bottom(coupons_table().rect().bottom()));

	std::map<std::string, std::any> coupon;
	if (!coupons_data.empty())
		coupon = coupons_data[0]; //data to display.

	using get = database::get;
	widgets::label_builder date_label(coupon_details_pane.get());
	date_label()
		.text("Date")
		.color_text(caption_color_)
		.rect().size({ 200.f, 20.f })
		.place(rect()
			.left(margin_)
			.right(200.f)
			.top(margin_)
			.bottom(20.f), 0.f, 0.f);

	widgets::label_builder date_details(coupon_details_pane.get(), "date_details");
	coupon.empty() ? date_details().text("") :
		date_details().text(get::text(coupon.at("Date")));
	date_details()
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200.f, 20.f })
		.snap_to(date_label().rect(), snap_type::bottom, 2.f);

	widgets::label_builder coupon_serialno_caption(coupon_details_pane.get());
	coupon_serialno_caption()
		.text("Serial Number")
		.color_text(caption_color_)
		.rect().size({ 200.f, 20.f })
		.snap_to(date_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder coupon_serialno_details(coupon_details_pane.get(), "coupon_serialno_details");
	coupon.empty() ? coupon_serialno_details().text("") :
		coupon_serialno_details().text(get::text(coupon.at("Serial Number")));
	coupon_serialno_details()
		.rect().size({ 200.f, 20.f })
		.snap_to(coupon_serialno_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder volume_issued_caption(coupon_details_pane.get());
	volume_issued_caption()
		.text("Volume")
		.color_text(caption_color_)
		.rect().size({ 200.f, 20.f })
		.snap_to(coupon_serialno_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder volume_details(coupon_details_pane.get(), "volume_details");
	coupon.empty() ? volume_details().text("") :
		volume_details().text(get::text(coupon.at("Volume")) + " Litres");
	volume_details()
		.rect().size({ 200.f, 20.f })
		.snap_to(volume_issued_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder fuel_caption(coupon_details_pane.get());
	fuel_caption()
		.text("Fuel")
		.color_text(caption_color_);
	fuel_caption().rect().size({ 200.f, 20.f })
		.snap_to(volume_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder fuel_details(coupon_details_pane.get(), "fuel_details");
	coupon.empty() ? fuel_details().text("") :
		fuel_details().text(get::text(coupon.at("Fuel")));
	fuel_details()
		.rect().size({ 200.f, 20.f })
		.snap_to(fuel_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder issuedby_caption(coupon_details_pane.get());
	issuedby_caption()
		.text("Issued by")
		.color_text(caption_color_)
		.rect().size({ 200.f, 20.f })
		.snap_to(fuel_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder issuedby_details(coupon_details_pane.get(), "issuedby_details");
	coupon.empty() ? issuedby_details().text("") :
		issuedby_details().text(get::text(coupon.at("Issued By")));
	issuedby_details()
		.rect().size({ 200.f, 20.f })
		.snap_to(issuedby_caption().rect(), snap_type::bottom, 2.f);

	widgets::button_builder btn_dispatch_coupon(coupon_details_pane.get());
	btn_dispatch_coupon()
		.text("Dispatch")
		.rect().size({ 80.f, 20.f })
		.place(rect()
			.left(margin_)
			.right(100.f + margin_)
			.top(issuedby_details().rect().get_bottom() + margin_)
			.bottom(issuedby_details().rect().get_bottom() + margin_ * 2.f), 0.f, 0.f);

	btn_dispatch_coupon().events().action = [&]() {
		std::string error_;
		if (!on_dispatch_coupon(error_)) {
			message("Error: " + error_);
			return;
		}
	};

	widgets::button_builder btn_return_coupoon(coupon_details_pane.get());
	btn_return_coupoon()
		.text("Return")
		.rect().size({ 80.f, 20.f })
		.snap_to(btn_dispatch_coupon().rect(), snap_type::right, margin_);
	btn_return_coupoon().events().action = [&]() {
		if (prompt("Are you sure you, return?")) {
			return;
		}
	};

	widgets::button_builder btn_delete_coupons_button(coupon_details_pane.get());
	btn_delete_coupons_button()
		.text("Delete")
		.rect().size({ 80.f, 20.f })
		.snap_to(btn_return_coupoon().rect(), snap_type::right, margin_);
	btn_delete_coupons_button().events().action = [&]() {
		if (prompt("Are you sure you, delete?")) {
			return;
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Reports
	containers::tab_builder reports_tab(tabs, "Reports");

	widgets::label_builder report_date_caption(reports_tab.get());
	report_date_caption()
		.text("Date")
		.color_text(caption_color_)
		.rect(rect()
			.left(margin_ * 2.f)
			.top(margin_)
			.width(200.f)
			.height(20.f));

	widgets::combobox_builder reports_dates_cbo(reports_tab.get(), "fueltype_cbo");
	{
		std::vector<widgets::combobox_specs::combobox_item> dates = {
			{ "today" }, { "yesterday" }
		};

		reports_dates_cbo()
			.items(dates)
			.color_fill({ 255,255,255,0 })
			.rect().size(200.f, 25.f)
			.snap_to(report_date_caption().rect(), snap_type::bottom, 0);
		reports_dates_cbo().events().selection = [](const std::string& selected) {};
	}

	std::vector<database::row> reports_items_data;
	widgets::table_view_builder reports_items_table(reports_tab.get(), "reports_items_table");
	{
		std::vector<table_column> reports_table_columns =
		{
			{ "Serial Number", 150 },
			{ "Fuel", 90 },
			{ "Issued By", 80 },
			{ "Receiver", 80 },
			{ "Volume", 80 },
		};

		if (!state_.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		reports_items_table()
			.border(0.f)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.color_fill(rgba(255, 255, 255, 0))
			.columns(reports_table_columns)
			.data(reports_items_data)
			.rect(rect()
				.left(margin_)
				.right(550.f)
				.top(reports_dates_cbo().rect().bottom() + margin_)
				.bottom(400.f));
		reports_items_table().events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};

		widgets::label_builder volume_total_caption(reports_tab.get());
		volume_total_caption()
			.text("TOTAL")
			.color_text(caption_color_)
			.rect().size({ 80.f, 20.f })
			.set(reports_items_table().rect().right() - 180.f, reports_items_table().rect().bottom(), 100.f, 20.f);

		widgets::label_builder volume_total_text(reports_tab.get());
		volume_total_text()
			.text("0000 Litres")
			.color_text(caption_color_)
			.rect().size({ 80.f, 20.f })
			.snap_to(volume_total_caption().rect(), snap_type::right, margin_);

		const size icon_size{ 80.f, 30.f };

		widgets::icon_builder print_button(reports_tab.get());
		print_button()
			.text("Print")
			.font_size(9.f)
			.file("assets/print.png")
			.max_image_size(20.f)
			.rect(rect()
				.left(reports_items_table().rect().right() - (80.f * 3.f) - 2.f * (margin_ / 3.f))
				.top(volume_total_text().rect().bottom() + 2.f * margin_)
				.width(icon_size.width)
				.height(icon_size.height))
			.events().action = [&]() {};

		widgets::icon_builder share_button(reports_tab.get());
		share_button()
			.text("Share")
			.font_size(9.f)
			.file("assets/share.png")
			.max_image_size(20.f)
			.rect(print_button().rect())
			.rect().snap_to(print_button().rect(), snap_type::right, margin_ / 3.f);
		share_button().events().action = [&]() {};

		widgets::icon_builder preview_button(reports_tab.get());
		preview_button()
			.text("Preview")
			.font_size(9.f)
			.file("assets/preview.png")
			.max_image_size(20.f)
			.rect(print_button().rect())
			.rect().snap_to(share_button().rect(), snap_type::right, margin_ / 3.f);
		preview_button().events().action = [&]() {};
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Settings
	containers::tab_builder settings_tab(tabs, "Settings");

	///////////////// Appearance settings.
	widgets::label_builder appearance_settings_caption(settings_tab.get());
	appearance_settings_caption()
		.text("Appearance")
		.color_text(caption_color_)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.set(margin_, margin_, 100, 25);

	widgets::label_builder theme_settings_caption(settings_tab.get());
	theme_settings_caption()
		.text("Theme")
		.rect().size({ 200.f, 20.f })
		.set(margin_, appearance_settings_caption().rect().bottom() + margin_, 200.f, 20.f);

	widgets::combobox_builder theme_settings_cbo(settings_tab.get(), "theme-settings-theme");
	{
		std::vector<widgets::combobox_specs::combobox_item> themes = {
			{ "light" }, { "dark" }
		};

		theme_settings_cbo()
			.items(themes)
			.color_fill({ 255,255,255,0 })
			.rect().size(200.f, 25.f)
			.snap_to(theme_settings_caption().rect(), snap_type::bottom, 0.f);
		theme_settings_cbo().events().selection = [](const std::string& selected) {};
	}

	///////////////// Updates settings.
	widgets::label_builder updates_settings_caption(settings_tab.get());
	updates_settings_caption()
		.text("Updates")
		.color_text(caption_color_)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(theme_settings_cbo().rect(), snap_type::bottom, 50.f);

	// add auto check updates toggle button
	widgets::label_builder autocheck_updates_caption(settings_tab.get());
	autocheck_updates_caption()
		.text("Auto-check")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_settings_caption().rect(), snap_type::bottom, margin_);

	widgets::toggle_builder autocheck_updates(settings_tab.get());
	autocheck_updates()
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically check for updates")
		.on(true)
		.rect(autocheck_updates_caption().rect());
	autocheck_updates().rect().snap_to(autocheck_updates_caption().rect(), snap_type::bottom, 0.f);
	autocheck_updates().events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	widgets::label_builder autodownload_updates_caption(settings_tab.get());
	autodownload_updates_caption()
		.text("Auto-Download")
		.rect().size({ 200.f, 20.f })
		.snap_to(autocheck_updates().rect(), snap_type::bottom, margin_);

	widgets::toggle_builder autodownload_updates(settings_tab.get());
	autodownload_updates()
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically download updates")
		.on(true)
		.rect(autocheck_updates_caption().rect());
	autodownload_updates().rect().snap_to(autodownload_updates_caption().rect(), snap_type::bottom, 0.f);
	autodownload_updates().events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	//////////////////// Backup and Restore settings.
	widgets::label_builder backup_restore_caption(settings_tab.get());
	backup_restore_caption()
		.text("Backup and Restore")
		.color_text(caption_color_)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(autodownload_updates().rect(), snap_type::bottom, 50.f);

	widgets::button_builder backup_now_btn(settings_tab.get());
	backup_now_btn()
		.text("Backup now")
		.rect().size({ 100.f, 20.f })
		.set(margin_, backup_restore_caption().rect().bottom() + margin_, 100.f, 20.f);

	tabs.select("Dashboard");

	page_man_.show(main_page_name_);
	return true;
}

bool dashboard::on_select_coupon(const std::vector<table_row>& rows) {
	try
	{
		std::string error;
		auto serial_number = rows[0].at("Serial Number");
		std::vector<database::row>  data_to_display;

		if (!state_.get_db().on_get_coupon(database::get::text(serial_number), data_to_display, error)) {
			message("Error: " + error);	//todo: remove this line.
			return false;
		}

		auto coupons_details_pane_alias = main_page_name_ + "/main_tab/Coupons/coupon_details_pane/";

		for (auto row : data_to_display) {
			get_label_specs(coupons_details_pane_alias + "date_details")
				.text(database::get::text(row.at("Date")));

			get_label_specs(coupons_details_pane_alias + "coupon_serialno_details")
				.text(database::get::text(row.at("Serial Number")));

			get_label_specs(coupons_details_pane_alias + "volume_details")
				.text(database::get::text(row.at("Volume")) + " Litres");

			get_label_specs(coupons_details_pane_alias + "fuel_details")
				.text(database::get::text(row.at("Fuel")));

			get_label_specs(coupons_details_pane_alias + "issuedby_details")
				.text(database::get::text(row.at("Issued By")));

			return true;
		}
		return true;
	}
	catch (const std::exception& ex) {
		message("Error: " + std::string(ex.what()));
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///								This function needs a lot of cleaning.
bool dashboard::on_dispatch_coupon(std::string& error) {
	auto table_view =
		get_table_view_specs(main_page_name_ + "/main_tab/Coupons/coupons_table");

	auto selected_ = table_view.selected();

	if (selected_.empty()) {
		error = "Error: No coupon selected!";
		return false;
	}

	auto first_row = table_view.data()[selected_[0]];
	std::any serial_number_ = first_row.at("Serial Number");

	std::vector<database::row> selected_coupons;
	if (!state_.get_db().on_get_coupon(serial_number_, selected_coupons, error)) {
		return false;
	}

	std::map<std::string, std::any> edited_coupon;
	edited_coupon = selected_coupons[0];

	if (edited_coupon.empty())
		return false;

	bool is_changed = false;
	dispatch_form dispatch_form_("Dispatch Coupon", *this, state_, edited_coupon, is_changed);
	if (!dispatch_form_.show(error)) {
		return false;
	}

	// Updating dashboard.
	{
		get_label_specs(main_page_name_ + "/main_tab/Dashboard/left_pane/petrol_details")
			.text(std::to_string(state_.get_db().on_get_petrol_volume()) + " Litres");

		get_label_specs(main_page_name_ + "/main_tab/Dashboard/left_pane/diesel_details")
			.text(std::to_string(state_.get_db().on_get_diesel_volume()) + " Litres");
	}

	// Updating the coupons table.
	if (is_changed) {
		auto& old_coupons =
			get_table_view_specs(main_page_name_ + "/main_tab/Coupons/coupons_table").data();

		std::vector<table_row> new_coupons;
		new_coupons.reserve(old_coupons.size() - 1);

		for (const auto& row : old_coupons) {
			if (get::text(row.at("Serial Number")) != get::text(serial_number_)) {
				new_coupons.push_back(row);
			}
		}

		old_coupons = new_coupons;
		update();
	}

	return true;
}

bool dashboard::on_add_coupons(std::string& error) {
	std::vector<table_row> saved_coupons;

	addcopoupons_form addcoupons("Add Coupons", *this, state_, saved_coupons);
	if (!addcoupons.show(error)) {
		message("Error: " + error);
		return false;
	}

	try {
		// updating dashboard.
		{
			get_label_specs(main_page_name_ + "/main_tab/Dashboard/left_pane/petrol_details")
				.text(std::to_string(state_.get_db().on_get_petrol_volume()) + " Litres");

			get_label_specs(main_page_name_ + "/main_tab/Dashboard/left_pane/diesel_details")
				.text(std::to_string(state_.get_db().on_get_diesel_volume()) + " Litres");
		}

		auto coupons_table =
			get_table_view_specs(main_page_name_ + "/main_tab/Coupons/coupons_table");

		auto table_size = coupons_table.data().size();

		if (!saved_coupons.empty()) {
			for (int i = 1; const auto & row : saved_coupons) {
				get_table_view_specs(main_page_name_ + "/main_tab/Coupons/coupons_table")
					.data().push_back(
						{
							{ "#", std::to_string(table_size + i++)},
							{ "Date", row.at("Date")},
							{ "Volume", row.at("Volume")},
							{ "Serial Number", row.at("Serial Number")},
							{ "Fuel", row.at("Fuel")},
						}
				);
			}
		}

		update();
	}
	catch (const std::exception& ex) {
		error = std::string(ex.what());
		return false;
	}

	return true;
}

color dashboard::rgba(
	const unsigned short& r,
	const unsigned short& g,
	const unsigned short& b,
	const unsigned short& a) {
	return color({ r, g, b, a });
}

