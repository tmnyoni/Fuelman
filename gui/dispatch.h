#pragma once

#include <iostream>
#include <any>
#include <chrono>

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
	const float _margin = 10.f;
	const std::string _page_name = "dispatch-page";
	
	state& _state;
	std::map<std::string, std::any>& _saved_coupon;
	bool& _is_changed;

	appearance _appearance{ *this };
	controls _controls{ *this };
	dimensions _dimensions{ *this };
	page_manager _page_manager{ *this };
	widget_manager _widget_manager{ *this };

	bool on_initialize(std::string& error) override {
		_controls.allow_minimize(false);
		_controls.allow_resize(false);
		_dimensions.set_size(lecui::size().width(350.f).height(410.f));
		return true;
	}

	void on_start() override {
		try	{
			// prevent changing of volume
			std::string error;
			if (!_widget_manager.disable(_page_name + "/volume-text", error)) {}
		}
		catch (const std::exception&) {}
	}

	bool on_layout(std::string& error) override {
		auto& page = _page_manager.add(_page_name);
		using get_ = database::get;

		auto& fuel_caption = widgets::label::add(page);
		fuel_caption
			.text("Fuel Type")
			.rect(rect().left(_margin))
			.rect().place(rect()
				.left(_margin)
				.right(page.size().get_width() - _margin)
				.top(_margin)
				.bottom(_margin + (_dimensions.get_size().get_height() / 2.f)),
				50.f, 0.f);

		auto& fuel_select = widgets::combobox::add(page, "fuel-select");
		{
			std::vector<widgets::combobox::combobox_item> fuels =
			{
				{  "Petrol" }, {"Diesel"}
			};

			fuel_select
				.items(fuels)
				.selected("Petrol")
				.color_fill({ 255, 255, 255, 0 })
				.rect().size(200.f, 25.f)
				.snap_to(fuel_caption.rect(), snap_type::bottom, 0);
			//fuel_caption.events().click = [](const std::string& selected) {};
		}

		auto& serial_number_caption = widgets::label::add(page);
		serial_number_caption
			.text("Serial Number")
			.rect().snap_to(fuel_select.rect(), snap_type::bottom, _margin);

		auto& serial_number_text = widgets::text_field::add(page, "serial-number-text");
		serial_number_text
			.text(get_::text(_saved_coupon.at("Serial Number")))
			.rect().snap_to(serial_number_caption.rect(), snap_type::bottom, 0);

		auto& volume_caption = widgets::label::add(page);
		volume_caption
			.text("Volume, in litres")
			.rect().snap_to(serial_number_text.rect(), snap_type::bottom, _margin);

		auto& volume_text = widgets::text_field::add(page, "volume-text");
		volume_text
			.allowed_characters({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' })
			.text(leccore::round_off::to_string(get_::real(_saved_coupon.at("Volume")), 0))
			.rect().snap_to(volume_caption.rect(), snap_type::bottom, 0);

		auto& receiver_department_caption = widgets::label::add(page);
		receiver_department_caption
			.text("Issued to")
			.rect().snap_to(volume_text.rect(), snap_type::bottom, _margin);

		auto& receiver_department_text = widgets::text_field::add(page, "receiver-department-text");
		receiver_department_text.rect().snap_to(receiver_department_caption.rect(), snap_type::bottom, 0);

		auto& receiver_caption = widgets::label::add(page);
		receiver_caption
			.text("Received By")
			.rect().snap_to(receiver_department_text.rect(), snap_type::bottom, _margin);

		auto& receiver_text = widgets::text_field::add(page, "receiver-text");
		receiver_text.rect().snap_to(receiver_caption.rect(), snap_type::bottom, 0);

		auto& comments_caption = widgets::label::add(page);
		comments_caption
			.text("Comments")
			.rect().snap_to(receiver_text.rect(), snap_type::bottom, _margin);

		auto& comments_text = widgets::text_field::add(page, "comments-text");
		comments_text.rect().snap_to(comments_caption.rect(), snap_type::bottom, 0);

		auto& dispatch_button = widgets::button::add(page);
		dispatch_button
			.text("Dispatch")
			.rect().snap_to(comments_text.rect(), snap_type::bottom, 2.f * _margin);
		dispatch_button.events().action = [&]() {
			if (!on_dispatch(error)) {
				message("Error: " + error);
				return;
			}

			close();
		};

		_page_manager.show(_page_name);
		return true;
	}

	bool on_dispatch(std::string& error) {
		try {

			auto fuel = get_combobox(_page_name + "/fuel-select").text();
			auto serial_number = get_text_field(_page_name + "/serial-number-text").text();
			auto volume = get_text_field(_page_name + "/volume-text").text();
			auto receiver_department = get_text_field(_page_name + "/receiver-department-text").text();
			auto receiver = get_text_field(_page_name + "/receiver-text").text();
			auto comments = get_text_field(_page_name + "/comments-text").text();

			if (serial_number.empty() ||
				volume.empty() ||
				receiver_department.empty() ||
				receiver.empty()
				) {
				error = "fill in all important fields.";
				return false;
			}

			double time = static_cast<double>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

			database::row dispatched_coupon = {
				{ "Date", time },
				{ "Serial Number", serial_number },
				{ "Fuel Type", fuel },
				{ "Volume", atof(volume.c_str())},
				{ "Issued To", receiver_department },
				{ "Receiver", receiver },
				{ "Comments", comments }
			};

			if (!_state.get_db().on_dispatch_coupons(dispatched_coupon, error))
				return false;

			_is_changed = true;
		}
		catch (const std::exception& ex) {
			error = std::string(ex.what());
			return false;
		}

		return true;
	}
public:
	dispatch_form(const std::string& caption,
		liblec::lecui::form& parent_form,
		state& _state,
		std::map<std::string, std::any>& edited_coupon,
		bool& is_changed
	) :
		form(caption, parent_form),
		_state(_state),
		_saved_coupon(edited_coupon),
		_is_changed(is_changed) {}

};