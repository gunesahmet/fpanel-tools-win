#include "vhostmng.h"

using namespace System;
using namespace System::IO;
using namespace Microsoft::Web::Administration;

namespace Tools
{
	void vhostmng::Main(array<String^> ^Args)
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
		vhost->setEnv(env);
		vdir->setEnv(env);
		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");

		user = main_case["user"];
		prefix = main_case["prefix"];
		passwd = main_case["passwd"];
		domain = main_case["domain"];
		ipaddr = main_case["ipaddr"];
		ssl = main_case["ssl"];
		ssl_name = main_case["ssl-name"];
		parentpaths = main_case["parentpaths"];
		limit = main_case["limit"];
		php = main_case["php"];
		perl = main_case["perl"];
		python = main_case["python"];
		cgi = main_case["cgi"];
		asp = main_case["asp"];
		aspnet = main_case["aspnet"];
		asp_net_core = main_case["asp-net-core"];
		aspnet_ver = main_case["aspnet-ver"];
		php_ver = main_case["php-ver"];
		appool = main_case["appool"];
		pool_mode = main_case["pool-mode"];

		acc_policy = main_case["acc-policy"];
		domain = main_case["domain"];
		virtual_dir_name = main_case["virtual-dir-name"];
		physical_path = main_case["physical-path"];
		appallow_client_debug = main_case["appallow-client-debug"];
		appallow_debug = main_case["appallow-debug"];
		buff_limit = main_case["buff-limit"];
		req_ent_allowed = main_case["req-ent-allowed"];
		virtual_dir_mode = main_case["virtual-dir-mode"];
		anony_auth_status = main_case["anony-auth-status"];
		con_string_prov = main_case["con-string-prov"];
		def_doc_order = main_case["def-file-order"];
		dir_ssl_flag = main_case["dir-ssl-flag"];

		setting_type = main_case["setting-type"];

		dir_browsing = main_case["dir-browsing"];
		log_visits = main_case["log-visits"];
		parent_path = main_case["parent-path"];
		exec_mta = main_case["exec-mta"];
		doc_status = main_case["doc-status"];
		def_file = main_case["def-file"];

		error_mode = main_case["error-mode"];
		default_lang = main_case["default-lang"];
		com_debug = main_case["com-debug"];
		request_encode = main_case["request-encode"];
		response_encode = main_case["response-encode"];
		file_encode = main_case["file-encode"];
		culture = main_case["culture"];
		ui_culture = main_case["ui-culture"];
		trust_level = main_case["trust-level"];
		sess_auth = main_case["sess-auth"];
		sess_timeout = main_case["sess-timeout"];
		status_code = main_case["status-code"];
		status_url = main_case["status-url"];
		con_string_name = main_case["con-string-name"];
		con_string = main_case["con-string"];

