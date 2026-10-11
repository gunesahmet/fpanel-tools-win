#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../env/Services.h"
#include "../env/Networking.h"
#include "../env/Ini.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class phpmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Services ^services = gcnew Services();

		static Dictionary<String^, String^> ^ini_files = gcnew Dictionary<String^, String^>();
		static String ^domain;
		static String ^phpver;

		static String ^display_errors;
		static String ^display_startup_errors;
		static String ^error_reporting;
		static String ^log_errors;
		static String ^log_errors_max_len;
		static String ^ignore_repeated_errors;
		static String ^html_errors;
		static String ^error_log;
		static String ^extensions;
		static String ^short_open_tag;
		static String ^output_buffering;
		static String ^precision;
		static String ^ignore_user_abort;
		static String ^default_charset;
		static String ^date_timezone;
		static String ^bcmath_scale;
		static String ^allow_url_fopen;
		static String ^auto_globals_jit;
		static String ^memory_limit;
		static String ^max_input_time;
		static String ^max_execution_time;
		static String ^post_max_size;
		static String ^upload_max_filesize;
		static String ^max_file_uploads;
		static String ^session_save_handler;
		static String ^session_save_path;
		static String ^session_use_cookies;
		static String ^session_name;
		static String ^session_auto_start;
		static String ^session_cookie_lifetime;
		static String ^session_cookie_path;
		static String ^session_cookie_domain;
		static String ^session_cache_expire;


		static void save();

		static void show();
		
		static void getModules();

		static void setSystem();

	};
}