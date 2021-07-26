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
	const float _margin = 10.f;
	const std::string _page_name = "add-coupons-page";

	state& _state;
	std::vector<table_row>& saved_coupons_;

	appearance _appearance{ *this };
	controls _controls{ *this };
	dimensions _dimensions{ *this };
	page_manager _page_manager{ *this };
	widget_manager _widget_manager{ *this };

	bool on_initialize(std::string& error) override {
		_controls.
			allow_minimize(false)
			.allow_resize(false);
		_dimensions.set_size({ 500.f, 600.f });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = _page_manager.add(_page_name);

		widgets::label_builder fuel_type_caption(page);
		fuel_type_caption()
			.text("Fuel")
			.rect().place(rect()
				.left(_margin)
				.right(page.size().width - _margin)
				.top(_margin)
				.bottom(_margin + (_dimensions.get_size().height / 2.f)),
				50.f, 0.f);

		widgets::combobox_builder fuel_type_cbo(page, "fueltype_cbo");
		{
			std::vector<widgets::combobox_specs::combobox_item> fueltypes = {
				{  "Petrol" }, {"Diesel"}
			};

			fuel_type_cbo()
				.items(fueltypes)
				.color_fill({ 255,255,255,0 })
				.rect().size(200, 25)
				.snap_to(fuel_type_caption().rect(), snap_type::bottom, 0);
			fuel_type_cbo().events().selection = [](const std::string& selected) {};
		}

		widgets::label_builder serial_number_caption(page);
		serial_number_caption()
			.text("Serial Number")
			.rect().snap_to(fuel_type_cbo().rect(), snap_type::bottom, _margin);

		widgets::text_field_builder serial_number_text(page, "couponno_text");
		serial_number_text()
			.rect().snap_to(serial_number_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder volume_caption(page);
		volume_caption()
			.text("Volume")
			.rect().snap_to(serial_number_text().rect(), snap_type::bottom, _margin);

		widgets::text_field_builder volume_text(page, "volume_text");
		volume_text()
			.rect().snap_to(volume_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder issuedby_caption(page);
		issuedby_caption()
			.text("Issued by")
			.rect().snap_to(volume_text().rect(), snap_type::bottom, _margin);

		widgets::text_field_builder issuedby_text(page, "issuedto_text");
		issuedby_text()
			.rect().snap_to(issuedby_caption().rect(), snap_type::bottom, 0);

		widgets::button_builder add_to_table(page, "add_to_table");
		add_to_table()
			.text("Add")
			.rect().size({ 80, 20 })
			.set(page.size().width - 90.f, issuedby_text().rect().bottom() + _margin * 2, 80.f, 20.f);
		add_to_table().events().action = [&]() {
			if (!on_add_coupon(error)) {
				message("Error: " + error);
				return;
			}
		};

		widgets::table_view_builder coupons_table(page, "coupons_table");
		{
			std::vector<table_column> coupons_table_cols =
			{
				{ "Serial Number", 150 },
				{ "Fuel", 90 },
				{ "Volume", 90 },
				{ "Issued By", 100 }
			};

			coupons_table()
				.border(0)
				.fixed_number_column(true)
				.corner_radius_x(0)
				.corner_radius_y(0)
				.color_fill({ 255, 255, 255, 0 })
				.on_resize({ -50.f, 0.f, 50.f, 0.f })
				.columns(coupons_table_cols)
				.rect().size({ page.size().width - _margin, 200 })
				.set(_margin, add_to_table().rect().get_bottom() + 2.f, page.size().width - (_margin * 2), 200);
			coupons_table().events().selection = [&]
			(const std::vector<table_row>& rows) {};
		}

		widgets::button_builder coupon_button(page, "coupon_button");
		coupon_button()
			.text("Save")
			.rect().snap_to(coupons_table().rect(), snap_type::bottom, 3.f * _margin);
		coupon_button().events().action = [&]() {
			if (!on_save(error)) {
				message("Error: " + error);
				return;
			}

			close();
		};

		_page_manager.show(_page_name);
		return true;
	}

	bool on_add_coupon(std::string& error) {
		try {
			auto serial_number = get_text_field_specs(_page_name + "/couponno_text").text();
			auto fuel_type = get_combobox_specs(_page_name + "/fueltype_cbo").text();
			auto volume = get_text_field_specs(_page_name + "/volume_text").text();
			auto issued_by = get_text_field_specs(_page_name + "/issuedto_text").text();

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
			
			auto table = get_table_view_specs(_page_name + "/coupons_table");
			auto table_size = table.data().size();

			if (table_size != 0){
				for (const auto& row : table.data()){
					if (serial_number == database::get::text(row.at("Serial Number"))){
						error = "Serial number already exists!";
						return false;
					}
				}
			}

			get_table_view_specs(_page_name + "/coupons_table")
				.data().push_back(
					{
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
			auto coupons = 
				get_table_view_specs(_page_name + "/coupons_table").data();

			if (coupons.empty()) {
				error = "There is nothing to save!";
				return false;
			}

			auto date_received = date_time::to_string(date_time::today());

			for (auto& row : coupons){
				row.insert(std::make_pair("Date", date_received));
			}

			if (!_state.get_db().on_save_coupons(coupons, error))
				return false;

			saved_coupons_ = coupons;
			get_table_view_specs(_page_name + "/coupons_table")
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
	addcopoupons_form(const std::string& caption, liblec::lecui::form& parent_form, state& app__state, std::vector<table_row>& saved_coupons) :
		form(caption, parent_form), _state(app__state), saved_coupons_(saved_coupons) {}

};

