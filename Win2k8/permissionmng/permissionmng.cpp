#include "permissionmng.h"

using namespace Microsoft::Win32;
using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;

namespace Tools
{

	void permissionmng::Main(array<String^> ^Args)
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


		if (Args->Length > 0) {

			Arguments ^main_case = gcnew Arguments(Args);

			domain = main_case["domain"];
			prefix = main_case["prefix"];
			file = env->vhosts_dir + prefix + main_case["file"];
			allow = main_case["allow"];
			deny = main_case["deny"];
			permission = main_case["permission"];
			user = main_case["user"];

			if (String::IsNullOrEmpty(file)) {
				logger->errorLog("Missing arguments.", 1601);
				System::Environment::Exit(1601);
			}
			bool file_exists = false;

			if (Directory::Exists(file) == true) {
				file_exists = true;
			}
			if (File::Exists(file) == true) {
				file_exists = true;
			}

			if (file_exists == false) {
				logger->errorLog("File doesnt exists.", 1602);
				System::Environment::Exit(1602);
			}

			String ^process = Args[0]->Replace("--", "");
			if (process == "get-permission") {
				getPermission();
			}
			else if (process == "set-permission") {
				setPermission();
			}
			else {
				System::Environment::Exit(0);
			}
		}
		System::Environment::Exit(0);
	}

	void permissionmng::getPermission()
	{
		if ((domain == nullptr) || (prefix == nullptr)) {
			logger->errorLog("Missing arguments", 2401);
			System::Environment::Exit(2401);
		}
		try {
			file_manager->getPermissions(file);
		}
		catch (Exception ^e) {
			logger->debugLog("permissionmng: " + e->Message, 1801);
		}
	}

	void permissionmng::setPermission()
	{
		if ((domain == nullptr) || (prefix == nullptr)) {
			logger->errorLog("Missing arguments", 2401);
			System::Environment::Exit(2401);
		}
		file = file->Replace("/\\", "/");
		if ((file == "/") || String::IsNullOrEmpty(file)) {
			logger->errorLog("Invalid file", 1601);
			return;
		}
		file = file->Replace("\\", "/");
		file = file->Replace("//", "/");
		file = file->Replace("//", "/");
		file = file->Replace("/", "\\");
		//String ^parameters = "";
		if (String::IsNullOrEmpty(permission)) {
			logger->errorLog("Missing arguments.", 1601);
		}
		//try {
			String ^perm_type = "allow";
			if (deny == "on")
				perm_type = "deny";

			Dictionary<String^, String^> ^permissions = gcnew Dictionary<String^, String^>();

			if (permission == "F") {
				permissions["FullControl"] = perm_type;
				//parameters = parameters + " --FullControl=" + perm_type;
			}
			else if (permission == "M") {
				permissions["Modify"] = perm_type;
				//parameters = parameters + "--Modify=" + perm_type;
			}
			else if (permission == "RXW") {
				permissions["ReadAndExecute"] = perm_type;
				permissions["Write"] = perm_type;
				//		parameters = parameters + "--ReadAndExecute=" + perm_type + " --Write=" + perm_type;
			}
			else if (permission == "RX") {
				permissions["ReadAndExecute"] = perm_type;
				//		parameters = parameters + "--ReadAndExecute=" + perm_type;
			}
			else if (permission == "W") {
				permissions["Write"] = perm_type;
				//		parameters = parameters + "--Write=" + perm_type;
			}
			else if (permission == "RW") {
				permissions["Write"] = perm_type;
				permissions["Read"] = perm_type;
				//		parameters = parameters + "--Write=" + perm_type + " --Read=" + perm_type;
			}
			else if (permission == "R") {
				permissions["Read"] = perm_type;
				//		parameters = parameters + "--Read=" + perm_type;
			}
			file_manager->ClearDirectorySecurity(file, user);
			file_manager->setPermissions(file, user, permissions);


	/*	}
		catch (Exception ^e) {
			logger->debugLog("permissionmng: " + e->Message, 1802);
		}*/
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	permissionmng ^mainProcess = gcnew permissionmng();
	mainProcess->Main(Args);
	return 0;
}