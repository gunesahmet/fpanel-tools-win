#include "subusermng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void subusermng::Main(array<String^> ^Args) {
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
		sub_user = main_case["sub-user"];
		passwd = main_case["passwd"];
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

	void subusermng::create() {
		try {

			if ((sub_user == nullptr) || (domain == nullptr) || (prefix == nullptr) || (subdomain == nullptr) || (passwd == nullptr)) {
				logger->errorLog("Missing arguments", 2301);
				System::Environment::Exit(2301);
			}
			if (validate->checkUser(sub_user)) {
				logger->errorLog(sub_user + " already exists", 2302);
				System::Environment::Exit(2302);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 2303);
				System::Environment::Exit(2303);
			}

			String ^path = env->vhosts_dir + prefix + "\\subdomains\\" + subdomain;
			users->createUser(sub_user, passwd, path, domain, subdomain, nullptr);
		}
		catch (Exception ^e) {
			logger->debugLog("subusermng: " + e->Message, 1801);
		}

	}

	void subusermng::update() {
		//try {

			if ((sub_user == nullptr) || (domain == nullptr) || (prefix == nullptr) || (subdomain == nullptr)) {
				logger->errorLog("Missing arguments", 2304);
				System::Environment::Exit(2304);
			}
			if (validate->checkUser(sub_user) == false) {
				logger->errorLog(sub_user + " not found", 2305);
				System::Environment::Exit(2305);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 2303);
				System::Environment::Exit(2303);
			}
			if (validate->isDomainExistsIis(subdomain + "." + domain) == false) {
				logger->errorLog(subdomain + "." + domain + " not found", 2307);
				System::Environment::Exit(2307);
			}
			String ^path = env->vhosts_dir + prefix + "\\subdomains\\" + subdomain;
			users->updateUser(sub_user, passwd, new_sub_user, subdomain, nullptr);
		//}
		//catch (Exception ^e) {
		//	logger->debugLog("subusermng:update " + e->Message, 1802);
		//}
	}

	void subusermng::remove() {
		try {
			if (sub_user == nullptr) {
				logger->errorLog("Missing arguments", 2304);
				System::Environment::Exit(2304);
			}
			users->removeUser(sub_user);
		}
		catch (Exception ^e) {
			logger->debugLog("subusermng: " + e->Message, 1803);
		}

	}

	void subusermng::suspend() {
		try {
			if (sub_user == nullptr) {
				logger->errorLog("Missing arguments", 2304);
				System::Environment::Exit(2304);
			}

			users->stateUser(sub_user, "suspend");

		}
		catch (Exception ^e) {
			logger->debugLog("subusermng: " + e->Message, 1804);
		}



	}

	void subusermng::unsuspend() {
		try {
			if (sub_user == nullptr) {
				logger->errorLog("Missing arguments", 2304);
				System::Environment::Exit(2304);
			}
			users->stateUser(sub_user, "unsuspend");
		}
		catch (Exception ^e) {
			logger->debugLog("subusermng: " + e->Message, 1805);
		}


	}
}

using namespace Tools;
int main(array<System::String ^> ^Args) {
	subusermng ^mainProcess = gcnew subusermng();
	mainProcess->Main(Args);
	return 0;
}