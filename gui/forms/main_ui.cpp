/*
** MIT License
**
** Copyright(c) 2021 Tawanda M. Nyoni
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

// include liblec headers.
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/containers/pane.h>
#include <liblec/lecui/containers/tab_pane.h>

#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/table_view.h>
#include <liblec/lecui/widgets/icon.h>
#include <liblec/lecui/widgets/image_view.h>
#include <liblec/lecui/widgets/toggle.h>
#include <liblec/lecui/widgets/progress_indicator.h>
#include <liblec/lecui/widgets/rectangle.h>
#include <liblec/lecui/widgets/strength_bar.h>

#include <liblec/lecui/menus/context_menu.h>

// include leccore headers
#include <liblec/leccore/app_version_info.h>
#include <liblec/leccore/hash.h>
#include <liblec/leccore/file.h>
#include <liblec/leccore/system.h>
#include <liblec/leccore/zip.h>

// STL
#include <filesystem>
#include <sstream>

#include <liblec/lecui/widgets/date.h>	// had to place this below STL headers because of a macro conflict

// include local headers.
#include "../main_ui.h"
#include "../dispatch.h"
#include "../addcoupons.h"
#include "../../version_info/version_info.h"
#include "../../resource.h"


#ifdef _WIN64
#define architecture	"64bit"
#else
#define architecture	"32bit"
#endif

using namespace liblec;
using snap_type = lecui::rect::snap_type;

// initialise form attributes.
bool main_window::on_initialize(std::string& error) {

	if (!_cleanup_mode)
		get_dpi_scale () < 2.f 
			? _splash_screen.display(splash_image_128, false, error) 
			: _splash_screen.display(splash_image_256, false, error);

	if (_cleanup_mode) {
		if (prompt("Would you like to delete the app settings?")) {
			// cleanup application settings
			if (!_settings.delete_recursive("", error))
				return false;

			// cleanup company settings (will delete the company subkey if no
			// other apps have placed subkeys under it
			leccore::registry reg(leccore::registry::scope::current_user);
			if (!reg.do_delete("Software\\com.github.tmnyoni\\", error)) {}
		}

		close();
		return true;
	}


	std::string value;
	if (!_settings.read_value("", "darktheme", value, error))
		return false;
	else
		_setting_darktheme = value == "on";

	_controls
		.allow_minimize(false)
		.allow_resize(false);

	_appearance
		.theme(_setting_darktheme ? lecui::themes::dark : lecui::themes::light)
		.main_icon(ico_resource)
		.mini_icon(ico_resource)
		.caption_icon(get_dpi_scale() < 2.f ? icon_png_32 : icon_png_64);
	
	_dimensions.set_size(_window_size);

	// schedule timer for dispatched coupons (1500ms kick start ... the method will do the timer looping)
	_timer_man.add("dispatched_coupon_timer", 1500, [&]() { dispatched_coupon_timer(); });

	_splash_screen.remove();

	return true;
}

bool main_window::on_layout(std::string& error) {
	auto& page = _page_manager.add(_page_name);

	auto& main_tab_pane = lecui::containers::tab_pane::add(page, _main_tab_name);
	main_tab_pane
		.border(1.f)
		.tabs_border(0.f)
		.color_tabs(lecui::color().alpha(0))
		.corner_radius_x(0).corner_radius_y(0)
		.tab_side(lecui::containers::tab_pane::side::left)
		.caption_reserve({ "Dashboard", "Coupons", "Reports", "Settings" })
		.rect().set(_margin, 0, page.size().get_width() - (_margin * 2), page.size().get_height() - _margin);

	/*
	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Dashboard.
	auto& dashboard_tab = lecui::containers::tab::add(main_tab_pane, "Dashboard");

	////////// Top Left Pane
	auto& top_left_pane = lecui::containers::pane::add(dashboard_tab, "top-left-pane");
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
		auto& consumption_sumary_title = lecui::widgets::label::add(top_left_pane);
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
		auto& petrol_summary_title = lecui::widgets::label::add(top_left_pane);
		petrol_summary_title
			.text("Petrol")
			.center_h(true)
			.color_text(_caption_color)
			.rect(rect()
				.left(_margin)
				.top(consumption_sumary_title.rect().bottom() + _margin * 2.f)
				.width((top_left_pane.size().get_width() / 2.f) - _margin / 2.f)
				.height(20.f));

		auto& petrol_summary_progress = lecui::widgets::progress_indicator::add(top_left_pane, "petrol-summary-progress");
		petrol_summary_progress
			.percentage(50.f)
			.rect().size(80.f, 80.f)
			.snap_to(petrol_summary_title.rect(), snap_type::bottom, _margin);

		auto& petrol_summary_caption = lecui::widgets::label::add(top_left_pane, "petrol-summary-caption");
		petrol_summary_caption
			.text(std::to_string(_state.get_db().on_get_petrol_volume()) + +"<span style= 'color: green;'>" + " Litres  </span> ")
			.center_h(true)
			.rect(rect()
				.left(_margin)
				.top(petrol_summary_progress.rect().bottom() + _margin)
				.width(top_left_pane.size().get_width() / 2.f)
				.height(20.f));

		// diesel summary.
		auto& diesel_summary_title = lecui::widgets::label::add(top_left_pane);
		diesel_summary_title
			.text("Diesel")
			.center_h(true)
			.color_text(_caption_color)
			.rect(petrol_summary_title.rect())
			.rect().snap_to(petrol_summary_title.rect(), snap_type::right, _margin);

		auto& diesel_summary_progress = lecui::widgets::progress_indicator::add(top_left_pane, "diesel-summary-progress");
		diesel_summary_progress
			.percentage(50.f)
			.rect(petrol_summary_progress.rect())
			.rect().snap_to(diesel_summary_title.rect(), snap_type::bottom, _margin);

		auto& diesel_summary_caption = lecui::widgets::label::add(top_left_pane, "diesel-summary-caption");
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
	auto& top_right_pane = lecui::containers::pane::add(dashboard_tab, "top-right-pane");
	top_right_pane
		.border(0.f)
		.corner_radius_x(0.f)
		.corner_radius_y(0.f)
		.rect(top_left_pane.rect())
		.rect().snap_to(top_left_pane.rect(), snap_type::right, _margin);

	auto& department_consumption_summary_title = lecui::widgets::label::add(top_right_pane);
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
			lecui::widgets::table_view::add(top_right_pane, "departmental-consumpt-stats-table");

		std::vector<table_column> departmental_consumpt_stats_columns =
		{ { "Department", 150 }, { "Volume", 85 }, };

		std::vector<database::row> departments_stats;
		if (!_state.get_db().on_departments_stats(departments_stats, error)) {
			message(error);
		}

		departmental_consumpt_stats_table
			.border(0.f)
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.color_fill(color().alpha(0))
			.user_sort(true)
			.columns(departmental_consumpt_stats_columns)
			.data(departments_stats)
			.rect(rect()
				.left(_margin)
				.right(top_right_pane.size().get_width() - _margin)
				.top(department_consumption_summary_title.rect().bottom() + _margin)
				.bottom(280.f));
	}
	catch (const std::exception& ex) {
		message("Error: " + std::string(ex.what()));
	}
	*/

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Coupons tab.

	auto& coupons_tab = lecui::containers::tab::add(main_tab_pane, "Coupons");

	auto& add_coupons_button = lecui::widgets::icon::add(coupons_tab);
	add_coupons_button
		.text("Add Coupons")
		.font_size(9.f)
		.png_resource(png_add_coupon)
		.max_image_size(20.f)
		.rect(lecui::rect()
			.left(_margin * 1.5f)
			.top(_margin / 4.f)
			.width(_margin * 12.f)
			.height(_margin * 3.f))
		.events().action = [&]() { on_add_coupons(error); };

	const float dispatched_coupons_width = 180.f;

	// add coupons table caption
	auto& coupons_table_caption = lecui::widgets::label::add(coupons_tab);
	coupons_table_caption
		.text("<strong>Available</strong>")
		.font_size(10.f)
		.rect(lecui::rect()
			.left(_margin)
			.right(coupons_tab.size().get_width() - _margin - dispatched_coupons_width - _margin)
			.top(add_coupons_button.rect().bottom() + _margin)
			.height(coupons_table_caption.rect().height()));

	auto& coupons_table = lecui::widgets::table_view::add(coupons_tab, "coupons-table");
	{
		std::vector<lecui::table_column> coupons_table_cols =
		{
			{ "Serial Number", 100 },
			{ "Fuel", 50 },
			{ "Volume", 60, 0 /* number of decimal places */ },
			{ "Date", 120 },
			{ "Issued By", 110 },
		};

		std::vector<database::row> coupons_data;
		if (!_state.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		std::vector<database::row> table_data;

		for (const auto& row : coupons_data) {

			database::row table_row;
			for (const auto& [column_name, data] : row) {
				if (column_name == "Date") {
					// get the time_t value
					long long time = static_cast<long long>(db_get::real(data));

					// convert to a std::tm, local time
					std::tm tm = { };
					localtime_s(&tm, &time);

					// convert to a string
					std::stringstream ss;
					ss << std::put_time(&tm, "%d %b %Y, %H:%M");
					std::string date_string = ss.str();

					// insert the string into the table row instead of the time_t value
					table_row.insert(std::make_pair(column_name, date_string));
				}
				else
					table_row.insert(std::make_pair(column_name, data));
			}

			table_data.push_back(table_row);
		}

		coupons_table
			.border(1.f)
			.color_border(lecui::defaults::color(_setting_darktheme ? lecui::themes::dark : lecui::themes::light, lecui::item::pane_border))
			.fixed_number_column(true)
			.corner_radius_x(0.f)
			.corner_radius_y(0.f)
			.user_sort(true)
			.columns(coupons_table_cols)
			.data(table_data)
			.rect(lecui::rect()
				.left(_margin)
				.right(coupons_tab.size().get_width() - _margin - dispatched_coupons_width - _margin)
				.top(coupons_table_caption.rect().bottom())
				.bottom(coupons_tab.size().get_height() - (_margin * _margin)))
			.events().context_menu = [&]
			(const std::vector<table_row>& rows) {

			context_menu::specs context_menu_specs;
			context_menu_specs.items = {
				{"Dispatch", png_dispatch_coupon },
				{"Return",  png_return_coupon },
				{"Delete",  png_delete_coupon } };

			auto selected_context_menu_item = lecui::context_menu()(*this, context_menu_specs);

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

	auto& total_coupons_caption = lecui::widgets::label::add(coupons_tab, "number-of-coupons");
	total_coupons_caption
		.text(std::to_string(coupons_table.data().size()) + " coupon" + (coupons_table.data().size() == 1 ? std::string() : std::string("s")) + " available")
		.color_text(_caption_color)
		.rect().set(_margin * 1.5f, coupons_table.rect().bottom() + _margin / 3.f, dispatched_coupons_width, 20.f);

	// add dispatched coupons pane caption
	auto& dispatched_coupons_pane_caption = lecui::widgets::label::add(coupons_tab);
	dispatched_coupons_pane_caption
		.text("<strong>Dispatched</strong>")
		.font_size(10.f)
		.rect(lecui::rect(coupons_table_caption.rect())
			.left(coupons_table.rect().right() + _margin)
			.right(coupons_tab.size().get_width() - _margin));

	// add dispatched coupons pane
	auto& dispatched_coupons_pane = lecui::containers::pane::add(coupons_tab, "dispatched-coupons");
	dispatched_coupons_pane
		.border(1.f)
		.rect(lecui::rect(coupons_table.rect())
			.left(coupons_table.rect().right() + _margin)
			.right(coupons_tab.size().get_width() - _margin))
		.color_fill(lecui::defaults::color(_setting_darktheme ? lecui::themes::dark : lecui::themes::light, lecui::item::table_view));

	auto& total_dispatched_caption = lecui::widgets::label::add(coupons_tab, "number-of-dispatched-coupons");
	total_dispatched_caption
		.color_text(_caption_color)
		.rect(lecui::rect(total_coupons_caption.rect()).left(dispatched_coupons_pane.rect().left()).right(dispatched_coupons_pane.rect().right()));


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Reports
	auto& reports_tab = lecui::containers::tab::add(main_tab_pane, "Reports");

	auto& start_date_caption = lecui::widgets::label::add(reports_tab);
	start_date_caption
		.text("Start Date")
		.color_text(_caption_color)
		.rect(lecui::rect()
			.left(_margin * 2.f)
			.top(_margin)
			.width(200.f)
			.height(20.f));

	auto& start_date = lecui::widgets::date::add(reports_tab, "start_date");
	start_date
		.rect(lecui::rect(start_date.rect()).snap_to(start_date_caption.rect(), snap_type::bottom_left, _margin))
		.events().change = [this](const lecui::date& dt) {
		run_report();
	};


	auto& end_date_caption = lecui::widgets::label::add(reports_tab);
	end_date_caption
		.text("End Date")
		.color_text(_caption_color)
		.rect(lecui::rect(start_date_caption.rect()).snap_to(start_date_caption.rect(), snap_type::right, _margin));
	
	auto& end_date = lecui::widgets::date::add(reports_tab, "end_date");
	end_date
		.rect(lecui::rect(start_date.rect()).snap_to(end_date_caption.rect(), snap_type::bottom_left, _margin))
		.events().change = [this](const lecui::date& dt) {
		run_report();
	};


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Settings
	auto& settings_tab = lecui::containers::tab::add(main_tab_pane, "Settings");

	//// Appearance settings.
	auto& appearance_settings_title = lecui::widgets::label::add(settings_tab);
	appearance_settings_title
		.text("Appearance")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.set(_margin, _margin, 100, 25);

	auto& theme_settings_caption = lecui::widgets::label::add(settings_tab);
	theme_settings_caption
		.text("Theme")
		.rect().size({ 200.f, 20.f })
		.set(_margin, appearance_settings_title.rect().bottom() + _margin, 200.f, 20.f);

	auto& theme_setting_select = lecui::widgets::combobox::add(settings_tab, "theme-settings-theme");
	{
		std::vector<lecui::widgets::combobox::combobox_item> themes =
		{ { "light" }, { "dark" } };

		theme_setting_select
			.items(themes)
			.color_fill({ 255,255,255,0 })
			.selected(_setting_darktheme ? std::string("dark") : std::string("light"))
			.rect().size(200.f, 25.f)
			.snap_to(theme_settings_caption.rect(), snap_type::bottom, 0.f);
		theme_setting_select.events().selection = [this](const std::string& selected) {
			std::string error;

			bool new_darktheme_setting = false;

			if (selected == "dark") {
				if (_settings.write_value("", "darktheme", "on", error))
					new_darktheme_setting = true;
			}
			else {
				if (_settings.write_value("", "darktheme", "off", error))
					new_darktheme_setting = false;
			}

			if (new_darktheme_setting != _setting_darktheme) {
				if (prompt("Would you like to restart the app now for the changes to take effect?")) {
					_restart_now = true;
					close();
				}
			}
		};
	}

	//// Updates settings.
	auto& updates_settings_title = lecui::widgets::label::add(settings_tab);
	updates_settings_title
		.text("Updates")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(theme_setting_select.rect(), snap_type::bottom, 50.f);

	// update auto check.
	auto& updates_autocheck_caption = lecui::widgets::label::add(settings_tab);
	updates_autocheck_caption
		.text("Auto-check")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_settings_title.rect(), snap_type::bottom, _margin);

	auto& updates_autocheck_toggle = lecui::widgets::toggle::add(settings_tab);
	updates_autocheck_toggle
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically check for updates")
		.on(true)
		.rect(updates_autocheck_caption.rect())
		.rect().snap_to(updates_autocheck_caption.rect(), snap_type::bottom, 0.f);
	updates_autocheck_toggle.events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	auto& updates_autodownload_caption = lecui::widgets::label::add(settings_tab);
	updates_autodownload_caption
		.text("Auto-Download")
		.rect().size({ 200.f, 20.f })
		.snap_to(updates_autocheck_toggle.rect(), snap_type::bottom, _margin);

	auto& updates_autodownload_toggle = lecui::widgets::toggle::add(settings_tab);
	updates_autodownload_toggle
		.text("Yes")
		.text_off("No")
		.tooltip("Select whether to automatically download updates")
		.on(true)
		.rect(updates_autodownload_caption.rect())
		.rect().snap_to(updates_autodownload_caption.rect(), snap_type::bottom, 0.f);
	updates_autodownload_toggle.events().toggle = [&](bool on) { /*on_autocheck_updates(on); */};

	//////////////////// Backup and Restore settings.
	auto& backup_restore_settings_title = lecui::widgets::label::add(settings_tab);
	backup_restore_settings_title
		.text("Backup and Restore")
		.color_text(_caption_color)
		.font_size(12.f)
		.rect().size({ 200.f, 25.f })
		.snap_to(updates_autodownload_toggle.rect(), snap_type::bottom, 50.f);

	auto& backup_now_button = lecui::widgets::button::add(settings_tab);
	backup_now_button
		.text("Backup now")
		.rect().size({ 100.f, 20.f })
		.set(_margin, backup_restore_settings_title.rect().bottom() + _margin, 100.f, 20.f);

	main_tab_pane.selected("Coupons");

	_page_manager.show(_page_name);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///								This function needs a lot of cleaning.
bool main_window::on_dispatch_coupon(const std::vector<table_row>& rows, std::string& error) {
	auto table_view =
		get_table_view(_main_tab_pane_path + "/Coupons/coupons-table");

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
	if (!dispatch_form_.create(error)) {
		return false;
	}

	//// Updating dashboard.
	//try
	//{
	//	get_label(_main_tab_pane_path + "/Dashboard/top-left-pane/petrol-summary-caption")
	//		.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

	//	get_label(_main_tab_pane_path + "/Dashboard/top-left-pane/diesel-summary-caption")
	//		.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");

	//	//get_table_view(_main_tab_pane_path + "/Dashboard/top-right-pane/departmental-consumpt-stats-table")
	//	//	.data(_state.get_db().on_update_departmental_stats());
	//}
	//catch (const std::exception& ex) {
	//	error = ex.what();
	//	return false;
	//}

	// Updating the coupons table.
	if (is_changed) {
		auto& old_coupons =
			get_table_view(_main_tab_pane_path + "/Coupons/coupons-table").data();

		std::vector<table_row> new_coupons;
		new_coupons.reserve(old_coupons.size() - 1);

		for (const auto& row : old_coupons) {
			if (get::text(row.at("Serial Number")) != get::text(serial_number_)) {
				new_coupons.push_back(row);
			}
		}

		old_coupons = new_coupons;
		update();


		get_label(_main_tab_pane_path +  "/Coupons/number-of-coupons")
			.text(std::to_string(new_coupons.size()) + " coupon" + (new_coupons.size() == 1 ? std::string() : std::string("s")) + " available");
	}

	return true;
}

bool main_window::on_add_coupons(std::string& error) {
	std::vector<table_row> saved_coupons;

	addcoupons_form addcoupons("Add Coupons", *this, _state, saved_coupons);
	if (!addcoupons.create(error)) {
		message("Error: " + error);
		return false;
	}

	try {
		//// updating dashboard.
		//{
		//	get_label(_main_tab_pane_path + "/Dashboard/top-left-pane/petrol-summary-caption")
		//		.text(std::to_string(_state.get_db().on_get_petrol_volume()) + " Litres");

		//	get_label(_main_tab_pane_path + "/Dashboard/top-left-pane/diesel-summary-caption")
		//		.text(std::to_string(_state.get_db().on_get_diesel_volume()) + " Litres");
		//}

		if (!saved_coupons.empty()) {
			for (const auto& row : saved_coupons) {
				// get the time_t value
				long long time = static_cast<long long>(db_get::real(row.at("Date")));

				// convert to a std::tm, local time
				std::tm tm = { };
				localtime_s(&tm, &time);

				// convert to a string
				std::stringstream ss;
				ss << std::put_time(&tm, "%d %b %Y, %H:%M");
				std::string date_string = ss.str();

				get_table_view(_main_tab_pane_path + "/Coupons/coupons-table")
					.data().push_back(
						{
							{ "Date", date_string },
							{ "Volume", row.at("Volume") },
							{ "Serial Number", row.at("Serial Number") },
							{ "Fuel", row.at("Fuel") },
							{ "Issued By", row.at("Issued By") },
						}
				);
			}
		}

		update();

		auto& coupons_table = get_table_view(_main_tab_pane_path + "/Coupons/coupons-table");

		get_label(_main_tab_pane_path + "/Coupons/number-of-coupons")
			.text(
				std::to_string(coupons_table
					.data().size()) + " coupon" + (coupons_table.data().size() == 1 ? std::string() : std::string("s")) + " available");
	}
	catch (const std::exception& ex) {
		error = std::string(ex.what());
		return false;
	}

	return true;
}

bool main_window::on_delete_coupon(std::string& error) {
	// function not finished.

	auto& coupons_table =
		get_table_view(_page_name + "/main_tab/Coupons/coupons_table");

	auto& old_coupons = coupons_table.data();
	auto& selected_coupon = coupons_table.selected();

	std::vector<table_row> new_coupons;
	new_coupons.reserve(old_coupons.size() - 1);

	old_coupons = new_coupons;
	update();


	get_label(_page_name + "/main_tab/Coupons/number-of-coupons")
		.text(std::to_string(new_coupons.size()) + " coupon" + (new_coupons.size() == 1 ? std::string() : std::string("s")) + " available");

	auto table_view =
		get_table_view(_page_name + "/" + _main_tab_name + "/Coupons/coupons-table");


	return true;
}

void main_window::dispatched_coupon_timer() {
	// stop the timer
	_timer_man.stop("dispatched_coupon_timer");

	std::string error;
	std::vector<fuelman_db::dispatched_coupon_struct> coupons_data;
	if (_state.get_db().on_get_dispatched_coupons(coupons_data, error)) {
		try {
			auto& dispatched_coupons_pane = get_pane(_main_tab_pane_path + "/Coupons/dispatched-coupons");
			auto& total_dispatched_caption = get_label(_main_tab_pane_path + "/Coupons/number-of-dispatched-coupons");

			const auto ref_rect = lecui::rect(dispatched_coupons_pane.size());

			float bottom_margin = 0.f;

			for (const auto& coupon : coupons_data) {
				// get the time_t value
				long long time = static_cast<long long>(coupon.date);

				// convert to a std::tm, local time
				std::tm tm = { };
				localtime_s(&tm, &time);

				// convert to a string
				std::stringstream ss;
				ss << std::put_time(&tm, "%d %b %Y, %H:%M");
				std::string date_string = ss.str();

				const std::string serial_number = coupon.serial_number;
				const std::string fuel = coupon.fuel;
				const std::string volume = leccore::round_off::to_string(coupon.volume, 0);
				const std::string issued_to = coupon.receiving_department;

				auto& coupon_pane = lecui::containers::pane::add(dispatched_coupons_pane, serial_number);
				coupon_pane
					.rect(lecui::rect(ref_rect)
						.top(bottom_margin)
						.height(80.f))
					.color_fill(_setting_darktheme ?
						lecui::color().red(35).green(45).blue(60) :
						lecui::color().red(245).green(245).blue(245));

				auto& volume_label = lecui::widgets::label::add(coupon_pane, serial_number + "_volume");
				volume_label
					.text("<strong>" + volume + "</strong> litres of <span style = 'font-size: 8.0pt;'><strong>" + fuel + "</strong> issued to</span>")
					.rect(volume_label.rect()
						.right(coupon_pane.size().get_width()));

				auto& issued_to_label = lecui::widgets::label::add(coupon_pane, serial_number + "_issued_to");
				issued_to_label
					.text("<span style = 'font-size: 8.0pt;'><strong><em>" + issued_to + "</em></strong></span>")
					.rect(lecui::rect(volume_label.rect())
						.snap_to(volume_label.rect(), snap_type::bottom, 0.f));

				auto& date_label = lecui::widgets::label::add(coupon_pane, serial_number + "_date");
				date_label
					.text(date_string)
					.rect(lecui::rect(volume_label.rect())
						.snap_to(issued_to_label.rect(), snap_type::bottom, 0.f));

				// add color rect to distinguish between petrol and diesel
				auto& color_rect = lecui::widgets::rectangle::add(dispatched_coupons_pane, serial_number + "_color_rect");
				color_rect
					.rect(lecui::rect(coupon_pane.rect()).width(2.f).top(coupon_pane.rect().top() + 10.f).bottom(coupon_pane.rect().bottom() - 10.f));
				color_rect
					.color_border().alpha(0);
				color_rect
					.color_fill(fuel == "Petrol" ? _petrol_color : _diesel_color);

				// update bottom margin
				bottom_margin = coupon_pane.rect().bottom() + _margin;
			}

			total_dispatched_caption.text(std::to_string(coupons_data.size()) + " coupon" +
				(coupons_data.size() == 1 ? std::string() : std::string("s")) + " dispatched");

			update();
		}
		catch (const std::exception&) {}
	}

	// resume the timer (1200ms looping ...)
	_timer_man.add("dispatched_coupon_timer", 1200, [&]() {
		dispatched_coupon_timer();
		});
}

void main_window::run_report() {
	try {
		auto& reports_tab = get_tab(_main_tab_pane_path + "/Reports");
		auto& start_date = get_date(_main_tab_pane_path + "/Reports/start_date");
		auto& end_date = get_date(_main_tab_pane_path + "/Reports/end_date");

		// to-do: filter by dates ... for now get for all time

		// get values
		auto available_petrol = _state.get_db().available_petrol();
		auto available_diesel = _state.get_db().available_diesel();
		auto total_available = available_petrol + available_diesel;

		auto dispatched_petrol = _state.get_db().dispatched_petrol();
		auto dispatched_diesel = _state.get_db().dispatched_diesel();
		auto total_dispatched = dispatched_petrol + dispatched_diesel;

		auto total_petrol = available_petrol + dispatched_petrol;
		auto total_diesel = available_diesel + dispatched_diesel;
		auto total_fuel = total_petrol + total_diesel;

		if ((total_petrol + total_diesel) == 0) {
			message("No data available");
			return;
		}

		{
			// available petrol
			auto& available_petrol_pane = lecui::containers::pane::add(reports_tab);
			available_petrol_pane
				.rect(lecui::rect(lecui::size(200.f, 100.f)).snap_to(start_date.rect(), snap_type::bottom_left, 2.f * _margin));

			{
				float percentage = 0.f;

				if (total_available > 0)
					percentage = static_cast<float>(100.0 * available_petrol / total_available);

				auto& label = lecui::widgets::label::add(available_petrol_pane);
				label
					.text("AVAILABLE PETROL RATIO")
					.alignment(lecui::text_alignment::center)
					.rect().width(available_petrol_pane.size().get_width());

				auto& indicator = lecui::widgets::progress_indicator::add(available_petrol_pane);
				indicator
					.percentage(percentage)
					.color_fill(_petrol_color)
					.color_text(_petrol_color)
					.rect().snap_to(label.rect(), snap_type::bottom, _margin);
			}

			// available diesel
			auto& available_diesel_pane = lecui::containers::pane::add(reports_tab);
			available_diesel_pane
				.rect(lecui::rect(available_petrol_pane.rect()).snap_to(available_petrol_pane.rect(), snap_type::right, _margin));

			{
				float percentage = 0.f;

				if (total_available > 0)
					percentage = static_cast<float>(100.0 * available_diesel / total_available);

				auto& label = lecui::widgets::label::add(available_diesel_pane);
				label
					.text("AVAILABLE DIESEL RATIO")
					.alignment(lecui::text_alignment::center)
					.rect().width(available_diesel_pane.size().get_width());

				auto& indicator = lecui::widgets::progress_indicator::add(available_diesel_pane);
				indicator
					.percentage(percentage)
					.color_fill(_diesel_color)
					.color_text(_diesel_color)
					.rect().snap_to(label.rect(), snap_type::bottom, _margin);
			}


			// dispatched petrol
			auto& dispatched_petrol_pane = lecui::containers::pane::add(reports_tab);
			dispatched_petrol_pane
				.rect(lecui::rect(available_petrol_pane.rect()).snap_to(available_petrol_pane.rect(), snap_type::bottom_left, 2.f * _margin));

			{
				float percentage = 0.f;

				if (total_dispatched > 0)
					percentage = static_cast<float>(100.0 * dispatched_petrol / total_dispatched);

				auto& label = lecui::widgets::label::add(dispatched_petrol_pane);
				label
					.text("DISPATCHED PETROL RATIO")
					.alignment(lecui::text_alignment::center)
					.rect().width(dispatched_petrol_pane.size().get_width());

				auto& indicator = lecui::widgets::progress_indicator::add(dispatched_petrol_pane);
				indicator
					.percentage(percentage)
					.color_fill(_petrol_color)
					.color_text(_petrol_color)
					.rect().snap_to(label.rect(), snap_type::bottom, _margin);
			}

			// dispatched diesel
			auto& dispatched_diesel_pane = lecui::containers::pane::add(reports_tab);
			dispatched_diesel_pane
				.rect(lecui::rect(dispatched_petrol_pane.rect()).snap_to(dispatched_petrol_pane.rect(), snap_type::right, _margin));

			{
				float percentage = 0.f;

				if (total_dispatched > 0)
					percentage = static_cast<float>(100.0 * dispatched_diesel / total_dispatched);

				auto& label = lecui::widgets::label::add(dispatched_diesel_pane);
				label
					.text("DISPATCHED DIESEL RATIO")
					.alignment(lecui::text_alignment::center)
					.rect().width(dispatched_diesel_pane.size().get_width());

				auto& indicator = lecui::widgets::progress_indicator::add(dispatched_diesel_pane);
				indicator
					.percentage(percentage)
					.color_fill(_diesel_color)
					.color_text(_diesel_color)
					.rect().snap_to(label.rect(), snap_type::bottom, _margin);
			}

			// overall ratio between petrol and diesel
			auto& petrol = lecui::widgets::label::add(reports_tab);
			petrol
				.text("TOTAL PETROL")
				.rect().snap_to(dispatched_petrol_pane.rect(), snap_type::bottom_left, 3.f * _margin);

			auto& diesel = lecui::widgets::label::add(reports_tab);
			diesel
				.text("TOTAL DIESEL")
				.alignment(lecui::text_alignment::right)
				.rect().snap_to(dispatched_diesel_pane.rect(), snap_type::bottom_right, 3.f * _margin);

			float petrol_ratio = 0.f;

			if (total_fuel > 0)
				petrol_ratio = static_cast<float>(100.0 * total_petrol / total_fuel);

			liblec::lecui::widgets::strength_bar::strength_level petrol_level;
			petrol_level.level = petrol_ratio;
			petrol_level.color = _petrol_color;

			liblec::lecui::widgets::strength_bar::strength_level diesel_level;
			diesel_level.level = 100.f;
			diesel_level.color = _diesel_color;

			auto& ratio = lecui::widgets::strength_bar::add(reports_tab);
			ratio
				.levels({ petrol_level, diesel_level })
				.percentage(100.f)
				.rect(lecui::rect(ratio.rect()).left(petrol.rect().left()).right(diesel.rect().right()).snap_to(petrol.rect(), snap_type::bottom_left, _margin));
		

			// add icons
			const lecui::size icon_size{ 80.f, 30.f };
			auto& print_button = lecui::widgets::icon::add(reports_tab);
			print_button
				.text("Print")
				.font_size(9.f)
				.png_resource(png_print_report)
				.max_image_size(20.f)
				.rect(rect()
					.left(ratio.rect().left())
					.top(ratio.rect().bottom() + 2.f * _margin)
					.width(icon_size.get_width())
					.height(icon_size.get_height()))
				.events().action = [&]() {};

			auto& share_button = lecui::widgets::icon::add(reports_tab);
			share_button
				.text("Share")
				.font_size(9.f)
				.png_resource(png_share_report)
				.max_image_size(20.f)
				.rect(print_button.rect())
				.rect().snap_to(print_button.rect(), snap_type::right, _margin / 3.f);
			share_button.events().action = [&]() {
				// share to excel

			};

			auto& preview_button = lecui::widgets::icon::add(reports_tab);
			preview_button
				.text("Preview")
				.font_size(9.f)
				.png_resource(png_preview_report)
				.max_image_size(20.f)
				.rect(print_button.rect())
				.rect().snap_to(share_button.rect(), snap_type::right, _margin / 3.f);
			preview_button.events().action = [&]() {};
		}

	}
	catch (const std::exception&) {

	}
}

main_window::main_window(const std::string& caption,
	state& app_state) :
	_cleanup_mode(leccore::commandline_arguments::contains("/cleanup")),
	lecui::form(caption),
	_state(app_state),
	_settings(_registry_settings) {
	
	_registry_settings.set_registry_path("Software\\com.github.tmnyoni\\" + std::string(appname));


	if (_cleanup_mode)
		force_instance();
}
