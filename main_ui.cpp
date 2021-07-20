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
#include "dispatch.h"
#include "addcoupons.h"
#include "edit_coupon.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

bool dashboard::on_initialize(std::string& error) {
	controls_.allow_minimize(true);
	controls_.allow_resize(false);
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
		//Todo.tab_side(containers::tab_pane_specs::side::left)
		.rect().set(margin_, 0, page.size().width - (margin_ * 2), page.size().height - margin_);

	// dashboard.
	containers::tab_builder dashboard_tab(tabs, "dashboard");

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

	int petrol_volume_ = 0;
	if (!state_.get_db().on_get_petrol_volume(petrol_volume_, error))
		message("Error: " + error);
	
	widgets::label_builder total_petrol_details (top_left_pane.get(), "petrol_details");
	total_petrol_details().text(std::to_string(petrol_volume_) + "  Litres");
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

	int diesel_volume_ = 0;
	if (!state_.get_db().on_get_diesel_volume(diesel_volume_, error))
		message("Error: " + error);

	widgets::label_builder total_diesel_details(top_left_pane.get(), "diesel_details");
	total_diesel_details().text(std::to_string(diesel_volume_) + "<span style = 'font-size: 10.0pt;'>" + " Litres  </span> ");
	total_diesel_details()
		.color_fill(rgba(32, 34, 244, 0))
		.rect().size({ 100, 20 })
		.snap_to(total_diesel_caption().rect(), snap_type::bottom, 2.f);

	containers::pane_builder right_pane(dashboard_tab.get(), "right_pane");
	right_pane()
		.border(1.f)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 0, 0, 0))
		.on_resize({ 0.f, 0.f, 0.f, 100.f })
		.rect().size((dashboard_tab.get().size().width / 2) - margin_, (dashboard_tab.get().size().height / 2) - margin_)
		.snap_to(top_left_pane().rect(), snap_type::right, margin_);


	///////////////////////////////////////////////////////////////////////////////////////////////
											// Coupons tab.

	containers::tab_builder coupons_tab(tabs, "coupons");

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
			{ "#", 50 },
			{ "Serial Number", 150 },
			{ "Fuel", 90 },
			{ "Volume", 80 },
			{ "Date", 90 },
		};

		if (!state_.get_db().on_get_coupons(coupons_data, error))
			message("Error: " + error);

		for (int i = 1; auto & row : coupons_data) {
			row.insert(std::make_pair("#", i++));
		}

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
				margin_ * 3,
				tabs().rect().width() / 2.f - (margin_ * 2.f),
				tabs().rect().height() - (margin_ * 9)
			);
		coupons_table().events().selection = [&]
		(const std::vector<table_row>& rows) {
			on_select_coupon(rows);
		};
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
											// Coupons details pane.

	containers::pane_builder coupon_details_pane(coupons_tab.get(), "coupon_details_pane");
	coupon_details_pane()
		.border(1)
		.corner_radius_x(0)
		.corner_radius_y(0)
		.color_fill(rgba(255, 255, 255, 0))
		.on_resize({ 50.f, 0.f, 50.f, 0.f })
		.rect().set(
			coupons_table().rect().right() + margin_,
			coupons_table().rect().top(),
			tabs().rect().width() / 2.f - (margin_ * 2.f),
			tabs().rect().height() - (margin_ * 9)
		);

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

	/// //////////////////////////
	/// Reports
	containers::tab_builder reports_tab(tabs, "reports");
	


	/// //////////////////////////
	/// Settings
	containers::tab_builder settings_tab(tabs, "settings");

	containers::tab_pane_builder settings_tabs(settings_tab.get(), "settings_tabs");
	settings_tabs()
		.border(1)
		.tabs_border(0)
		.color_tabs({ 255, 255, 255, 0 })
		.rect().set(margin_, 0, settings_tab.get().size().width - (margin_ * 2), settings_tab.get().size().height - margin_);

	containers::tab_builder appearance_settings(settings_tabs, "appearance");

	settings_tabs.select("appearance");
	tabs.select("dashboard");

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
		int petrol_volume_ = 0;
		if (!state_.get_db().on_get_petrol_volume(petrol_volume_, error))
			message("Error: " + error);
		widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/petrol_details").text(std::to_string(petrol_volume_) + " Litres");


		int diesel_volume_ = 0;
		if (!state_.get_db().on_get_diesel_volume(diesel_volume_, error))
			message("Error: " + error);
		widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/diesel_details").text(std::to_string(diesel_volume_) + " Litres");
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
			int petrol_volume_ = 0;
			if (!state_.get_db().on_get_diesel_volume(petrol_volume_, error))
				message("Error: " + error);
			widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/petrol_details").text(std::to_string(petrol_volume_) + " Litres");

			int diesel_volume_ = 0;
			if (!state_.get_db().on_get_diesel_volume(diesel_volume_, error))
				message("Error: " + error);
			widgets::label_builder::specs(*this, main_page_name_ + "/main_tab/dashboard/left_pane/diesel_details").text(std::to_string(diesel_volume_) + " Litres");
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

