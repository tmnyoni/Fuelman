#pragma once
#include <iostream>
#include <any>

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

#include "main_ui.h"

using namespace liblec::lecui;
using snap_type = rect::snap_type;

class voucher_form : public form {
	const float margin_ = 10.f;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_manager page_man{ *this };
	widget_manager widget_man{ *this };

	bool on_initialize(std::string& error) override {
		controls_.allow_minimize(false);
		controls_.allow_resize(false);
		appearance_.theme(themes::light);
		dims_.set_size({ 400, 600 });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = page_man.add("voucher_page");

		widgets::label_builder fueltype_caption(page);
		fueltype_caption()
			.text("Fuel type")
			.rect().place(
				{
					margin_,
					page.size().width - margin_,
					margin_,
					margin_ + (dims_.get_size().height / 2.f)
				}, 50.f, 0.f
			);

		std::vector<widgets::combobox_specs::combobox_item> fueltypes =
		{
			{  "Petrol " }, {"Diesel"}
		};

		widgets::combobox_builder fueltype_cbo(page, "fueltype_cbo");
		fueltype_cbo()
			.items(fueltypes)
			.color_fill({ 255,255,255,0 })
			.rect().size(200, 25)
			.snap_to(fueltype_caption().rect(), snap_type::bottom, 0);
		fueltype_cbo().events().selection = [](const std::string& selected) {};

		widgets::label_builder voucherno_caption(page);
		voucherno_caption()
			.text("Voucher Number")
			.rect().snap_to(fueltype_cbo().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder voucherno_text(page, "voucherno_text");
		voucherno_text()
			.rect().snap_to(voucherno_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder quantity_caption(page);
		quantity_caption()
			.text("Quantity")
			.rect().snap_to(voucherno_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder quantity_text(page, "quantity_text");
		quantity_text()
			.rect().snap_to(quantity_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder issuedby_caption(page);
		issuedby_caption()
			.text("Issued by")
			.rect().snap_to(quantity_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder issuedby_text(page, "issuedto_text");
		issuedby_text()
			.rect().snap_to(issuedby_caption().rect(), snap_type::bottom, 0);

		widgets::button_builder add_to_table(page, "add_to_table");
		add_to_table()
			.text("Add")
			.rect().size({80, 20})
			.set(page.size().width - 90.f, issuedby_text().rect().bottom() + margin_ * 2, 80.f, 20.f);
		add_to_table().events().click = [&]() {
			if (!on_add_voucher(error)) {
				message("Error: " + error);
				return;
			}
		};

		std::vector<table_column> coupons_table_cols =
		{
			{"#", 50},
			{"Date", 80},
			{"Voucher Number", 120},
			{"Fuel Type", 90},
			{"Quantity", 90},
			{"Issued by", 100}
		};

		widgets::table_view_builder vouchers_table(page, "vouchers_table");
		vouchers_table()
			.border(1)
			.corner_radius_x(0)
			.corner_radius_y(0)
			.color_fill({ 255, 255, 255, 0 })
			.on_resize({ -50.f, 0.f, 50.f, 0.f })
			.columns(coupons_table_cols)
			.rect().size({ page.size().width - margin_, 200})
			.set(margin_, add_to_table().rect().get_bottom() +2.f, page.size().width - (margin_ * 2),200);
		vouchers_table().events().selection = [&]
			(const std::vector<table_row>& rows){};

		widgets::button_builder voucher_button(page, "voucher_button");
		voucher_button()
			.text("Save")
			.rect().snap_to(vouchers_table().rect(), snap_type::bottom, 3.f * margin_);
		voucher_button().events().click = [&]() {
			if (!on_save(error)) {
				message("Error: " + error);
				return;
			}

			close();
		};

		page_man.show("voucher_page");
		return true;
	}

	bool on_add_voucher(std::string& error) {
		try {
			auto voucher_number = widgets::text_field_builder::specs(*this, "voucher_page/voucherno_text").text();
			auto fuel_type = widgets::combobox_builder::specs(*this, "voucher_page/fueltype_cbo").text();
			auto quantity = widgets::text_field_builder::specs(*this, "voucher_page/quantity_text").text();
			auto issued_by = widgets::text_field_builder::specs(*this, "voucher_page/issuedto_text").text();

			if (voucher_number.empty() ||
				fuel_type.empty() ||
				quantity.empty() ||
				issued_by.empty()
				) {
				error = "fill in all fields.";
				return false;
			}

			auto date_of_voucher = date_time::to_string(date_time::today());

			auto table_size =
				widgets::table_view_builder::specs(*this, "voucher_page/vouchers_table").data().size();
			widgets::table_view_builder::specs(*this, "voucher_page/vouchers_table")
				.data().push_back(
					{
						{"#", std::to_string(table_size + 1)},
						{"Date", date_of_voucher},
						{"Voucher Number", voucher_number },
						{"Fuel Type", fuel_type},
						{"Quantity", quantity },
						{"Issued by", issued_by }
					}
			);

		}
		catch (const std::exception& ex) {
			error = std::string(ex.what());
			return false;
		}

		return true;
	}

	bool on_save(std::string& error){
		try
		{
			//save to db.

			widgets::table_view_builder::specs(*this, "voucher_page/vouchers_table")
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
	voucher_form(const std::string& caption, liblec::lecui::form& parent_form) :
		form(caption, parent_form) {}

};

