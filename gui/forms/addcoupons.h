#pragma once
#include <iostream>
#include <any>
#include <algorithm>

#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>
#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/password_field.h>
#include <liblec/lecui/widgets/combobox.h>
#include <liblec/lecui/widgets/image_view.h>

#include <liblec/lecui/utilities/date_time.h>

#include "../../appstate/app_state.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

class addcopoupons_form : public form {
	const float margin_ = 10.f;
	const std::string page_name_ = "add_coupons_page";

	state& state_;
	std::vector<table_row>& saved_coupons_;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_manager page_man{ *this };
	widget_manager widget_man{ *this };

	bool on_initialize(std::string& error) override {
		controls_.allow_minimize(false);
		controls_.allow_resize(false);
		dims_.set_size({ 500, 600 });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = page_man.add(page_name_);

		widgets::label_builder fueltype_caption(page);
		fueltype_caption()
			.text("Fuel")
			.rect().place(
				{
					margin_,
					page.size().width - margin_,
					margin_,
					margin_ + (dims_.get_size().height / 2.f)
				}, 50.f, 0.f
			);

		widgets::combobox_builder fueltype_cbo(page, "fueltype_cbo");
		{
			std::vector<widgets::combobox_specs::combobox_item> fueltypes = {
				{  "Petrol" }, {"Diesel"}
			};

			fueltype_cbo()
				.items(fueltypes)
				.color_fill({ 255,255,255,0 })
				.rect().size(200, 25)
				.snap_to(fueltype_caption().rect(), snap_type::bottom, 0);
			fueltype_cbo().events().selection = [](const std::string& selected) {};
		}

		widgets::label_builder couponno_caption(page);
		couponno_caption()
			.text("Serial Number")
			.rect().snap_to(fueltype_cbo().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder couponno_text(page, "couponno_text");
		couponno_text()
			.rect().snap_to(couponno_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder volume_caption(page);
		volume_caption()
			.text("Volume")
			.rect().snap_to(couponno_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder volume_text(page, "volume_text");
		volume_text()
			.rect().snap_to(volume_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder issuedby_caption(page);
		issuedby_caption()
			.text("Issued by")
			.rect().snap_to(volume_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder issuedby_text(page, "issuedto_text");
		issuedby_text()
			.rect().snap_to(issuedby_caption().rect(), snap_type::bottom, 0);

		widgets::button_builder add_to_table(page, "add_to_table");
		add_to_table()
			.text("Add")
			.rect().size({ 80, 20 })
			.set(page.size().width - 90.f, issuedby_text().rect().bottom() + margin_ * 2, 80.f, 20.f);
		add_to_table().events().click = [&]() {
			if (!on_add_coupon(error)) {
				message("Error: " + error);
				return;
			}
		};

		widgets::table_view_builder coupons_table(page, "coupons_table");
		{
			std::vector<table_column> coupons_table_cols =
			{
				{ "#", 50 },
				{ "Serial Number", 150 },
				{ "Fuel", 90 },
				{ "Volume", 90 },
				{ "Issued By", 100 }
			};

			coupons_table()
				.border(1)
				.corner_radius_x(0)
				.corner_radius_y(0)
				.color_fill({ 255, 255, 255, 0 })
				.on_resize({ -50.f, 0.f, 50.f, 0.f })
				.columns(coupons_table_cols)
				.rect().size({ page.size().width - margin_, 200 })
				.set(margin_, add_to_table().rect().get_bottom() + 2.f, page.size().width - (margin_ * 2), 200);
			coupons_table().events().selection = [&]
			(const std::vector<table_row>& rows) {};
		}

		widgets::button_builder coupon_button(page, "coupon_button");
		coupon_button()
			.text("Save")
			.rect().snap_to(coupons_table().rect(), snap_type::bottom, 3.f * margin_);
		coupon_button().events().click = [&]() {
			if (!on_save(error)) {
				message("Error: " + error);
				return;
			}

			close();
		};

		page_man.show(page_name_);
		return true;
	}

	bool on_add_coupon(std::string& error) {
		try {
			auto serial_number = widgets::text_field_builder::specs(*this, page_name_ + "/couponno_text").text();
			auto fuel_type = widgets::combobox_builder::specs(*this, page_name_ + "/fueltype_cbo").text();
			auto volume = widgets::text_field_builder::specs(*this, page_name_ + "/volume_text").text();
			auto issued_by = widgets::text_field_builder::specs(*this, page_name_ + "/issuedto_text").text();

			if (serial_number.empty() ||
				fuel_type.empty() ||
				volume.empty() ||
				issued_by.empty()
				) {
				error = "fill in all fields!";
				return false;
			}

			//Checking if volume is int.
			if (!std::all_of(volume.begin(), volume.end(), ::isdigit)) {
				error = "Volume not a number!";
				return false;
			}
			
			auto table = widgets::table_view_builder::specs(*this, page_name_ + "/coupons_table");
			auto table_size = table.data().size();

			if (table_size != 0){
				for (const auto& row : table.data()){
					if (serial_number == database::get::text(row.at("Serial Number"))){
						error = "Serial number already exists!";
						return false;
					}
				}
			}

			widgets::table_view_builder::specs(*this, page_name_ + "/coupons_table")
				.data().push_back(
					{
						{"#", std::to_string(table_size + 1)},
						{"Serial Number", serial_number },
						{"Fuel", fuel_type},
						{"Volume", volume },
						{"Issued By", issued_by }
					}
			);
			update();

		}
		catch (const std::exception& ex) {
			error = std::string(ex.what());
			return false;
		}

		return true;
	}

	bool on_save(std::string& error) {
		try
		{
			auto date_received = date_time::to_string(date_time::today());

			auto coupons = 
				widgets::table_view_builder::specs(*this, page_name_ + "/coupons_table").data();

			if (coupons.empty()) {
				error = "There is nothing to save!";
				return false;
			}

			for (auto& row : coupons){
				row.erase("#");
				row.insert(std::make_pair("Date", date_received));
			}

			if (!state_.get_db().on_save_coupons(coupons, error))
				return false;

			saved_coupons_ = coupons;
			widgets::table_view_builder::specs(*this, page_name_ + "/coupons_table")
				.data().clear();
		}
		catch (const std::exception& ex)
		{
			error = std::string(ex.what());
			return false;
		}

		return true;
	}
public:
	addcopoupons_form(const std::string& caption, liblec::lecui::form& parent_form, state& app_state_, std::vector<table_row>& saved_coupons) :
		form(caption, parent_form), state_(app_state_), saved_coupons_(saved_coupons) {}

};

