#include "websrvmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void websrvmng::Main(array<String^> ^Args)
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
		default_language = main_case["default-lang"];
		connection_string = main_case["con-string"];
		connection_string_name = main_case["con-string-name"];
		connection_string_provider = main_case["con-string-prov"];
		custom_error_mode = main_case["error-mode"];
		status_code = main_case["status-code"];
		status_code_remove = main_case["status-code-remove"];
		status_url = main_case["status-url"];
		compilation_debug = main_case["com-debug"];
		default_language = main_case["default-lang"];
		culture = main_case["culture"];
		file_encoding = main_case["file-encode"];
		request_encoding = main_case["request-encode"];
		response_encoding = main_case["response-encode"];
		uiculture = main_case["ui-culture"];
		trust_level = main_case["trust-level"];
		session_auth = main_case["sess-auth"];
		session_timeout = main_case["sess-timeout"];
		file_extension = main_case["file-ext"];
		file_extension_octet = main_case["file-ext-octet"];
		directory_browsing = main_case["dir-browsing"];
		access_policy = main_case["acc-policy"];
		logging_visits = main_case["log-visits"];
		parent_path = main_case["parent-path"];
		execute_in_mta = main_case["exec-mta"];
		default_document_status = main_case["def-doc-status"];
		default_document_files_remove = main_case["def-files-remove"];
		default_document_files = main_case["def-files"];
		dir_ssl_flags = main_case["dir-ssl-flags"];
		anony_auth_status = main_case["anony-auth-status"];
		buffering_limit = main_case["buff-limit"];
		maxrequest_entity_allowed = main_case["req-ent-allowed"];
		appallow_clientdebug = main_case["appallow-client-debug"];
		appallow_debugging = main_case["appallow-debug"];
		physical_path = main_case["physical-path"];
		virtual_dir_name = main_case["virtual-dir-name"];
		virtual_dir_mode = main_case["virtual-dir-mode"];
		data = main_case["data"];

		aspnetver = main_case["asp-net-ver"];
		hellomessage = main_case["hellomessage"];
		hiddensegments = main_case["hiddensegments"];
		dynamiccomp = main_case["dynamiccomp"];
		staticcomp = main_case["staticcomp"];
		filesizecomp = main_case["filesizecomp"];
		cachedir = main_case["cachedir"];
		diskspace = main_case["diskspace"];
		diskspacelimit = main_case["diskspacelimit"];

		keep_alive = main_case["keep-alive"];
		keep_alive_timeout = main_case["keep-alive-timeout"];
		server_timeout = main_case["server-timeout"];
		max_keep_alive_req = main_case["max-keep-alive-req"];
		start_server = main_case["start-server"];
		min_spare_server = main_case["min-spare-server"];
		max_clients = main_case["max-clients"];
		max_req_per_child = main_case["max-req-per-child"];

		if (process == "init") {
			init();
		}
		else if (process == "aspnet-settings") {
			aspnetSettings();
		}
		else if (process == "list-modules") {
			listModules();
		}
		else if (process == "list-mimetypes") {
			listMimeTypes();
		}
		else if (process == "server-setting") {
			serverSettings();
		}
		else if (process == "save-setting") {
			saveServerSettings();
		}
		else if (process == "add-mimetype") {
			addMimeType();
		}
		else if (process == "remove-mimetype") {
			removeMimeType();
		}
		else if (process == "init-log") {
			initIisLog();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void websrvmng::init()
	{
		utils->exec("netsh", "advfirewall set global StatefulFtp enable");
		webserver->initIIS();
	}

	void websrvmng::listModules()
	{
		try {
			lists->settings(nullptr, "modules", nullptr);
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1802);
		}
	}

	void websrvmng::listMimeTypes()
	{
		try {
			lists->settings(nullptr, "mimetype", nullptr);
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1803);
		}
	}

	void websrvmng::serverSettings()
	{
		try {
			iis_setting->iisWebServerSettingsList();
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1804);
		}
	}

	void websrvmng::aspnetSettings()
	{
		try {
			if (culture != nullptr)
				culture = "/culture:" + culture;
			if (file_encoding != nullptr)
				file_encoding = "/fileencoding:" + file_encoding;
			if (request_encoding != nullptr)
				request_encoding = "/requestencoding:" + request_encoding;
			if (response_encoding != nullptr)
				response_encoding = "/responseencoding:" + response_encoding;
			if (uiculture != nullptr)
				uiculture = "/uiculture:" + uiculture;

			if (domain == nullptr) {
				if (default_language != nullptr) {
					utils->exec(env->appcmd, "set config /section:compilation /defaultLanguage:\"" + default_language + "\" /clr:" + aspnetver + " /commit:apphost");
				}
				if (compilation_debug != nullptr) {
					utils->exec(env->appcmd, "set config /section:system.web/compilation /debug:\"" + compilation_debug + "\" /clr:" + aspnetver + " /commit:apphost");
				}
				if ((culture != nullptr) || (file_encoding != nullptr) || (request_encoding != nullptr) || (response_encoding != nullptr) || (uiculture != nullptr)) {
					utils->exec(env->appcmd, "set config /section:system.web/globalization " + culture + " " + file_encoding + " " + request_encoding + " " + response_encoding + " " + uiculture + " /clr:" + aspnetver + " /commit:apphost");
				}
				if (custom_error_mode != nullptr) {
					utils->exec(env->appcmd, "set config /section:system.web/customErrors /mode:" + custom_error_mode + " /clr:" + aspnetver + " /commit:apphost");
				}
				if (trust_level != nullptr) {
					utils->exec(env->appcmd, "set config /section:trust /level:\"" + trust_level + "\" /clr:" + aspnetver + " /commit:apphost");
				}
				if (session_auth != nullptr) {
					utils->exec(env->appcmd, "set config /section:system.web/authentication /mode:" + session_auth + " /clr:" + aspnetver + " /commit:apphost");
				}
				if (session_timeout != nullptr) {
					utils->exec(env->appcmd, "set config /section:system.web/sessionState /timeout:" + session_timeout + " /clr:" + aspnetver + " /commit:apphost");
				}
			}
			else {
				if (default_language != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:compilation /defaultLanguage:\"" + default_language + "\" /commit:apphost");
				}
				if (compilation_debug != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:system.web/compilation /debug:\"" + compilation_debug + "\" /commit:apphost");
				}
				if ((culture != nullptr) || (file_encoding != nullptr) || (request_encoding != nullptr) || (response_encoding != nullptr) || (uiculture != nullptr)) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:system.web/globalization " + culture + " " + file_encoding + " " + request_encoding + " " + response_encoding + " " + uiculture + " /commit:apphost");
				}
				if (custom_error_mode != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:system.web/customErrors /mode:\"" + custom_error_mode + "\" /commit:apphost");
				}
				if (trust_level != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:trust /level:\"" + trust_level + "\" /commit:apphost");
				}
				if (session_auth != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:system.web/authentication /mode:\"" + session_auth + "\" /commit:apphost");
				}
				if (session_timeout != nullptr) {
					utils->exec(env->appcmd, "set config \"" + domain + "\" /section:system.web/sessionState /timeout:\"" + session_timeout + "\" /commit:apphost");
				}
			}
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1806);
		}
	}

	void websrvmng::saveServerSettings()
	{
		iis_setting->saveIISsetting(dynamiccomp, staticcomp, filesizecomp, cachedir, diskspace, diskspacelimit);
	}

	void websrvmng::addMimeType()
	{
		if ((file_extension == nullptr) || (file_extension_octet == nullptr)) {
			logger->errorLog("Missing arguments", 7101);
			System::Environment::Exit(7101);
		}
		webserver->addMimeType(file_extension, file_extension_octet);

		logger->infoLog("Mime type added", 7101);
	}

	void websrvmng::removeMimeType()
	{
		try {
			if ((file_extension == nullptr)) {
				logger->errorLog("Missing arguments", 7101);
				System::Environment::Exit(7101);
			}
			webserver->removeMimeType(file_extension, file_extension_octet);
			logger->infoLog("Mime type removed", 7102);
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1809);
		}
	}

	void websrvmng::initIisLog()
	{
		try {
			Console::WriteLine("");
			//				string system_xml = @"<?xml version=""1.0"" encoding=""UTF-8""?>
			//								<appcmd>
			//									<CONFIG CONFIG.SECTION=""system.webServer/advancedLogging/server"" path=""MACHINE/WEBROOT/APPHOST"" overrideMode=""Inherit"" locked=""false"">
			//										<system.webServer-advancedLogging-server enabled=""true"">
			//											<logDefinitions>
			//												<logDefinition rollLogFileOnConfigChanges=""false"" writeLogDataToDisk=""true"" baseFileName=""%COMPUTERNAME%-Server"" logRaiseTraceEvents=""false"" enabled=""true"" logRollOption=""Schedule"" maxDurationSeconds=""86400"" maxFileSizeKB=""1024"" schedule=""Daily"" publishLogEvent=""true"">
			//													<selectedFields>
			//														<logField id=""Site Name"" logHeaderName=""s-sitename"" required=""false"" defaultValue="""" />
			//														<logField id=""Bytes Sent"" logHeaderName=""sc-bytes"" required=""false"" defaultValue="""" />
			//													</selectedFields>
			//													<filter>
			//														<condition operator=""And"">
			//														</condition>
			//													</filter>
			//												</logDefinition>
			//											</logDefinitions>
			//										</system.webServer-advancedLogging-server>
			//									</CONFIG>
			//									<CONFIG CONFIG.SECTION=""system.webServer/advancedLogging/client"" path=""MACHINE/WEBROOT/APPHOST"" overrideMode=""Inherit"" locked=""false"">
			//										<system.webServer-advancedLogging-client enabled=""true"" />
			//									</CONFIG>
			//								</appcmd>";

			//				utils.exec(env.appcmd, @"set config  -section:system.webServer/advancedLogging/server /-""logDefinitions.[baseFileName='%COMPUTERNAME%-Server']"" /commit:apphost");
			//				utils.exec("echo", system_xml + @" | " + env.appcmd + " set config /in");
		}
		catch (Exception ^e) {
			logger->debugLog("websrvmng: " + e->Message, 1810);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	websrvmng ^mainProcess = gcnew websrvmng();
	mainProcess->Main(Args);
	return 0;
}