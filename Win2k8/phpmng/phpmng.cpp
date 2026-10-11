#include "phpmng.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

namespace Tools
{
	void phpmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);

		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);
		
		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");

		domain = main_case["domain"];
		phpver = main_case["phpver"];

		display_errors = main_case["display_errors"];

		display_startup_errors = main_case["display_startup_errors"];
		error_reporting = main_case["error_reporting"];
		log_errors = main_case["log_errors"];
		log_errors_max_len = main_case["log_errors_max_len"];
		ignore_repeated_errors = main_case["ignore_repeated_errors"];
		html_errors = main_case["html_errors"];
		error_log = main_case["error_log"];
		extensions = main_case["extensions"];
		short_open_tag = main_case["short_open_tag"];
		output_buffering = main_case["output_buffering"];
		precision = main_case["precision"];
		ignore_user_abort = main_case["ignore_user_abort"];
		default_charset = main_case["default_charset"];
		date_timezone = main_case["date_timezone"];
		bcmath_scale = main_case["bcmath_scale"];
		allow_url_fopen = main_case["allow_url_fopen"];
		auto_globals_jit = main_case["auto_globals_jit"];
		memory_limit = main_case["memory_limit"];
		max_input_time = main_case["max_input_time"];
		max_execution_time = main_case["max_execution_time"];
		post_max_size = main_case["post_max_size"];
		upload_max_filesize = main_case["upload_max_filesize"];
		max_file_uploads = main_case["max_file_uploads"];
		session_save_handler = main_case["session_save_handler"];
		session_save_path = main_case["session_save_path"];
		session_use_cookies = main_case["session_use_cookies"];
		session_name = main_case["session_name"];
		session_auto_start = main_case["session_auto_start"];
		session_cookie_lifetime = main_case["session_cookie_lifetime"];
		session_cookie_path = main_case["session_cookie_path"];
		session_cookie_domain = main_case["session_cookie_domain"];
		session_cache_expire = main_case["session_cache_expire"];


		ini_files["php56"] = env->php_dir + "56\\php.ini";
		ini_files["php72"] = env->php_dir + "72\\php.ini";
		ini_files["php74"] = env->php_dir + "74\\php.ini";

		if (process == "save") {
			save();
		}
		else if (process == "show") {
			show();
		}
		else if (process == "get-modules") {
			getModules();
		}
		else if (process == "set-system") {
			setSystem();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void phpmng::save()
	{
		try {
			String ^domain_ini_file_path = env->panel_domain_configs_dir + domain + "\\php\\php.ini";
			String ^system_ini_file_path = ini_files[phpver];
			String ^content_file = File::ReadAllText(system_ini_file_path);

			if (!String::IsNullOrEmpty(display_errors)) {
				utils->iniReplace(content_file, "^display_errors.*=.*", "display_errors = " + display_errors);
			}
			if (!String::IsNullOrEmpty(display_startup_errors)) {
				utils->iniReplace(content_file, "^display_startup_errors.*=.*", "display_startup_errors = " + display_startup_errors);
			}
			if (!String::IsNullOrEmpty(error_reporting)) {
				utils->iniReplace(content_file, "^error_reporting.*=.*", "error_reporting = " + error_reporting);
			}

			// Ozel durum log_errors ile log_errors_max_len karisiyorlar
			if (!String::IsNullOrEmpty(log_errors)) {
				utils->iniReplace(content_file, "^log_errors =.*", "log_errors = " + log_errors);
				utils->iniReplace(content_file, "^log_errors=.*", "log_errors = " + log_errors);
			}
			if (!String::IsNullOrEmpty(log_errors_max_len)) {
				utils->iniReplace(content_file, "^log_errors_max_len.*=.*", "log_errors_max_len = " + log_errors_max_len);
			}
			if (!String::IsNullOrEmpty(ignore_repeated_errors)) {
				utils->iniReplace(content_file, "^ignore_repeated_errors.*=.*", "ignore_repeated_errors = " + ignore_repeated_errors);
			}
			if (!String::IsNullOrEmpty(html_errors)) {
				utils->iniReplace(content_file, "^html_errors.*=.*", "html_errors = " + html_errors);
			}
			if (!String::IsNullOrEmpty(error_log)) {
				utils->iniReplace(content_file, "^error_log.*=.*", "error_log = " + error_log);
			}

		
			if (!String::IsNullOrEmpty(short_open_tag)) {
				utils->iniReplace(content_file, "^short_open_tag.*=.*", "short_open_tag = " + short_open_tag);
			}
			if (!String::IsNullOrEmpty(output_buffering)) {
				utils->iniReplace(content_file, "^output_buffering.*=.*", "output_buffering = " + output_buffering);
			}
			if (!String::IsNullOrEmpty(precision)) {
				utils->iniReplace(content_file, "^precision.*=.*", "precision = " + precision);
			}
			if (!String::IsNullOrEmpty(ignore_user_abort)) {
				utils->iniReplace(content_file, "^ignore_user_abort.*=.*", "ignore_user_abort = " + ignore_user_abort);
			}
			if (!String::IsNullOrEmpty(default_charset)) {
				utils->iniReplace(content_file, "^default_charset.*=.*", "default_charset = " + default_charset);
			}
			if (!String::IsNullOrEmpty(date_timezone)) {
				utils->iniReplace(content_file, "^date.timezone.*=.*", "date.timezone = " + date_timezone);
			}
			if (!String::IsNullOrEmpty(bcmath_scale)) {
				utils->iniReplace(content_file, "^bcmath.scale.*=.*", "bcmath.scale = " + bcmath_scale);
			}
			if (!String::IsNullOrEmpty(allow_url_fopen)) {
				utils->iniReplace(content_file, "^allow_url_fopen.*=.*", "allow_url_fopen = " + allow_url_fopen);
			}
			if (!String::IsNullOrEmpty(auto_globals_jit)) {
				utils->iniReplace(content_file, "^auto_globals_jit.*=.*", "auto_globals_jit = " + auto_globals_jit);
			}
			if (!String::IsNullOrEmpty(memory_limit)) {
				utils->iniReplace(content_file, "^memory_limit.*=.*", "memory_limit = " + memory_limit + "M");
			}
			if (!String::IsNullOrEmpty(max_input_time)) {
				utils->iniReplace(content_file, "^max_input_time.*=.*", "max_input_time = " + max_input_time);
			}
			if (!String::IsNullOrEmpty(max_execution_time)) {
				utils->iniReplace(content_file, "^max_execution_time.*=.*", "max_execution_time = " + max_execution_time);
			}
			if (!String::IsNullOrEmpty(post_max_size)) {
				utils->iniReplace(content_file, "^post_max_size.*=.*", "post_max_size = " + post_max_size + "M");
			}
			if (!String::IsNullOrEmpty(upload_max_filesize)) {
				utils->iniReplace(content_file, "^upload_max_filesize.*=.*", "upload_max_filesize = " + upload_max_filesize + "M");
			}
			if (!String::IsNullOrEmpty(max_file_uploads)) {
				utils->iniReplace(content_file, "^max_file_uploads.*=.*", "max_file_uploads = " + max_file_uploads);
			}

			if (!String::IsNullOrEmpty(session_save_handler)) {
				utils->iniReplace(content_file, "^session.save_handler.*=.*", "session.save_handler = " + session_save_handler);
			}


			if (!String::IsNullOrEmpty(session_save_path)) {
				bool session_save_path_status = utils->iniReplace(content_file, "^session.save_path.*=.*", "session.save_path = \"" + session_save_path + "\"");
				if (!session_save_path_status) {
					session_save_path_status = utils->iniReplace(content_file, "^;session.save_path.*=.*", "session.save_path = \"" + session_save_path + "\"");
					if (!session_save_path_status) {
						utils->iniReplace(content_file, "^; session.save_path.*=.*", "session.save_path = \"" + session_save_path + "\"");
					}
				}
			}
			if (!String::IsNullOrEmpty(session_use_cookies)) {
				utils->iniReplace(content_file, "^session.use_cookies.*=.*", "session.use_cookies = " + session_use_cookies);
			}
			if (!String::IsNullOrEmpty(session_name)) {
				utils->iniReplace(content_file, "^session.name.*=.*", "session.name = " + session_name);
			}
			if (!String::IsNullOrEmpty(session_auto_start)) {
				utils->iniReplace(content_file, "^session.auto_start.*=.*", "session.auto_start = " + session_auto_start);
			}
			if (!String::IsNullOrEmpty(session_cookie_lifetime)) {
				utils->iniReplace(content_file, "^session.cookie_lifetime.*=.*", "session.cookie_lifetime = " + session_cookie_lifetime);
			}
			if (!String::IsNullOrEmpty(session_cookie_path)) {
				utils->iniReplace(content_file, "^session.cookie_path.*=.*", "session.cookie_path = " + session_cookie_path);
			}
			if (!String::IsNullOrEmpty(session_cookie_domain)) {
				utils->iniReplace(content_file, "^session.cookie_domain.*=.*", "session.cookie_domain = " + session_cookie_domain);
			}
			if (!String::IsNullOrEmpty(session_cache_expire)) {
				utils->iniReplace(content_file, "^session.cache_expire.*=.*", "session.cache_expire = " + session_cache_expire);
			}

			if (!String::IsNullOrEmpty(extensions)) {
				array<String^> ^extension_list = extensions->Split(',');
				utils->iniReplace(content_file, "^extension =.*", nullptr); // Tum extensionlar pasif hale getiriliyor
				for (int i = 0; i < extension_list->Length; i++) {
					bool extension_list_status = utils->iniReplace(content_file, "^extension =.*" + extension_list[i], "extension = " + extension_list[i]);
					if (!extension_list_status) {
						utils->iniReplace(content_file, "^;extension =.*", "extension = " + extension_list[i]);
					}
				}
			}

			
			//Console::WriteLine(content_file);
			utils->writeFile(domain_ini_file_path, content_file);
			logger->infoLog("php config file writed", 4001);


			String ^path = "C:\\fpanel\\php\\"+ phpver->Replace("php", "") +"\\php-cgi.exe";

			try {
				String ^output = utils->exec("wmic", "process get ProcessID,ExecutablePath");
				array<String^> ^output_list = output->Replace("\r", "")->Split('\n');
				for (int i = 0; i < output_list->Length; i++) {
					array<String^> ^proc_array = output_list[i]->Split(' ');
					if (proc_array->Length > 1) {
						String ^proc_name = proc_array[0];
						String ^proc_id = proc_array[proc_array->Length - 1];
						if (String::Equals(proc_name, path, StringComparison::OrdinalIgnoreCase)) {
							Process ^proc = Process::GetProcessById(Convert::ToInt32(proc_id));
							proc->Kill();
						}
					}
				}
			}
			catch (...) {
			}

			
		}
		catch (Exception ^e) {
			logger->debugLog("phpmng: " + e->Message, 1801);
		}
	}

	void phpmng::show()
	{
		if (validate->checkFile(env->panel_domain_configs_dir + domain + "\\php\\php.ini") == true) {
			Console::WriteLine(File::ReadAllText(env->panel_domain_configs_dir + domain + "\\php\\php.ini"));
		}
	}

	void phpmng::getModules()
	{
		String ^extension_names;
		array<String^> ^all_extensions = Directory::GetFiles(env->panel_dir + "php\\" + phpver + "\\ext", "*");
		for (int i = 0; i < all_extensions->Length; i++) {
			String ^file_name = all_extensions[i]->Replace(env->panel_dir + "php\\" + phpver + "\\ext\\", "");
			try {
				String ^extension_ext = file_name->Split('.')[1];
				if (String::Equals(extension_ext, "dll")) {
					extension_names += file_name->Split('.')[0]->Replace("php_", "") + ",";
				}
			}
			catch (...) {
			}
		}

		Console::WriteLine(extension_names);

	}

	void phpmng::setSystem()
	{
		try {
			if (String::IsNullOrEmpty(display_errors)) {

			}
			if (String::IsNullOrEmpty(display_startup_errors)) {

			}
			if (String::IsNullOrEmpty(error_reporting)) {

			}
			if (String::IsNullOrEmpty(log_errors)) {

			}
			if (String::IsNullOrEmpty(log_errors_max_len)) {

			}
			if (String::IsNullOrEmpty(ignore_repeated_errors)) {

			}
			if (String::IsNullOrEmpty(html_errors)) {

			}
			if (String::IsNullOrEmpty(error_log)) {

			}

			if (String::IsNullOrEmpty(extensions)) {

			}

			if (String::IsNullOrEmpty(short_open_tag)) {

			}
			if (String::IsNullOrEmpty(output_buffering)) {

			}
			if (String::IsNullOrEmpty(precision)) {

			}
			if (String::IsNullOrEmpty(ignore_user_abort)) {

			}
			if (String::IsNullOrEmpty(default_charset)) {

			}
			if (String::IsNullOrEmpty(date_timezone)) {

			}
			if (String::IsNullOrEmpty(bcmath_scale)) {

			}
			if (String::IsNullOrEmpty(allow_url_fopen)) {

			}
			if (String::IsNullOrEmpty(auto_globals_jit)) {

			}
			if (String::IsNullOrEmpty(memory_limit)) {

			}
			if (String::IsNullOrEmpty(max_input_time)) {

			}
			if (String::IsNullOrEmpty(max_execution_time)) {

			}
			if (String::IsNullOrEmpty(post_max_size)) {

			}
			if (String::IsNullOrEmpty(upload_max_filesize)) {

			}
			if (String::IsNullOrEmpty(max_file_uploads)) {

			}

			if (String::IsNullOrEmpty(session_save_handler)) {

			}
			if (String::IsNullOrEmpty(session_save_path)) {

			}
			if (String::IsNullOrEmpty(session_use_cookies)) {

			}
			if (String::IsNullOrEmpty(session_name)) {

			}
			if (String::IsNullOrEmpty(session_auto_start)) {

			}
			if (String::IsNullOrEmpty(session_cookie_lifetime)) {

			}
			if (String::IsNullOrEmpty(session_cookie_path)) {

			}
			if (String::IsNullOrEmpty(session_cookie_domain)) {

			}
			if (String::IsNullOrEmpty(session_cache_expire)) {

			}
		}
		catch (Exception ^e) {
			logger->debugLog("phpmng: " + e->Message, 1804);
		}

	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	phpmng ^mainProcess = gcnew phpmng();
	mainProcess->Main(Args);
	return 0;
}