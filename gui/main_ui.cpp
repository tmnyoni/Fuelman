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
#include <liblec/lecui/menus/context_menu.h>

#include "main_ui.h"
#include "forms/dispatch.h"
#include "forms/addcoupons.h"
#include "forms/edit_coupon.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

bool dashboard::on_initialize(std::string& error) {
	_controls
		.allow_minimize(true)
		.allow_resize(false);
	_appearance.theme(themes::dark);
	_dims.set_size({ 800.f, 700.f });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = _page_man.add(_page_name);

	containers::tab_pane_builder tabs(page, "main_tab");
	tabs()
		.border(1.f)
		.tabs_border(0.f)
		.color_tabs({ 255, 255, 255, 0 })
		.tab_side(containers::tab_pane_specs::side::left)
		.caption_reserve({ "Dashboard", "Coupons", "Reports", "Settings" })
		.rect().set(_margin, 0, page.size().width - (_margin * 2), page.size().height - _margin);

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
			.left(_margin / 2.f)
			.top(_margin)
			.width((dashboard_tab.get().size().width / 2.f) - _margin)
			.height((dashboard_tab.get().size().height / 2.f) - _margin));

	try {
		widgets::label_builder fuel_consumption_caption(top_left_pane.get());
		fuel_consumption_caption()
			.text("Fuel Consumption (%)")
			.center_h(true)
			.font_size(12)
			.color_text(_caption_color)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ top_left_pane.get().size().width, 25.f })
			.place(
				{
					_margin,
					200.f,
					_margin,
					20.f
				}, 0.f, 0.f
			);

		// Petrol summary.
		widgets::label_builder total_petrol_caption(top_left_pane.get());
		total_petrol_caption()
			.text("Petrol")
			.center_h(true)
			.color_text(_caption_color)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(_margin)
				.top(fuel_consumption_caption().rect().bottom() + _margin * 2)
				.width((top_left_pane.get().size().width / 2) - 5)
				.height(20.f));

		widgets::progress_indicator_builder petrol_progress(top_left_pane.get(), "petrol-progress");
		petrol_progress()
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(total_petrol_caption().rect(), snap_type::bottom, _margin);

		widgets::label_builder total_petrol_details(top_left_pane.get(), "petrol_details");
		total_petrol_details().text(std::to_string(_state.get_db().on_get_petrol_volume()) + "  Litres");
		total_petrol_details()
			.center_h(true)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(_margin)
				.top(petrol_progress().rect().bottom() + _margin)
				.width(top_left_pane.get().size().width / 2.f)
				.height(20.f));

		// diesel summary.
		widgets::label_builder total_diesel_caption(top_left_pane.get());
		total_diesel_caption()
			.text("Diesel")
			.center_h(true)
			.color_text(_caption_color)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ (top_left_pane.get().size().width / 2.f) - _margin, 20.f })
			.snap_to(total_petrol_caption().rect(), snap_type::right, _margin);

		widgets::progress_indicator_builder diesel_progress(top_left_pane.get(), "diesel-progress");
		diesel_progress()
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(total_diesel_caption().rect(), snap_type::bottom, _margin);


		widgets::label_builder total_diesel_details(top_left_pane.get(), "diesel_details");
		total_diesel_details().text(std::to_string(_state.get_db().on_get_diesel_volume()) + "<span style = 'font-size: 10.0pt;'>" + " Litres  </span> ");
		total_diesel_details()
			.center_h(true)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ top_left_pane.get().size().width / 2.f, 20.f })
			.snap_to(diesel_progress().rect(), snap_type::bottom, _margin);
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
			.rect().size((dashboard_tab.get().size().width / 2.f) - _margin, (dashboard_tab.get().size().height / 2.f) - _margin)
			.snap_to(top_left_pane().rect(), snap_type::right, _margin);

		widgets::label_builder fuel_consumption_caption(right_pane.get());
		fuel_consumption_caption()
			.text("Fuel Consumption (Departments)")
			.center_h(true)
			.font_size(12)
			.color_text(_caption_color)
			.color_fill(rgba(32, 34, 244, 0))
			.rect(rect()
				.left(_margin)
				.top(_margin)
				.width(right_pane.get().size().width - _margin)
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
					.left(_margin)
					.right(right_pane.get().size().width - _margin)
					.top(fuel_consumption_caption().rect().bottom() + _margin)
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

	widgets::icon_builder add_coupons_button(coupons_tab.get());
	add_coupons_button()
		.text("Add Coupons")
		.font_size(9.f)
		.file("assets/print.png")
		.max_image_size(20.f)
		.rect(rect()
			.left(_margin * 1.5f)
			.top(_margin / 4.f)
			.width(_margin * 12.f)
			.height(_margin * 3.f))
		.events().action = [&]() { on_add_coupons(error); };

	std::vector<database::row> coupons_data;
	widgets::table_view_builder coupons_table(coupons_tab.get(), "coupons_table");
	{
		std::vector<table_column> coupons_table_cols =
		{
			{ "Serial Number", 250 },
			{ "Fuel", 90 },
			{ "Volume", 80 },
			{ "Date", 90 },
			{ "Issued By", 90 },
		};

		if (!_state.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		coupons_table()
			.border(0)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.color_fill(rgba(255, 255, 255, 0))
			.columns(coupons_table_cols)
			.data(coupons_data)
			.rect(rect()
				.left(_margin)
				.right(coupons_tab.get().size().width - _margin)
				.top(add_coupons_button().rect().bottom() + _margin)
				.bottom(coupons_tab.get().size().height - (_margin * _margin)))
			.events().context_menu = [&]
			(const std::vector<table_row>& rows) {

				context_menu::specs context_menu_specs;
				context_menu_specs.items = {
					{"Dispatch", "assets/pump.png"}, 
					{"Return",  "assets/pump.png"},
					{"Delete",  "assets/pump.png"}};

				auto selected_context_menu_item = context_menu()(*this, context_menu_specs);

				if (selected_context_menu_item.compare("Dispatch") == 0) {
					if (rows.empty()){
						message("Error: no selected item");
						return;
					}

					std::string error_;
					if (!on_dispatch_coupon(rows, error_)) {
						message("Error: " + error_);
						return;
					}
				}
		};
	}

	widgets::label_builder total_coupons_caption(coupons_tab.get(), "number-of-coupons");
	total_coupons_caption()
		.text(std::to_string(coupons_data.size()) + " coupons available")
		.color_text(_caption_color)
		.rect().set(_margin * 1.5f, coupons_table().rect().bottom(), 200.f, 20.f);


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Reports
	containers::tab_builder reports_tab(tabs, "Reports");

	widgets::label_builder report_date_caption(reports_tab.get());
	report_date_caption()
		.text("Date")
		.color_text(_caption_color)
		.rect(rect()
			.left(_margin * 2.f)
			.top(_margin)
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

		if (!_state.get_db().on_get_coupons(coupons_data, error))
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
				.left(_margin)
				.right(550.f)
				.top(reports_dates_cbo().rect().bottom() + _margin)
				.bottom(400.f));
		reports_items_table().events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};

		widgets::label_builder volume_total_caption(reports_tab.get());
		volume_total_caption()
			.text("TOTAL")
			.color_text(_caption_color)
			.rect().size({ 80.f, 20.f })
			.set(reports_items_table().rect().right() - 180.f, reports_items_table().rect().bottom(), 100.f, 20.f);

		widgets::label_builder volume_total_text(reports_tab.get());
		volume_total_text()
			.text("0000 Litres")
			.color_text(_caption_color)
			.rect().size({ 80.f, 20.f })
			.snap_to(volume_total_caption().rect(), snap_type::right, _margin);

		const size icon_size{ 80.f, 30.f };

		widgets::icon_builder print_button(reports_tab.get());
		print_button()
			.text("Print")
			.font_size(9.f)
			.file("assets/print.png")
			.max_image_size(20.f)
			.rect(rect()
				.left(reports_items_table().rect().right() - (80.f * 3.f) - 2.f * (_margin / 3.f))
				.top(volume_total_text().rect().bottom() + 2.f * _margin)
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
			.rect().snap_to(print_button().rect(), snap_type::right, _margin / 3.f);
		share_button().events().action = [&]() {};

		widgets::icon_builder preview_button(reports_tab.get());
		preview_button()
			.text("Preview")
			.font_size(9.f)
			.file("assets/preview.png")
			.max_image_size(20.f)
			.rect(print_button().rect())
			.rect().snap_to(share_button().rect(), snap_type::right, _margin / 3.f);
		preview_button().events().action = [&]() {};
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Settings
	containers::tab_builder settings_tab(tabs, "Settings");

	///////////////// Appearance settings.
	widgets::label_builder appearance_settings_caption(settings_tab.get());
	appearance_settings_caption()
		.text("Appearance")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.set(_margin, _margin, 100, 25);

	widgets::label_builder theme_settings_caption(settings_tab.get());
	theme_settings_caption()
		.text("Theme")
		.rect().size({ 200.f, 20.f })
		.set(_margin, appearance_settings_caption().rect().bottom() + _margin, 200.f, 20.f);

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
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(theme_settings_cbo().rect(), snap_type::bottom, 50.f);

	// add auto check updates toggle button
	widgets::label_builder autocheck_updates_caption(settings_tab.get());
	autocheck_updates_caption()
		.text("Auto-check")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_settings_caption().rect(), snap_type::bottom, _margin);

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
		.snap_to(autocheck_updates().rect(), snap_type::bottom, _margin);

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
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(autodownload_updates().rect(), snap_type::bottom, 50.f);

	widgets::button_builder backup_now_btn(settings_tab.get());
	backup_now_btn()
		.text("Backup now")
		.rect().size({ 100.f, 20.f })
		.set(_margin, backup_restore_caption().rect().bottom() + _margin, 100.f, 20.f);

	tabs.select("Dashboard");

	_page_man.show(_page_name);
	return true;
}

