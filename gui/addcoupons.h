#pragma once
#include <iostream>
#include <any>
#include <algorithm>
#include <set>
#include <chrono>

#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/containers/page.h>

#include <liblec/lecui/widgets/button.h>
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/text_field.h>
#include <liblec/lecui/widgets/table_view.h>
#include <liblec/lecui/widgets/combobox.h>
#include <liblec/lecui/widgets/image_view.h>

#include <liblec/lecui/utilities/date_time.h>

#include "../appstate/app_state.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

class addcoupons_form : public form {
	const float _margin = 10.f;
	const std::string _page_name = "add-coupons-page";

	state& _state;
	std::vector<table_row>& _saved_coupons;

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

		auto& fuel_caption = widgets::label::add(page);
		fuel_caption
			.text("Fuel")
			.rect().place(rect()
				.left(_margin)
				.right(page.size().get_width() - _margin)
				.top(_margin)
				.bottom(_margin + (_dimensions.get_size().get_height() / 2.f)),
				50.f, 0.f);

		auto& fuel_select = widgets::combobox::add(page, "fuel-select");
		fuel_select
			.items({ { "Petrol" }, { "Diesel" } })
			.rect().size(200, 25)
			.snap_to(fuel_caption.rect(), snap_type::bottom, 0);
		fuel_select.events().selection = [](const std::string& selected) {};

		auto& serial_number_caption = widgets::label::add(page);
		serial_number_caption
			.text("Serial Number")
			.rect().snap_to(fuel_select.rect(), snap_type::bottom, _margin);

		auto& serial_number_text = widgets::text_field::add(page, "serial-number-text");
		serial_number_text.rect().snap_to(serial_number_caption.rect(), snap_type::bottom, 0);

		auto& volume_caption = widgets::label::add(page);
		volume_caption
			.text("Volume")
			.rect().snap_to(serial_number_text.rect(), snap_type::bottom, _margin);

		auto& volume_text = widgets::text_field::add(page, "volume-text");
		volume_text
			.allowed_characters({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' })
			.rect().snap_to(volume_caption.rect(), snap_type::bottom, 0);

		auto& issuedby_caption = widgets::label::add(page);
		issuedby_caption
			.text("Issued by")
			.rect().snap_to(volume_text.rect(), snap_type::bottom, _margin);

		auto& issuedby_text = widgets::text_field::add(page, "issuedby-text");
		issuedby_text.rect().snap_to(issuedby_caption.rect(), snap_type::bottom, 0);

		auto& add_to_table_button = widgets::button::add(page);
		add_to_table_button
			.text("Add")
			.rect(rect()
				.left(page.size().get_width() - (_margin * 9.0f))
				.top(issuedby_text.rect().bottom() + _margin * 2)
				.width(8.f * _margin)
				.height(2 * _margin))
			.events().action = [&]() {
				if (!on_add_coupon(error)) {
					message("Error: " + error);
					return;
				}};

		auto& added_coupons_table = widgets::table_view::add(page, "added-coupons-table");
		{
			std::vector<table_column> added_coupons_columns =
			{
				{ "Serial Number", 100 },
				{ "Fuel", 90 },
				{ "Volume", 90, 0 /* number of decimal places */ },
				{ "Issued By", 100 }
			};

			added_coupons_table
				.border(0)
				.fixed_number_column(true)
				.corner_radius_x(0)
				.corner_radius_y(0)
				.color_fill({ 255, 255, 255, 0 })
				.on_resize({ -50.f, 0.f, 50.f, 0.f })
				.columns(added_coupons_columns)
				.rect().size({ page.size().get_width() - _margin, 200 })
				.set(_margin, add_to_table_button.rect().get_bottom() + 2.f, page.size().get_width() - (_margin * 2), 200);
		}

		auto& coupon_button = widgets::button::add(page);
		coupon_button
			.text("Save")
			.rect().snap_to(added_coupons_table.rect(), snap_type::bottom, 3.f * _margin);
		coupon_button.events().action = [&]() {
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
			auto serial_number = get_text_field(_page_name + "/serial-number-text").text();
			auto fuel_type = get_combobox(_page_name + "/fuel-select").text();
			auto volume = get_text_field(_page_name + "/volume-text").text();
			auto issued_by = get_text_field(_page_name + "/issuedby-text").text();

			if (serial_number.empty() ||
				fuel_type.empty() ||
				volume.empty() ||
				issued_by.empty()
				) {
				error = "fill in all fields!";
				return false;
			}

			auto table = get_table_view(_page_name + "/added-coupons-table");
			auto table_size = table.data().size();

			if (table_size != 0) {
				for (const auto& row : table.data()) {
					if (serial_number == database::get::text(row.at("Serial Number"))) {
						error = "Serial number already exists!";
						return false;
					}
				}
			}

			get_table_view(_page_name + "/added-coupons-table")
				.data().push_back(
					{
						{"Serial Number", serial_number },
						{"Fuel", fuel_type},
						{"Volume", atof(volume.c_str()) },
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
				get_table_view(_page_name + "/added-coupons-table").data();

			if (coupons.empty()) {
				error = "There is nothing to save!";
				return false;
			}

			for (auto& row : coupons) {
				double time = static_cast<double>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
				row.insert(std::make_pair("Date", time));
			}
			
			if (!_state.get_db().on_save_coupons(coupons, error))
				return false;

			_saved_coupons = coupons;
			get_table_view(_page_name + "/added-coupons-table")
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
	addcoupons_form(const std::string& caption,
		liblec::lecui::form& parent_form,
		state& app__state,
		std::vector<table_row>& saved_coupons
	) :
		form(caption, parent_form),
		_state(app__state),
		_saved_coupons(saved_coupons) {}
};

