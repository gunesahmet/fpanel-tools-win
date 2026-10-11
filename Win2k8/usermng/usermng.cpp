#include "usermng.h"
using namespace System;
using namespace System::IO;

namespace Tools
{
	void usermng::Main(array<String^> ^Args)
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
		user = main_case["user"];
		prefix = main_case["prefix"];
		passwd = main_case["passwd"];
		new_passwd = main_case["new-passwd"];
		new_user = main_case["new-user"];

		if (process == "init") {
			init();
		}
		else if (process == "create") {
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

	void usermng::init()
	{
		FPanel::Windows::FileManager ^file_manager = gcnew FPanel::Windows::FileManager();
		String ^file = env->vhosts_dir;
		String ^panel_etc = env->panel_dir + "\\etc";
		String ^username = "Users";
		
		try {
			DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
			DirectoryEntry ^newGroup = local->Children->Add("fpusers", "group");
			newGroup->Invoke("Put", gcnew array<Object^> {"Description", "FPanel Users"});
			newGroup->CommitChanges();
		}
		catch (Exception ^e) {
			logger->debugLog("usermng: " + e->Message, 1802);
		}

		//utils->exec("cmd", "/c cacls " + env->vhosts_dir + " /E /T /C /R \"Users\"");

		file_manager->ListContents(env->panel_dir, "fpusers", AccessControlType::Deny);
		file_manager->ReadAndExecute(env->panel_dir, "fpusers", AccessControlType::Deny);
	}

	void usermng::create()
	{
		if ((user == nullptr) || (passwd == nullptr) || (domain == nullptr) || (prefix == nullptr)) {
			logger->errorLog("Missing arguments", 2601);
			System::Environment::Exit(2601);
		}
		if (validate->checkUser(user) == true) {
			logger->errorLog(user + " already exists", 2602);
			System::Environment::Exit(2602);
		}

		String ^path = env->vhosts_dir + prefix;

		users->createUser(user, passwd, path, domain, nullptr, nullptr);

		logger->infoLog(user + " created for " + domain, 2601);
	}

	void usermng::update()
	{
		try {
			if (user == nullptr) {
				logger->errorLog("Missing arguments", 2601);
				System::Environment::Exit(2601);
			}
			if (validate->checkUser(user) == false) {
				logger->errorLog(user + " not found", 2602);
				System::Environment::Exit(2602);
			}

			if ((new_passwd != nullptr) && !String::IsNullOrEmpty(new_passwd)) {
				users->updateUser(user, new_passwd, nullptr, nullptr, nullptr);
			}

			if ((new_user != nullptr) && !String::IsNullOrEmpty(new_user)) {
				users->updateUser(user, nullptr, new_user, nullptr, nullptr);
			}
			logger->infoLog(user + " updated", 2602);
		}
		catch (Exception ^e) {
			logger->debugLog("usermng: " + e->Message, 1802);
		}
	}

	void usermng::remove()
	{
		try {
			if (user == nullptr) {
				logger->errorLog("Missing arguments", 2601);
				System::Environment::Exit(2601);
			}
			if (validate->checkUser(user) == false) {
				logger->errorLog(user + " not found", 2602);
				System::Environment::Exit(2602);
			}

			users->removeUser(user);
			logger->infoLog(user + " removed", 2603);
		}
		catch (Exception ^e) {
			logger->debugLog("usermng: " + e->Message, 1803);
		}
	}

	void usermng::suspend()
	{
		try {
			if (user == nullptr) {
				logger->errorLog("Missing arguments", 2601);
				System::Environment::Exit(2601);
			}
			if (validate->checkUser(user) == false) {
				logger->errorLog(user + " not found", 2602);
				System::Environment::Exit(2602);
			}
			users->stateUser(user, "suspend");
			logger->infoLog(user + " suspended", 2604);
		}
		catch (Exception ^e) {
			logger->debugLog("usermng: " + e->Message, 1804);
		}
	}

	void usermng::unsuspend()
	{
		try {
			if (user == nullptr) {
				logger->errorLog("Missing arguments", 2601);
				System::Environment::Exit(2601);
			}
			if (validate->checkUser(user) == false) {
				logger->errorLog(user + " not found", 2602);
				System::Environment::Exit(2602);
			}
			users->stateUser(user, "unsuspend");
			logger->infoLog(user + " unsuspended", 2605);
		}
		catch (Exception ^e) {
			logger->debugLog("usermng: " + e->Message, 1805);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	usermng ^mainProcess = gcnew usermng();
	mainProcess->Main(Args);
	return 0;
}