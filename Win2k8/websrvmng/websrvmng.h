#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/WebServer.h"
#include "../Windows/Headers/Lists.h"
#include "../Windows/Headers/IISSettings.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class websrvmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static FPanel::Windows::WebServer ^webserver = gcnew FPanel::Windows::WebServer();
		static FPanel::Windows::Lists ^lists = gcnew FPanel::Windows::Lists();
		static FPanel::Windows::IISSettings ^iis_setting = gcnew FPanel::Windows::IISSettings();

		static String ^domain;
		static String ^default_language;
		static String ^connection_string;
		static String ^connection_string_name;

		static String ^connection_string_provider;
		static String ^custom_error_mode;
		static String ^status_url;
		static String ^status_code;
		static String ^status_code_remove;
		static String ^compilation_debug;
		static String ^culture;
		static String ^file_encoding;
		static String ^request_encoding;
		static String ^response_encoding;
		static String ^uiculture;
		static String ^trust_level;
		static String ^session_auth;
		static String ^session_timeout;
		static String ^file_extension;
		static String ^file_extension_octet;
		static String ^directory_browsing;
		static String ^access_policy;
		static String ^logging_visits;
		static String ^parent_path;
		static String ^execute_in_mta;
		static String ^default_document_status;
		static String ^default_document_files_remove;
		static String ^default_document_files;
		static String ^dir_ssl_flags;
		static String ^anony_auth_status;
		static String ^buffering_limit;
		static String ^maxrequest_entity_allowed;
		static String ^appallow_clientdebug;
		static String ^appallow_debugging;
		static String ^physical_path;
		static String ^virtual_dir_name;
		static String ^virtual_dir_mode;
		static String ^data;
		static String ^aspnetver;
		static String ^hellomessage;
		static String ^hiddensegments;
		static String ^dynamiccomp;
		static String ^staticcomp;
		static String ^filesizecomp;
		static String ^cachedir;
		static String ^diskspace;
		static String ^diskspacelimit;

		static String ^keep_alive;
		static String ^keep_alive_timeout;
		static String ^server_timeout;
		static String ^max_keep_alive_req;
		static String ^start_server;
		static String ^min_spare_server;
		static String ^max_clients;
		static String ^max_req_per_child;

		static void init();

		static void listModules();

		static void listMimeTypes();

		static void serverSettings();

		static void aspnetSettings();

		static void saveServerSettings();

		static void addMimeType();

		static void removeMimeType();

		static void initIisLog();
	};
}
