#include "subvhostmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void subvhostmng::Main(array<String^> ^Args)
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
		prefix = main_case["prefix"];
		user = main_case["user"];
		subdomain = main_case["subdomain"];
		new_user = main_case["new-user"];
		ipaddr = main_case["ipaddr"];
		old_ipaddr = main_case["old-ipaddr"];
		ssl = main_case["ssl"];
		php = main_case["php"];
		perl = main_case["perl"];
		python = main_case["python"];
		cgi = main_case["cgi"];
		asp = main_case["asp"];
		aspnet = main_case["aspnet"];
		asp_net_core = main_case["asp-net-core"];
		aspnet_ver = main_case["aspnet-ver"];
		php_ver = main_case["php-ver"];
		passwd = main_case["passwd"];
		vhost->setEnv(env);

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
		else {
			System::Environment::Exit(0);
		}
	}

	void subvhostmng::create()
	{
		try {
			if ((domain == nullptr) || (subdomain == nullptr) || (prefix == nullptr) || (ipaddr == nullptr)) {
				logger->errorLog("Missing arguments", 2401);
				System::Environment::Exit(2401);
			}
			String ^path = env->vhosts_dir + prefix + "\\subdomains\\" + subdomain;
			domain = subdomain + "." + domain;

			if (php->Equals("on")) {
				utils->createFolder(env->vhosts_dir + prefix + "\\vhost\\sessions");
				utils->copyFile(env->panel_dir + "php\\php_ver\\php.ini", env->panel_domain_configs_dir + "domains\\" + domain + "\\php\\php.ini");
				utils->exec(env->phpmng, "--save --domain=" + domain + " --phpver=" + php_ver + " --session_save_path=" + env->vhosts_dir + prefix + "\\vhost\\sessions");
			}
			vhost->createSite(domain, nullptr, nullptr, path, ipaddr, nullptr, asp, php, aspnet, cgi, perl, python, php_ver, aspnet_ver, user, passwd, prefix, asp_net_core);
		}
		catch (Exception ^e) {
			logger->debugLog("subvhostmng: " + e->Message, 1801);
		}
	}

	void subvhostmng::update()
	{
		try {
			if ((domain == nullptr) || (subdomain == nullptr) || (prefix == nullptr)) {
				logger->errorLog("Missing arguments", 2401);
				System::Environment::Exit(2401);
			}

			if (ipaddr != nullptr) {
				String ^path = env->vhosts_dir + prefix + "\\subdomains\\" + subdomain;
				domain = subdomain + "." + domain;
				vhost->updateSite(domain, nullptr, nullptr, ipaddr, asp, php, aspnet, cgi, perl, python, php_ver, aspnet_ver, nullptr, user, passwd, prefix, asp_net_core);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("subvhostmng: " + e->Message, 1802);
		}
	}

	void subvhostmng::remove()
	{
		try {
			if ((domain == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2401);
				System::Environment::Exit(2401);
			}
			domain = subdomain + "." + domain;
			vhost->removeSite(domain);

		}
		catch (Exception ^e) {
			logger->debugLog("subvhostmng: " + e->Message, 1803);
		}
	}

	void subvhostmng::suspend()
	{
		try {
			if ((domain == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2401);
				System::Environment::Exit(2401);
			}
			domain = subdomain + "." + domain;
			vhost->startStopSite(domain, "suspend");
		}
		catch (Exception ^e) {
			logger->debugLog("subvhostmng: " + e->Message, 1804);
		}
	}

	void subvhostmng::unsuspend()
	{
		try {
			if ((domain == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2401);
				System::Environment::Exit(2401);
			}
			domain = subdomain + "." + domain;
			vhost->startStopSite(domain, "unsuspend");
		}
		catch (Exception ^e) {
			logger->debugLog("subvhostmng: " + e->Message, 1805);
		}


	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	subvhostmng ^mainProcess = gcnew subvhostmng();
	mainProcess->Main(Args);
	return 0;
}