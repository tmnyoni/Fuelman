
// include liblec headers.
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/containers/pane.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/table_view.h>
#include <liblec/lecui/widgets/icon.h>
#include <liblec/lecui/widgets/image_view.h>
#include <liblec/lecui/widgets/toggle.h>
#include <liblec/lecui/widgets/progress_indicator.h>
#include <liblec/lecui/containers/tab_pane.h>
#include <liblec/lecui/menus/context_menu.h>

// include local headers.
#include "../main_ui.h"
#include "../dispatch.h"
#include "../addcoupons.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

// initialise form attributes.
bool main_window::on_initialize(std::string& error) {
	_controls
		.allow_minimize(true)
		.allow_resize(false);
	_appearance.theme(themes::dark);
	_dims.set_size({ 800.f, 700.f });
	return true;
}

bool main_window::on_layout(std::string& error) {
	auto& page = _page_man.add(_page_name);

	auto& main_tab_pane = containers::tab_pane::add(page, _main_tab_name);
	main_tab_pane
		.border(1.f)
		.tabs_border(0.f)
		.color_tabs(color().alpha(0))
		.color_fill(color().alpha(0))
		.corner_radius_x(0).corner_radius_y(0)
		.tab_side(containers::tab_pane::side::left)
		.caption_reserve({ "Dashboard", "Coupons", "Reports", "Settings" })
		.rect().set(_margin, 0, page.size().get_width() - (_margin * 2), page.size().get_height() - _margin);


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Dashboard.
	auto& dashboard_tab = containers::tab::add(main_tab_pane, "Dashboard");

	////////// Top Left Pane
	auto& top_left_pane = containers::pane::add(dashboard_tab, "top-left-pane");
	top_left_pane
		.border(0.f)
		.corner_radius_x(0.f)
		.corner_radius_y(0.f)
		.rect(rect()
			.left(_margin / 2.f)
			.top(_margin / 2.f)
			.width((dashboard_tab.size().get_width() / 2.f) - _margin)
			.height((dashboard_tab.size().get_width() / 2.f) - _margin));

	try {
		auto& consumption_sumary_title = widgets::label::add(top_left_pane);
		consumption_sumary_title
			.text("Fuel Consumption (%)")
			.center_h(true)
			.font_size(12)
			.color_text(_caption_color)
			.rect().size({ top_left_pane.size().get_width(), 25.f })
			.place(
				{
					_margin,
					200.f,
					_margin,
					20.f
				}, 0.f, 0.f
			);

		// Petrol summary.
		auto& petrol_summary_title = widgets::label::add(top_left_pane);
		petrol_summary_title
			.text("Petrol")
			.center_h(true)
			.color_text(_caption_color)
			.rect(rect()
				.left(_margin)
				.top(consumption_sumary_title.rect().bottom() + _margin * 2.f)
				.width((top_left_pane.size().get_width() / 2.f) - _margin / 2.f)
				.height(20.f));

		auto& petrol_summary_progress = widgets::progress_indicator::add(top_left_pane, "petrol-summary-progress");
		petrol_summary_progress
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(petrol_summary_title.rect(), snap_type::bottom, _margin);

		auto& petrol_summary_caption = widgets::label::add(top_left_pane, "petrol-summary-caption");
		petrol_summary_caption
			.text(std::to_string(_state.get_db().on_get_petrol_volume()) + +"<span style= 'color: green;'>" + " Litres  </span> ")
			.center_h(true)
			.rect(rect()
				.left(_margin)
				.top(petrol_summary_progress.rect().bottom() + _margin)
				.width(top_left_pane.size().get_width() / 2.f)
				.height(20.f));

		// diesel summary.
		auto& diesel_summary_title = widgets::label::add(top_left_pane);
		diesel_summary_title
			.text("Diesel")
			.center_h(true)
			.color_text(_caption_color)
			.rect(petrol_summary_title.rect())
			.rect().snap_to(petrol_summary_title.rect(), snap_type::right, _margin);

		auto& diesel_summary_progress = widgets::progress_indicator::add(top_left_pane, "diesel-summary-progress");
		diesel_summary_progress
			.percentage(50.f)
			.rect(petrol_summary_progress.rect())
			.rect().snap_to(diesel_summary_title.rect(), snap_type::bottom, _margin);

		auto& diesel_summary_caption = widgets::label::add(top_left_pane, "diesel-summary-caption");
		diesel_summary_caption.
			text(std::to_string(_state.get_db().on_get_diesel_volume()) + "<span style= 'color: green;'>" + " Litres  </span> ")
			.center_h(true)
			.rect(petrol_summary_caption.rect())
			.rect().snap_to(diesel_summary_progress.rect(), snap_type::bottom, _margin);
	}
	catch (const std::exception& ex) {
		message(std::string(ex.what()));
	}

	////////// Top Right pane.
	auto& top_right_pane = containers::pane::add(dashboard_tab, "top-right-pane");
	top_right_pane
		.border(0.f)
		.corner_radius_x(0.f)
		.corner_radius_y(0.f)
		.rect(top_left_pane.rect())
		.rect().snap_to(top_left_pane.rect(), snap_type::right, _margin);

	auto& department_consumption_summary_title = widgets::label::add(top_right_pane);
	department_consumption_summary_title
		.text("Fuel Consumption (Departments)")
		.center_h(true)
		.font_size(12)
		.color_text(_caption_color)
		.rect(rect()
			.left(_margin)
			.top(_margin)
			.width(top_right_pane.size().get_width() - _margin)
			.height(20.f));

	try {
		auto& departmental_consumpt_stats_table =
			widgets::table_view::add(top_right_pane, "departmental-consumpt-stats-table");

		std::vector<table_column> departmental_consumpt_stats_columns =
		{ { "Department", 150 }, { "Volume", 85 }, };

		departmental_consumpt_stats_table
			.border(0.f)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.color_fill(color().alpha(0))
			.user_sort(true)
			.columns(departmental_consumpt_stats_columns)
			.data(_state.get_db().on_update_departmental_stats())
			.rect(rect()
				.left(_margin)
				.right(top_right_pane.size().get_width() - _margin)
				.top(department_consumption_summary_title.rect().bottom() + _margin)
				.bottom(280.f));
	}
	catch (const std::exception& ex) {
		message("Error: " + std::string(ex.what()));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Coupons tab.

	auto& coupons_tab = containers::tab::add(main_tab_pane, "Coupons");

	auto& add_coupons_button = widgets::icon::add(coupons_tab);
	add_coupons_button
		.text("Add Coupons")
		.font_size(9.f)
		.file("assets/add.png")
		.max_image_size(20.f)
		.rect(rect()
			.left(_margin * 1.5f)
			.top(_margin / 4.f)
			.width(_margin * 12.f)
			.height(_margin * 3.f))
		.events().action = [&]() { on_add_coupons(error); };

	auto& coupons_table = widgets::table_view::add(coupons_tab, "coupons-table");
	{
		std::vector<table_column> coupons_table_cols =
		{
			{ "Serial Number", 250 },
			{ "Fuel", 90 },
			{ "Volume", 80 },
			{ "Date", 90 },
			{ "Issued By", 90 },
		};

		std::vector<database::row> coupons_data;
		if (!_state.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		coupons_table
			.border(0)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.columns(coupons_table_cols)
			.data(coupons_data)
			.rect(rect()
				.left(_margin)
				.right(coupons_tab.size().get_width() - _margin)
				.top(add_coupons_button.rect().bottom() + _margin)
				.bottom(coupons_tab.size().get_height() - (_margin * _margin)))
			.events().context_menu = [&]
			(const std::vector<table_row>& rows) {

			context_menu::specs context_menu_specs;
			context_menu_specs.items = {
				{"Dispatch", "assets/pump.png"},
				{"Return",  "assets/pump.png"},
				{"Delete",  "assets/pump.png"} };

			auto selected_context_menu_item = context_menu()(*this, context_menu_specs);

			if (selected_context_menu_item.compare("Dispatch") == 0) {
				if (rows.empty()) {
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

	auto& total_coupons_caption = widgets::label::add(coupons_tab, "number-of-coupons");
	total_coupons_caption
		.text(std::to_string(coupons_table.data().size()) + " coupons available")
		.color_text(_caption_color)
		.rect().set(_margin * 1.5f, coupons_table.rect().bottom(), 200.f, 20.f);


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Reports
	auto& reports_tab = containers::tab::add(main_tab_pane, "Reports");

	auto& report_date_caption = widgets::label::add(reports_tab);
	report_date_caption
		.text("Date")
		.color_text(_caption_color)
		.rect(rect()
			.left(_margin * 2.f)
			.top(_margin)
			.width(200.f)
			.height(20.f));

	auto& report_date_select = widgets::combobox::add(reports_tab, "report-date-select");
	{
		std::vector<widgets::combobox::combobox_item> dates =
		{ { "today" }, { "yesterday" } };

		report_date_select
			.items(dates)
			.color_fill({ 255,255,255,0 })
			.rect().size(200.f, 25.f)
			.snap_to(report_date_caption.rect(), snap_type::bottom, 0);
		report_date_select.events().selection = [](const std::string& selected) {};
	}

	auto& report_items_table = widgets::table_view::add(reports_tab, "reports-items-table");
	{
		std::vector<table_column> reports_table_columns =
		{
			{ "Serial Number", 150 },
			{ "Fuel", 90 },
			{ "Issued By", 80 },
			{ "Receiver", 80 },
			{ "Volume", 80 },
		};

		std::vector<database::row> reports_items_data;
		if (!_state.get_db().on_get_coupons(reports_items_data, error))
			message("Error: " + error);

		report_items_table
			.border(0.f)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.color_fill(color().red(255).green(255).blue(255))
			.columns(reports_table_columns)
			.data(reports_items_data)
			.rect(rect()
				.left(_margin)
				.right(550.f)
				.top(report_date_select.rect().bottom() + _margin)
				.bottom(400.f));


		auto& total_volume_caption = widgets::label::add(reports_tab);
		total_volume_caption
			.text("TOTAL")
			.color_text(_caption_color)
			.rect().size({ 80.f, 20.f })
			.set(report_items_table.rect().right() - 180.f, report_items_table.rect().bottom(), 100.f, 20.f);

		auto& total_volume_text = widgets::label::add(reports_tab, "total-volume-text");
		total_volume_text
			.text("0000 Litres")
			.color_text(_caption_color)
			.rect().size({ 80.f, 20.f })
			.snap_to(total_volume_caption.rect(), snap_type::right, _margin);


		const size icon_size{ 80.f, 30.f };

		auto& print_button = widgets::icon::add(reports_tab);
		print_button
			.text("Print")
			.font_size(9.f)
			.file("assets/print.png")
			.max_image_size(20.f)
			.rect(rect()
				.left(report_items_table.rect().right() - (80.f * 3.f) - 2.f * (_margin / 3.f))
				.top(total_volume_text.rect().bottom() + 2.f * _margin)
				.width(icon_size.get_width())
				.height(icon_size.get_height()))
			.events().action = [&]() {};

		auto& share_button = widgets::icon::add(reports_tab);
		share_button
			.text("Share")
			.font_size(9.f)
			.file("assets/share.png")
			.max_image_size(20.f)
			.rect(print_button.rect())
			.rect().snap_to(print_button.rect(), snap_type::right, _margin / 3.f);
		share_button.events().action = [&]() {};

		auto& preview_button = widgets::icon::add(reports_tab);
		preview_button
			.text("Preview")
			.font_size(9.f)
			.file("assets/preview.png")
			.max_image_size(20.f)
			.rect(print_button.rect())
			.rect().snap_to(share_button.rect(), snap_type::right, _margin / 3.f);
		preview_button.events().action = [&]() {};
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Settings
	auto& settings_tab = containers::tab::add(main_tab_pane, "Settings");

	//// Appearance settings.
	auto& appearance_settings_title = widgets::label::add(settings_tab);
	appearance_settings_title
		.text("Appearance")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.set(_margin, _margin, 100, 25);

	auto& theme_settings_caption = widgets::label::add(settings_tab);
	theme_settings_caption
		.text("Theme")
		.rect().size({ 200.f, 20.f })
		.set(_margin, appearance_settings_title.rect().bottom() + _margin, 200.f, 20.f);

	auto& theme_setting_select = widgets::combobox::add(settings_tab, "theme-settings-theme");
	{
		std::vector<widgets::combobox::combobox_item> themes =
		{ { "light" }, { "dark" } };

		theme_setting_select
			.items(themes)
			.color_fill({ 255,255,255,0 })
			.rect().size(200.f, 25.f)
			.snap_to(theme_settings_caption.rect(), snap_type::bottom, 0.f);
		theme_setting_select.events().selection = [](const std::string& selected) {};
	}

	//// Updates settings.
	auto& updates_settings_title = widgets::label::add(settings_tab);
	updates_settings_title
		.text("Updates")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(theme_setting_select.rect(), snap_type::bottom, 50.f);

	// update auto check.
	auto& updates_autocheck_caption = widgets::label::add(settings_tab);
	updates_autocheck_caption
		.text("Auto-check")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_settings_title.rect(), snap_type::bottom, _margin);

	auto& updates_autocheck_toggle = widgets::toggle::add(settings_tab);
	updates_autocheck_toggle
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically check for updates")
		.on(true)
		.rect(updates_autocheck_caption.rect())
		.rect().snap_to(updates_autocheck_caption.rect(), snap_type::bottom, 0.f);
	updates_autocheck_toggle.events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	auto& updates_autodownload_caption = widgets::label::add(settings_tab);
	updates_autodownload_caption
		.text("Auto-Download")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_autocheck_toggle.rect(), snap_type::bottom, _margin);

	auto& updates_autodownload_toggle = widgets::toggle::add(settings_tab);
	updates_autodownload_toggle
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically download updates")
		.on(true)
		.rect(updates_autodownload_caption.rect())
		.rect().snap_to(updates_autodownload_caption.rect(), snap_type::bottom, 0.f);
	updates_autodownload_toggle.events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	//////////////////// Backup and Restore settings.
	auto& backup_restore_settings_title = widgets::label::add(settings_tab);
	backup_restore_settings_title
		.text("Backup and Restore")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(updates_autodownload_toggle.rect(), snap_type::bottom, 50.f);

	auto& backup_now_button = widgets::button::add(settings_tab);
	backup_now_button
		.text("Backup now")
		.rect().size({ 100.f, 20.f })
		.set(_margin, backup_restore_settings_title.rect().bottom() + _margin, 100.f, 20.f);

	main_tab_pane.selected("Dashboard");

	_page_man.show(_page_name);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///								This function needs a lot of cleaning.
bool main_window::on_dispatch_coupon(const std::vector<table_row>& rows, std::string& error) {
	auto table_view =
		get_table_view(_page_name + "/" + _main_tab_name + "/Coupons/coupons-table");

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
	try
	{
		get_label(_page_name + "/main-tab-pane/Dashboard/top-left-pane/petrol-summary-caption")
			.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

		get_label(_page_name + "/main_tab/Dashboard/left_pane/diesel-summary-caption")
			.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");

		get_table_view(_page_name + "/main_tab/Dashboard/right_pane/fuel-stats-departments")
			.data(_state.get_db().on_update_departmental_stats());
	}
	catch (const std::exception& ex) {
		error = ex.what();
		return false;
	}

	// Updating the coupons table.
	if (is_changed) {
		auto& old_coupons =
			get_table_view(_page_name + "/main_tab/Coupons/coupons_table").data();

		std::vector<table_row> new_coupons;
		new_coupons.reserve(old_coupons.size() - 1);

		for (const auto& row : old_coupons) {
			if (get::text(row.at("Serial Number")) != get::text(serial_number_)) {
				new_coupons.push_back(row);
			}
		}

		old_coupons = new_coupons;
		update();


		get_label(_page_name + "/main_tab/Coupons/number-of-coupons")
			.text(std::to_string(new_coupons.size()) + " coupons available");
	}

	return true;
}

bool main_window::on_add_coupons(std::string& error) {
	std::vector<table_row> saved_coupons;

	addcoupons_form addcoupons("Add Coupons", *this, _state, saved_coupons);
	if (!addcoupons.show(error)) {
		message("Error: " + error);
		return false;
	}

	try {
		// updating dashboard.
		{
			get_label(main_tab_pane_path() + "/Dashboard/top-left-pane/petrol-summary-caption")
				.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

			get_label(main_tab_pane_path() + "/Dashboard/top-left-pane/diesel-summary-caption")
				.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");
		}

		if (!saved_coupons.empty()) {
			for (const auto& row : saved_coupons)
				get_table_view(main_tab_pane_path() + "/Coupons/coupons-table")
					.data().push_back(
						{
							{ "Date", row.at("Date")},
							{ "Volume", row.at("Volume")},
							{ "Serial Number", row.at("Serial Number")},
							{ "Fuel", row.at("Fuel")},
							{ "Issued By", row.at("Issued By")},
						}
					);
		}

		update();

		get_label(main_tab_pane_path () + "/Coupons/number-of-coupons")
			.text(
				std::to_string(get_table_view(main_tab_pane_path() + "/Coupons/coupons-table")
					.data().size()) + " coupons available");
	}
	catch (const std::exception& ex) {
		error = std::string(ex.what());
		return false;
	}

	return true;
}

bool main_window::on_delete_coupon()
{
	return false;
}