		if (process == "create") {
			create();
		}
		else if (process == "update") {
			update();
		}
		else if (process == "remove") {
			remove();
		}
		else if (process == "suspend") {
			suspend();
		}
		else if (process == "unsuspend") {
			unsuspend();
		}
		else if (process == "create-vdir") {
			createVdir();
		}
		else if (process == "update-vdir") {
			updateVdir();
		}
		else if (process == "remove-vdir") {
			removeVdir();
		}
		else if (process == "list-setting") {
			listSetting();
		}
		else if (process == "save-vdir-setting") {
			saveVdirSetting();
		}
		else if (process == "aspnet-setting") {
			aspNetSetting();
		}
		else if (process == "conn-remove") {
			connRemove();
		}
		else if (process == "error-remove") {
			errorRemove();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void vhostmng::create()
	{
		if ((domain == nullptr) || (prefix == nullptr) || (user == nullptr) || (ipaddr == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}

		if (validate->isDomainExistsIis(domain) == true) {
			logger->errorLog(domain + " already exists", 2702);
			System::Environment::Exit(2702);
		}
		String ^path = env->vhosts_dir + prefix;
		vhost->createSite(domain, appool, pool_mode, path, ipaddr, limit, asp, php, aspnet, cgi, perl, python, php_ver, aspnet_ver, user, passwd, prefix, asp_net_core);
		if (php->Equals("on")) {
			utils->createFolder(env->vhosts_dir + prefix + "\\vhost\\sessions");
			utils->copyFile(env->panel_dir + "php\\php_ver\\php.ini", env->panel_domain_configs_dir + "domains\\" + domain + "\\php\\php.ini");
			utils->exec(env->phpmng, "--save --domain=" + domain + " --phpver=" + php_ver + " --session_save_path=" + env->vhosts_dir + prefix + "\\vhost\\sessions");
		}
		utils->exec(env->pagemng, "--set --domain=" + domain + " --user=" + user + " --asp=" + asp + " --aspnet=" + aspnet + " --php=" + php + " --perl=" + perl + " --cgi=" + cgi + " --python=" + python + " --prefix=" + prefix);
		logger->infoLog(domain + " created", 2701);
	}

	void vhostmng::update()
	{
		if ((domain == nullptr) || (prefix == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->checkUser(user) == false) {
			logger->errorLog(user + " not found", 2704);
			System::Environment::Exit(2704);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2705);
			System::Environment::Exit(2705);
		}
		String ^path = env->vhosts_dir + prefix;

		if (ipaddr != nullptr)
			vhost->updateSite(domain, appool, pool_mode, ipaddr, asp, php, aspnet, cgi, perl, python, php_ver, aspnet_ver, limit, user, nullptr, prefix, asp_net_core);

		logger->infoLog(domain + " updated", 2702);
	}

	void vhostmng::remove()
	{
		try {
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Missing arguments", 2701);
				System::Environment::Exit(2701);
			}

			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 2702);
				System::Environment::Exit(2702);
			}
			vhost->removeSite(domain);
			logger->infoLog(domain + " removed", 2703);
		}
		catch (Exception ^e) {
			logger->errorLog(e->Message, 1801);
			logger->debugLog("Error: " + e->Message + " Line: " + FExceptions::LineNumber(e).ToString(), 999);
		}
	}

	void vhostmng::suspend()
	{
		if ((domain == nullptr) || (user == nullptr) || (ipaddr == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}

		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		vhost->startStopSite(domain, "suspend");
		logger->infoLog(domain + " suspended", 2704);
	}

	void vhostmng::unsuspend()
	{
		if ((domain == nullptr) || (user == nullptr) || (ipaddr == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		vhost->startStopSite(domain, "unsuspend");
		logger->infoLog(domain + " unsuspended", 2705);
	}

	void vhostmng::createVdir()
	{
		if ((domain == nullptr) || (prefix == nullptr) || (virtual_dir_name == nullptr) || (virtual_dir_mode == nullptr) || (physical_path == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}

		vdir->createVirtualDirectory(domain, virtual_dir_name, physical_path, virtual_dir_mode, prefix);
		vdir->virtualDirectorySettings(domain, virtual_dir_name, acc_policy, anony_auth_status, buff_limit, req_ent_allowed, appallow_client_debug, appallow_debug);
	}

	void vhostmng::updateVdir()
	{
		if ((domain == nullptr) || (virtual_dir_name == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		vdir->virtualDirectorySettings(domain, virtual_dir_name, acc_policy, anony_auth_status, buff_limit, req_ent_allowed, appallow_client_debug, appallow_debug);
	}

	void vhostmng::removeVdir()
	{
		if ((domain == nullptr) || (virtual_dir_name == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		vdir->removeVirtualDirectory(domain, virtual_dir_name, virtual_dir_mode);
	}

	void vhostmng::listSetting()
	{
		if ((domain == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		lists->settings(domain, setting_type, virtual_dir_name);
	}

	void vhostmng::saveVdirSetting()
	{
		if ((domain == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		// , , , , , , , , defaultDocumentOrder, directorySslFlag, , , , , 
		vdir->setEnv(env);

		if (virtual_dir_name != "/") {
			/*
			virtualDirectorySettings(String ^siteName, String ^vDirName, String ^accessPolicy, String ^anonymousAuthentication,
				String ^aspBufferLimit, String ^maxRequestEntityAllowed, String ^appAllowClientDebug, String ^appAllowDebugging)
			*/
			vdir->virtualDirectorySettings(domain,
				virtual_dir_name,
				acc_policy,
				anony_auth_status,
				buff_limit,
				req_ent_allowed,
				appallow_client_debug,
				appallow_debug);
		}
		else {
			/**
			virtualDirectoryGeneralSettings(String ^siteName, String ^directoryBrowse, String ^accessPolicy, String ^dontLog,
			String ^enableParentPath, String ^executeInMTA, String ^defaultDocStatus, String ^defaultDocument, String ^defaultDocOrder,
			String ^directorySslFlag, String ^anonymousAuthStatus, String ^aspBufferLimit, String ^maxRequestEntityAllowed,
			String ^appAllowClientDebug, String ^appAllowDebugging)
			*/
			vdir->virtualDirectoryGeneralSettings(domain,
				dir_browsing,
				acc_policy,
				log_visits,
				parent_path,
				exec_mta,
				doc_status,
				def_file,
				def_doc_order,
				dir_ssl_flag,
				anony_auth_status,
				buff_limit,
				req_ent_allowed,
				appallow_client_debug,
				appallow_debug);
		}
	}

	void vhostmng::aspNetSetting()
	{
		if ((domain == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		/**
		 * default-lang
		 * com-debug
		 * culture
		 * file-encode
		 * request-encode
		 *
		 *
		*/

		/**
		 *
		 String ^siteName,
		 String ^defaultLanguage,
		 String ^switchDebug,
		 String ^culture,
		 String ^fileEncoding,
		 String ^requestEncoding,
		 String ^responseEncoding,
		 String ^uiCulture,
		 String ^customErrorMode,
		 String ^trustLevel,
		 String ^sessionAuth,
		 String ^sessionTimeout,

		 String ^connectionString,
		 String ^connectionStringName,
		 String ^connectionStringProvider,
		 String ^statusCode,
		 String ^redirectUrl
		*/
		asp_net_domain->save(domain, // +
			default_lang, // +
			com_debug,  // +
			culture, // +
			file_encode, // +
			request_encode, // +
			response_encode, // +
			ui_culture, // +
			error_mode, // +
			trust_level, // +
			sess_auth, // +
			sess_timeout, // +
			con_string,  // +
			con_string_name, // +
			con_string_prov, // ?
			status_code, // +
			status_url); // +
	}

	void vhostmng::connRemove()
	{
		if ((domain == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		asp_net_domain->removeConnectionString(domain, con_string_name);
	}

	void vhostmng::errorRemove()
	{
		if ((domain == nullptr)) {
			logger->errorLog("Missing arguments", 2701);
			System::Environment::Exit(2701);
		}
		if (validate->isDomainExistsIis(domain) == false) {
			logger->errorLog(domain + " not found", 2702);
			System::Environment::Exit(2702);
		}
		asp_net_domain->removeCustomError(domain, status_code);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	vhostmng ^mainProcess = gcnew vhostmng();
	mainProcess->Main(Args);
	return 0;
}