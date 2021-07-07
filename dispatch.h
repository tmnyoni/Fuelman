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

class dispatch_form : public form {
	const float margin_ = 10.f;

	appearance appearance_{ *this };
	controls controls_{ *this };
	dimensions dims_{ *this };
	page_management page_man{ *this };
	widget_management widget_man{ *this };

	bool on_initialize(std::string& error) override {
		controls_.allow_minimize(false);
		controls_.allow_resize(false);
		appearance_.theme(themes::light);
		dims_.set_size({ 350, 450 });
		return true;
	}

	bool on_layout(std::string& error) override {
		auto& page = page_man.add("dispatch_page");

		widgets::label_builder fueltype_caption(page);
		fueltype_caption()
			.text("Fuel type")
			.rect().place(
				{
					margin_,
					page.size().width - margin_,
					margin_, margin_ + (dims_.get_size().height / 2.f) 
				},  50.f, 0.f
			);

		std::vector<widgets::combobox_specs::combobox_item> fueltypes = 
		{
			{  "Petrol " }, {"Diesel"}
		};

		widgets::combobox_builder fueltype_cbo(page, "fueltype_cbo");
		fueltype_cbo()
			.items(fueltypes)
			.color_fill({255,255,255,0})
			.rect().size(200, 25)
			.snap_to(fueltype_caption().rect(), snap_type::bottom, 0);
		fueltype_cbo().events().selection = [](const std::string& selected) {};

		widgets::label_builder serialno_caption(page);
		serialno_caption()
			.text("Serial Number")
			.rect().snap_to(fueltype_cbo().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder serialno_text(page, "serialno_text");
		serialno_text()
			.rect().snap_to(serialno_caption().rect(), snap_type::bottom, 0);
		
		widgets::label_builder quantity_caption(page);
		quantity_caption()
			.text("Quantity")
			.rect().snap_to(serialno_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder quantity_text(page, "quantity_text");
		quantity_text()
			.rect().snap_to(quantity_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder issuedto_caption(page);
		issuedto_caption()
			.text("Issued to")
			.rect().snap_to(quantity_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder issuedto_text(page, "issuedto_text");
		issuedto_text()
			.rect().snap_to(issuedto_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder recvby_caption(page);
		recvby_caption()
			.text("Received by")
			.rect().snap_to(issuedto_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder recvby_text(page, "recvby_text");
		recvby_text()
			.rect().snap_to(recvby_caption().rect(), snap_type::bottom, 0);

		widgets::label_builder comments_caption(page);
		comments_caption()
			.text("Comments")
			.rect().snap_to(recvby_text().rect(), snap_type::bottom, margin_);

		widgets::text_field_builder comments_text(page, "comments_text");
		comments_text()
			.rect().snap_to(comments_caption().rect(), snap_type::bottom, 0);

		widgets::button_builder dispatch_button(page, "dispatch_button");
		dispatch_button()
			.text("Dispatch")
			.rect().snap_to(comments_text().rect(), snap_type::bottom, 3.f * margin_);
		dispatch_button().events().click = [&]() {
			if (!on_dispatch(error)){
				message("Error: " + error);
				return;
			}

			close();
		};

		page_man.show("dispatch_page");
		return true;
	}

	bool on_dispatch(std::string& error){
		try {
			auto serial_number = widgets::text_field_builder::specs(*this, "dispatch_page/serialno_text").text();
			auto quantity = widgets::text_field_builder::specs(*this, "dispatch_page/quantity_text").text();
			auto issued_to = widgets::text_field_builder::specs(*this, "dispatch_page/issuedto_text").text();
			auto received_by = widgets::text_field_builder::specs(*this, "dispatch_page/recvby_text").text();
			auto comments = widgets::text_field_builder::specs(*this, "dispatch_page/comments_text").text();

			if (serial_number.empty() ||
				quantity.empty() ||
				issued_to.empty() ||
				received_by.empty()
				) {
				error = "fill in all important fields.";
				return false;
			}

			auto date_of_dispatch = date_time::to_string(date_time::today());

			/*if (!get_state.get_db().on_coupon_dispatch(quantity, error)) {
				message("Error: " + error);
				return false;
			}*/

		}
		catch (std::exception& ex){
			error = std::string(ex.what());
			return false;
		}

		return true;
	}
public:
	dispatch_form(const std::string& caption, liblec::lecui::form& parent_form) :
		form(caption, parent_form) {}

};

