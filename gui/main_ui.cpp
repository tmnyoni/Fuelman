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
	dims_.set_size({ 800, 700 });
	return true;
}

bool dashboard::on_layout(std::string& error) {
	auto& page = page_man_.add(main_page_name_);

	containers::tab_pane_builder tabs(page, "main_tab");
	tabs()
		.border(1)
		.tabs_border(0)
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
		.border(1.f)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 0, 0, 0))
		.on_resize({ 0.f, 0.f, 0.f, 100.f })
		.rect().set(
			margin_ / 2,
			margin_, 
			(dashboard_tab.get().size().width / 2) - margin_,
			(dashboard_tab.get().size().height / 2) - margin_
		);

	widgets::label_builder fuel_volume_caption(top_left_pane.get());
	fuel_volume_caption()
		.text("<strong>Fuel total volume </strong>")
		.font_size(11)
		.color_text(caption_color_)
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.place(
			{
				margin_,
				200,
				margin_,
				20
			}, 0.f, 0.f
		);

	try {
		widgets::label_builder total_petrol_caption(top_left_pane.get());
		total_petrol_caption()
			.text("Petrol")
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ 100, 20 })
			.place(
				{
					margin_,
					80,
					fuel_volume_caption().rect().bottom() + margin_,
					fuel_volume_caption().rect().bottom() + (margin_ * 3)
				}, 0.f, 0.f
			);

		widgets::label_builder total_petrol_details(top_left_pane.get(), "petrol_details");
		total_petrol_details().text(std::to_string(state_.get_db().on_get_petrol_volume()) + "  Litres");
		total_petrol_details()
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ 100, 20 })
			.snap_to(total_petrol_caption().rect(), snap_type::bottom, 2.f);

		widgets::label_builder total_diesel_caption(top_left_pane.get());
		total_diesel_caption()
			.text("Diesel")
			.color_text(caption_color_)
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ 100, 20 })
			.snap_to(total_petrol_details().rect(), snap_type::bottom, margin_);

		widgets::label_builder total_diesel_details(top_left_pane.get(), "diesel_details");
		total_diesel_details().text(std::to_string(state_.get_db().on_get_diesel_volume()) + "<span style = 'font-size: 10.0pt;'>" + " Litres  </span> ");
		total_diesel_details()
			.color_fill(rgba(32, 34, 244, 0))
			.rect().size({ 100, 20 })
			.snap_to(total_diesel_caption().rect(), snap_type::bottom, 2.f);

		///////////////////////// Right pane.
		containers::pane_builder right_pane(dashboard_tab.get(), "right_pane");
		right_pane()
			.border(1.f)
			.corner_radius_x(0)
			.corner_radius_y(0)
			.color_fill(rgba(255, 0, 0, 0))
			.on_resize({ 0.f, 0.f, 0.f, 100.f })
			.rect().size((dashboard_tab.get().size().width / 2) - margin_, (dashboard_tab.get().size().height / 2) - margin_)
			.snap_to(top_left_pane().rect(), snap_type::right, margin_);
	}
	catch(const std::exception& ex){
		message(std::string(ex.what()));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Coupons tab.

	containers::tab_builder coupons_tab(tabs, "Coupons");

	widgets::button_builder add_coupons(coupons_tab.get());
	add_coupons()
		.text("New Coupons")
		.rect().size({ 89, 20 })
		.place(
			{
				margin_,
				margin_ * 10,
				margin_ / 4,
				margin_ * 2
			}, 0.f, 0.f
		);
	add_coupons().events().click = [&]() { on_add_coupons(error);  };

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
			.corner_radius_x(0)
			.corner_radius_y(0)
			.user_sort(true)
			.color_fill(rgba(255, 255, 255, 0))
			.on_resize({ -50.f, 0.f, 50.f, 0.f })
			.columns(coupons_table_cols)
			.data(coupons_data)
			.rect(
				{
					margin_,
					coupons_tab.get().size().width / 2.f - margin_,
					add_coupons().rect().bottom() + margin_,
					coupons_tab.get().size().height - margin_
				});
		coupons_table().events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Coupons details pane.

	containers::pane_builder coupon_details_pane(coupons_tab.get(), "coupon_details_pane");
	coupon_details_pane()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 50.f, 0.f, 50.f, 0.f })
		.rect(
			{
				coupons_table().rect().right() + margin_,
				coupons_tab.get().size().width - margin_,
				coupons_table().rect().top(),
				coupons_table().rect().bottom()
			});

	std::map<std::string, std::any> coupon;
	if (!coupons_data.empty())
		coupon = coupons_data[0]; //data to display.

	using get = database::get;
	widgets::label_builder date_label(coupon_details_pane.get());
	date_label()
		.text("Date")
		.color_text(caption_color_)
		.rect().size({ 200, 20 })
		.place(
			{
				margin_,
				200,
				margin_,
				20
			}, 0.f, 0.f);


	widgets::label_builder date_details(coupon_details_pane.get(), "date_details");
	coupon.empty() ? date_details().text("") :
		date_details().text(get::text(coupon.at("Date")));
	date_details()
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 200, 20 })
		.snap_to(date_label().rect(), snap_type::bottom, 2.f);

	widgets::label_builder coupon_serialno_caption(coupon_details_pane.get());
	coupon_serialno_caption()
		.text("Serial Number")
		.color_text(caption_color_)
		.rect().size({ 200, 20 })
		.snap_to(date_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder coupon_serialno_details(coupon_details_pane.get(), "coupon_serialno_details");
	coupon.empty() ? coupon_serialno_details().text("") :
		coupon_serialno_details().text(get::text(coupon.at("Serial Number")));
	coupon_serialno_details()
		.rect().size({ 200, 20 })
		.snap_to(coupon_serialno_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder volume_issued_caption(coupon_details_pane.get());
	volume_issued_caption()
		.text("Volume")
		.color_text(caption_color_)
		.rect().size({ 200, 20 })
		.snap_to(coupon_serialno_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder volume_details(coupon_details_pane.get(), "volume_details");
	coupon.empty() ? volume_details().text("") :
		volume_details().text(get::text(coupon.at("Volume")) + " Litres");
	volume_details()
		.rect().size({ 200, 20 })
		.snap_to(volume_issued_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder fuel_caption(coupon_details_pane.get());
	fuel_caption()
		.text("Fuel")
		.color_text(caption_color_);
	fuel_caption().rect().size({ 200, 20 })
		.snap_to(volume_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder fuel_details(coupon_details_pane.get(), "fuel_details");
	coupon.empty() ? fuel_details().text("") :
		fuel_details().text(get::text(coupon.at("Fuel")));
	fuel_details()
		.rect().size({ 200, 20 })
		.snap_to(fuel_caption().rect(), snap_type::bottom, 2.f);

	widgets::label_builder issuedby_caption(coupon_details_pane.get());
	issuedby_caption()
		.text("Issued by")
		.color_text(caption_color_)
		.rect().size({ 200, 20 })
		.snap_to(fuel_details().rect(), snap_type::bottom, margin_);

	widgets::label_builder issuedby_details(coupon_details_pane.get(), "issuedby_details");
	coupon.empty() ? issuedby_details().text("") :
		issuedby_details().text(get::text(coupon.at("Issued By")));
	issuedby_details()
		.rect().size({ 200, 20 })
		.snap_to(issuedby_caption().rect(), snap_type::bottom, 2.f);

	widgets::button_builder btn_dispatch_coupon(coupon_details_pane.get());
	btn_dispatch_coupon()
		.text("Dispatch")
		.rect().size({ 80, 20 })
		.place(
			{
				margin_,
				100 + margin_,
				issuedby_details().rect().get_bottom() + margin_,
				issuedby_details().rect().get_bottom() + margin_ * 2
			}, 0.f, 0.f
		);
	btn_dispatch_coupon().events().click = [&]() {
		std::string error_;
		if (!on_dispatch_coupon(error_)){
			message("Error: " + error_);
			return;
		}
	};

	widgets::button_builder btn_return_coupoon(coupon_details_pane.get());
	btn_return_coupoon()
		.text("Return")
		.rect().size({ 80, 20 })
		.snap_to(btn_dispatch_coupon().rect(), snap_type::right, margin_);
	btn_return_coupoon().events().click = [&]() {
		if (prompt("Are you sure you, return?")) {
			return;
		}
	};

	widgets::button_builder btn_delete_coupons_button(coupon_details_pane.get());
	btn_delete_coupons_button()
		.text("Delete")
		.rect().size({ 80, 20 })
		.snap_to(btn_return_coupoon().rect(), snap_type::right, margin_);
	btn_delete_coupons_button().events().click = [&]() {
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
		.rect().size({ 200, 20 })
		.set(margin_, margin_, 200, 20);

	widgets::combobox_builder reports_dates_cbo(reports_tab.get(), "fueltype_cbo");
	{
		std::vector<widgets::combobox_specs::combobox_item> fueltypes = {
			{  "today" }, {"yesterday"}
		};

		reports_dates_cbo()
			.items(fueltypes)
			.color_fill({ 255,255,255,0 })
			.rect().size(200, 25)
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
			{ "Volume", 80 },
		};

		if (!state_.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		reports_items_table()
			.border(0)
			.fixed_number_column(true)
			.corner_radius_x(0)
			.corner_radius_y(0)
			.user_sort(true)
			.color_fill(rgba(255, 255, 255, 0))
			.on_resize({ -50.f, 0.f, 50.f, 0.f })
			.columns(reports_table_columns)
			.data(reports_items_data)
			.rect(
				{
					margin_,
					450,
					reports_dates_cbo().rect().bottom() + margin_,
					400
				});
		reports_items_table().events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};

		widgets::button_builder print_button(reports_tab.get());
		print_button()
			.text("Print")
			.rect().size({ 80, 20 })
			.snap_to(reports_items_table().rect(), snap_type::bottom, margin_);
		print_button().events().click = [&]() {};

		widgets::button_builder share_button(reports_tab.get());
		share_button()
			.text("Share")
			.rect().size({ 80, 20 })
			.snap_to(print_button().rect(), snap_type::right, margin_);
		share_button().events().click = [&]() {};

		widgets::button_builder preview_button(reports_tab.get());
		preview_button()
			.text("Preview")
			.rect().size({ 80, 20 })
			.snap_to(share_button().rect(), snap_type::right, margin_);
		preview_button().events().click = [&]() {};
	}

	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// Settings
	containers::tab_builder settings_tab(tabs, "Settings");

	containers::tab_pane_builder settings_tabs(settings_tab.get(), "settings_tabs");
	settings_tabs()
		.border(1)
		.tabs_border(0)
		.color_tabs({ 255, 255, 255, 0 })
		.rect().set(margin_, 0, settings_tab.get().size().width - (margin_ * 2), settings_tab.get().size().height - margin_);

	containers::tab_builder appearance_settings(settings_tabs, "Appearance");

	settings_tabs.select("Appearance");
	tabs.select("Dashboard");

	page_man_.show(main_page_name_);
	return true;
}

bool dashboard::on_select_coupon(
	const std::vector<table_row>& rows)
{
	try
	{
		std::string error;
		auto serial_number = rows[0].at("Serial Number");
		std::vector<database::row>  data_to_display;

		if (!state_.get_db().on_get_coupon(database::get::text(serial_number), data_to_display, error)) {
			message("Error: " + error);	//todo: remove this line.
			return false;
		}

		auto coupons_details_pane_alias = main_page_name_ + "/main_tab/coupons/coupon_details_pane/";

		for (auto row : data_to_display) {
			widgets::label_builder::specs(*this, coupons_details_pane_alias + "date_details")
				.text(database::get::text(row.at("Date")));

			widgets::label_builder::specs(*this, coupons_details_pane_alias + "coupon_serialno_details")
				.text(database::get::text(row.at("Serial Number")));

			widgets::label_builder::specs(*this, coupons_details_pane_alias + "volume_details")
				.text(database::get::text(row.at("Volume")) + " Litres");

			widgets::label_builder::specs(*this, coupons_details_pane_alias + "fuel_details")
				.text(database::get::text(row.at("Fuel")));

			widgets::label_builder::specs(*this, coupons_details_pane_alias + "issuedby_details")
				.text(database::get::text(row.at("Issued By")));

			return true;
		}
		return true;
	}
	catch (const std::exception& ex)
	{
		message("Error: " + std::string(ex.what()));
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///								This function needs a lot of cleaning.
bool dashboard::on_dispatch_coupon(std::string& error)
{
	auto table_view =
		widgets::table_view_builder::specs(*this, main_page_name_ + "/main_tab/coupons/coupons_table");

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
		widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/petrol_details")
			.text(std::to_string(state_.get_db().on_get_petrol_volume()) + " Litres");

		widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/diesel_details")
			.text(std::to_string(state_.get_db().on_get_diesel_volume()) + " Litres");
	}

	// Updating the coupons table.
	if (is_changed) { 
		auto& old_coupons =
			widgets::table_view_builder::specs(*this, main_page_name_ + "/main_tab/coupons/coupons_table").data();

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

	try
	{
		// updating dashboard.
		{
			widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/petrol_details")
				.text(std::to_string(state_.get_db().on_get_petrol_volume()) + " Litres");
			
			widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/diesel_details")
				.text(std::to_string(state_.get_db().on_get_diesel_volume()) + " Litres");
		}

		auto coupons_table =
			widgets::table_view_builder::specs(*this, main_page_name_ + "/main_tab/coupons/coupons_table");

		auto table_size = coupons_table.data().size();

		if (!saved_coupons.empty()) {
			for (int i = 1; const auto & row : saved_coupons) {
				widgets::table_view_builder::specs(*this, main_page_name_ + "/main_tab/coupons/coupons_table")
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
	catch (const std::exception& ex)
	{
		error = std::string(ex.what());
		return false;
	}

	return true;
}

color dashboard::rgba(const unsigned short& r,
	const unsigned short& g,
	const unsigned short& b,
	const unsigned short& a)
{
	return color({ r, g, b, a });
}

