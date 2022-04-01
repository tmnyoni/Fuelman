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

#include <liblec/leccore/app_version_info.h>
#include <liblec/leccore/hash.h>
#include <liblec/leccore/file.h>
#include <liblec/leccore/system.h>
#include <liblec/leccore/zip.h>
#include <filesystem>

#include "../../version_info/version_info.h"
#include "../../resource.h"

#include "../main_ui.h"

#ifdef _WIN64
#define architecture	"64bit"
#else
#define architecture	"32bit"
#endif

bool main_window::on_initialize(std::string& error) {

	if (!_cleanup_mode || !_update_mode)
		get_dpi_scale() < 2.f
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
	else {
		std::string value, update_architecture;
		// check if there is an update ready to be installed
		if (_settings.read_value("updates", "readytoinstall", value, error)) {}
		if (_settings.read_value("updates", "architecture", update_architecture, error)) {}

		// clear the registry entry
		if (!_settings.delete_value("updates", "readytoinstall", error)) {}
		if (!_settings.delete_value("updates", "architecture", error)) {}

		if (!value.empty()) {
			const std::string app_architecture(architecture);

			if (app_architecture == update_architecture ||
				update_architecture.empty()) {

				try {
					const std::string fullpath(value);
					std::filesystem::path file_path(fullpath);

					const std::string directory = file_path.parent_path().string();
					const std::string filename = file_path.filename().string();

					// assume the zip file extracts to a directory with the same name
					std::string unzipped_folder;
					const auto idx = filename.find(".zip");

					if (idx != std::string::npos)
						unzipped_folder = directory + "\\" + filename.substr(0, idx);

					// unzip the file into the same directory as the zip file
					leccore::unzip unzip;
					unzip.start(fullpath, directory);

					while (unzip.unzipping()) {
						if (!keep_alive()) {
							// to-do: implement stopping mechanism
							//unzip.stop()
							close();
							return true;
						}
					}

					leccore::unzip::unzip_log log;
					if (unzip.result(log, error) && std::filesystem::exists(unzipped_folder)) {
						// get target directory
						std::string target_directory;

#ifdef _WIN64
						target_directory = _install_location_64;
#else
						target_directory = _install_location_32;
#endif

						if (!target_directory.empty()) {
							if (_settings.write_value("updates", "rawfiles", unzipped_folder, error) &&
								_settings.write_value("updates", "target", target_directory, error)) {

								// run downloaded app from the unzipped folder
#ifdef _WIN64
								const std::string new_exe_fullpath = unzipped_folder + "\\pc_info64.exe";
#else
								const std::string new_exe_fullpath = unzipped_folder + "\\pc_info32.exe";
#endif
								if (leccore::shell::create_process(new_exe_fullpath, { "/update" }, error)) {
									close();
									return true;
								}
							}
						}

						// continue app execution normally
					}
					else {
						// delete the update folder ... there many be something wrong with the update file
						if (!leccore::file::remove_directory(directory, error)) {}

						// continue app execution normally
					}
				}
				catch (const std::exception&) {
					// continue app execution normally
				}
			}
			else {
				// system architecture did not match ... continue app execution normally
			}
		}
		else
			if (_update_mode) {
				// get the location of the raw files
				if (_settings.read_value("updates", "rawfiles", value, error) && !value.empty()) {
					const std::string raw_files_directory(value);

					if (_settings.read_value("updates", "target", value, error)) {
						std::string target(value);
						if (!target.empty()) {
							try {
								// overrwrite the files in target with the files in raw_files_directory
								for (const auto& path : std::filesystem::directory_iterator(raw_files_directory)) {
									std::filesystem::path p(path);
									const std::string dest_file = target + p.filename().string();
									std::filesystem::copy_file(path, dest_file, std::filesystem::copy_options::overwrite_existing);
								}

								// files copied successfully, now execute the app in the target directory
#ifdef _WIN64
								const std::string updated_exe_fullpath = target + "\\pc_info64.exe";
#else
								const std::string updated_exe_fullpath = target + "\\pc_info32.exe";
#endif
								if (leccore::shell::create_process(updated_exe_fullpath, { "/recentupdate" }, error)) {}

								close();
								return true;
							}
							catch (const std::exception& e) {
								error = e.what();

								// exit
								close();
								return true;
							}
						}
					}
				}
				else {
					close();
					return true;
				}
			}
			else
				if (_recent_update_mode) {
					// check if the updates_rawfiles and updates_target settings are set, and eliminated them if so then notify user of successful update
					std::string updates_rawfiles;
					if (!_settings.read_value("updates", "rawfiles", updates_rawfiles, error) && !value.empty()) {}

					std::string updates_target;
					if (!_settings.read_value("updates", "target", updates_target, error)) {}

					if (!updates_rawfiles.empty() || !updates_target.empty()) {
						if (!_settings.delete_value("updates", "rawfiles", error)) {}
						if (!_settings.delete_value("updates", "target", error)) {}

						// update inno setup version number
						leccore::registry reg(leccore::registry::scope::current_user);
#ifdef _WIN64
						if (!reg.do_write("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + _install_guid_64 + "_is1",
							"DisplayVersion", std::string(appversion), error)) {
						}
#else
						if (!reg.do_write("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + _install_guid_32 + "_is1",
							"DisplayVersion", std::string(appversion), error)) {
						}
#endif

						// to-do: use a timer instead for all the calls below, for better user experience
						message("App updated successfully to version " + std::string(appversion));

						std::string updates_tempdirectory;
						if (!_settings.read_value("updates", "tempdirectory", updates_tempdirectory, error)) {}
						else {
							// delete updates temp directory
							if (!leccore::file::remove_directory(updates_tempdirectory, error)) {}
						}
						if (!_settings.delete_value("updates", "tempdirectory", error)) {}
					}
				}
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
		.theme(lecui::themes::dark)
		.main_icon(ico_resource)
		.mini_icon(ico_resource);

	_dimensions.set_size(_window_size);

	_splash_screen.remove();

	return true;
}