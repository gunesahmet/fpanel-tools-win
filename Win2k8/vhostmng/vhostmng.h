#pragma once
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/Vhost.h"
#include "../Windows/Headers/Vdir.h"
#include "../Windows/Headers/Lists.h"
#include "../Windows/Headers/AspNetDomain.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	private ref class vhostmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Environments::Env ^env = gcnew Environments::Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static FPanel::Windows::Vhost ^vhost = gcnew FPanel::Windows::Vhost();
		static FPanel::Windows::Vdir ^vdir = gcnew FPanel::Windows::Vdir();
		static FPanel::Windows::Lists ^lists = gcnew FPanel::Windows::Lists();
		static FPanel::Windows::AspNetDomain ^asp_net_domain = gcnew FPanel::Windows::AspNetDomain();

		static String ^user;
		static String ^prefix;
		static String ^passwd;
		static String ^domain;
		static String ^ipaddr;
		static String ^ssl;
		static String ^php;
		static String ^perl;
		static String ^python;
		static String ^cgi;
		static String ^asp;
		static String ^aspnet;
		static String ^asp_net_core;
		static String ^aspnet_ver;
		static String ^php_ver;
		static String ^ssl_name;
		static String ^parentpaths;
		static String ^limit;
		static String ^appool;
		static String ^pool_mode;

		static String ^acc_policy;
		static String ^virtual_dir_name;
		static String ^physical_path;
		static String ^appallow_client_debug;
		static String ^appallow_debug;
		static String ^buff_limit;
		static String ^req_ent_allowed;
		static String ^virtual_dir_mode;
		static String ^anony_auth_status;
		static String ^con_string_prov;
		static String ^def_doc_order;
		static String ^dir_ssl_flag;

		static String ^setting_type;

		static String ^dir_browsing;
		static String ^log_visits;
		static String ^parent_path;
		static String ^exec_mta;
		static String ^doc_status;
		static String ^def_file;

		static String ^error_mode;
		static String ^default_lang;
		static String ^com_debug;
		static String ^request_encode;
		static String ^response_encode;
		static String ^file_encode;
		static String ^culture;
		static String ^ui_culture;
		static String ^trust_level;
		static String ^sess_auth;
		static String ^sess_timeout;
		static String ^status_code;
		static String ^status_url;
		static String ^con_string_name;
		static String ^con_string;

		static void create();

		static void update();

		static void remove();

		static void suspend();

		static void unsuspend();

		static void createVdir();

		static void updateVdir();

		static void removeVdir();

		static void listSetting();

		static void saveVdirSetting();

		static void aspNetSetting();

		static void connRemove();

		static void errorRemove();
	};
}