bool dashboard::on_select_coupon(const std::vector<table_row>& rows) {
	try
	{
		std::string error;
		auto serial_number = rows[0].at("Serial Number");
		std::vector<database::row>  data_to_display;

		if (!_state.get_db().on_get_coupon(database::get::text(serial_number), data_to_display, error)) {
			message("Error: " + error);	//todo: remove this line.
			return false;
		}

		auto coupons_details_pane_alias = _page_name + "/main_tab/Coupons/coupon_details_pane/";

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
bool dashboard::on_dispatch_coupon(const std::vector<table_row>& rows,std::string& error) {
	auto table_view =
		get_table_view_specs(_page_name + "/main_tab/Coupons/coupons_table");

	auto selected_ = table_view.selected();

	if (selected_.empty()) {
		error = "Error: No coupon selected!";
		return false;
	}

	auto first_row = table_view.data()[selected_[0]];
	std::any serial_number_ = first_row.at("Serial Number");

	std::vector<database::row> selected_coupons;
	if (!_state.get_db().on_get_coupon(serial_number_, selected_coupons, error)) {
		return false;
	}

	std::map<std::string, std::any> edited_coupon;
	edited_coupon = selected_coupons[0];

	if (edited_coupon.empty())
		return false;

	bool is_changed = false;
	dispatch_form dispatch_form_("Dispatch Coupon", *this, _state, edited_coupon, is_changed);
	if (!dispatch_form_.show(error)) {
		return false;
	}

	// Updating dashboard.
	{
		get_label_specs(_page_name + "/main_tab/Dashboard/left_pane/petrol_details")
			.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

		get_label_specs(_page_name + "/main_tab/Dashboard/left_pane/diesel_details")
			.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");
	}

	// Updating the coupons table.
	if (is_changed) {
		auto& old_coupons =
			get_table_view_specs(_page_name + "/main_tab/Coupons/coupons_table").data();

		std::vector<table_row> new_coupons;
		new_coupons.reserve(old_coupons.size() - 1);

		for (const auto& row : old_coupons) {
			if (get::text(row.at("Serial Number")) != get::text(serial_number_)) {
				new_coupons.push_back(row);
			}
		}

		old_coupons = new_coupons;
		update();


		get_label_specs(_page_name + "/main_tab/Coupons/number-of-coupons")
			.text(std::to_string(new_coupons.size()) + " coupons available");
	}

	return true;
}

bool dashboard::on_add_coupons(std::string& error) {
	std::vector<table_row> saved_coupons;

	addcopoupons_form addcoupons("Add Coupons", *this, _state, saved_coupons);
	if (!addcoupons.show(error)) {
		message("Error: " + error);
		return false;
	}

	try {
		// updating dashboard.
		{
			get_label_specs(_page_name + "/main_tab/Dashboard/left_pane/petrol_details")
				.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

			get_label_specs(_page_name + "/main_tab/Dashboard/left_pane/diesel_details")
				.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");
		}

		

		if (!saved_coupons.empty()) {
			for (const auto & row : saved_coupons) {
				get_table_view_specs(_page_name + "/main_tab/Coupons/coupons_table")
					.data().push_back(
						{
							{ "Date", row.at("Date")},
							{ "Volume", row.at("Volume")},
							{ "Serial Number", row.at("Serial Number")},
							{ "Fuel", row.at("Fuel")},
						}
				);
			}
		}

		update();

		get_label_specs(_page_name + "/main_tab/Coupons/number-of-coupons")
			.text(
				std::to_string(get_table_view_specs(_page_name + "/main_tab/Coupons/coupons_table")
					.data().size()) + " coupons available");
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

