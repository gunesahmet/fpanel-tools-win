#include "subdirectorymng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void subdirectorymng::Main(array<String^> ^Args)
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
		ftp_type = main_case["ftp-type"];
		subdomain = main_case["subdomain"];
		sub_user = main_case["sub-user"];
		new_sub_user = main_case["new-sub-user"];


		if (process == "create") {
			create();
		}
		else if (process == "update") {
			update();
		}
		else if (process == "remove") {
			remove();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void subdirectorymng::create()
	{
		try {
			if ((domain == nullptr) || (prefix == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2101);
				System::Environment::Exit(2101);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 2102);
				System::Environment::Exit(2102);
			}
			if (validate->checkDirectory(env->vhosts_dir + prefix) == false) {
				logger->errorLog(domain + " directory not found", 2103);
				System::Environment::Exit(2103);
			}
			if (validate->checkDirectory(env->vhosts_dir + prefix + "\\subdomains\\" + subdomain) == true) {
				logger->errorLog(subdomain + " directory already exists", 2104);
				System::Environment::Exit(2104);
			}
			String ^path = env->vhosts_dir + prefix;
			String ^sub_path = path + "\\subdomains\\" + subdomain;

			String ^ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + user;
			String ^sub_ftp_path;

			if (ftp_type == "native") {
				sub_ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + sub_user;
			}

			sub_directories->create(sub_path, user, sub_user, sub_ftp_path, ftp_type);
			logger->infoLog(subdomain + "." + domain + " created", 2101);

		}
		catch (Exception ^e) {
			logger->debugLog("subdirectorymng: " + e->Message, 1801);
		}
	}

	void subdirectorymng::update()
	{
		try {
			if ((domain == nullptr) || (prefix == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2105);
				System::Environment::Exit(2105);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 2106);
				System::Environment::Exit(2106);
			}

			if (validate->checkDirectory(env->vhosts_dir + prefix) == false) {
				logger->errorLog(domain + " directory not found", 2108);
				System::Environment::Exit(2108);
			}
			if (validate->checkDirectory(env->vhosts_dir + prefix + "\\subdomains\\" + subdomain) == false) {
				logger->errorLog(subdomain + " directory not found", 2109);
				System::Environment::Exit(2109);
			}
			if (new_sub_user != nullptr) {
				String ^path = env->vhosts_dir + prefix;
				String ^ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + user;

				String ^sub_path = path + "\\subdomains\\" + subdomain;
				String ^sub_ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + sub_user;

				if (ftp_type == "native") {

				}

				if (ftp_type == "native") {
					sub_directories->update(sub_path, user, sub_ftp_path, new_sub_user, sub_ftp_path); // @TODO: Duzeltilecek
				}
			}

			logger->infoLog(subdomain + "." + domain + " updated", 2102);
		}
		catch (Exception ^e) {
			logger->debugLog("subdirectorymng: " + e->Message, 1802);
		}
	}

	void subdirectorymng::remove()
	{
		try {
			if ((domain == nullptr) || (prefix == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2110);
				System::Environment::Exit(2110);
			}
			utils->removeFolder(env->vhosts_dir + prefix + "\\subdomains\\" + subdomain, true);

			if (ftp_type == "native") {
				if (sub_user != nullptr) {
					utils->removeFolder(env->ftproot_dir + "localuser\\" + sub_user, true);
				}
			}
			logger->infoLog(subdomain + "." + domain + " removed", 2103);

		}
		catch (Exception ^e) {
			logger->debugLog("subdirectorymng: " + e->Message, 1803);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	subdirectorymng ^mainProcess = gcnew subdirectorymng();
	mainProcess->Main(Args);
	return 0;
}