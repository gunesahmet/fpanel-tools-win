#include "packagemng.h"
#include <vector>

using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Management;
using namespace System::Collections::Generic;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void packagemng::Main(array<String^> ^Args)
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
			String ^process = Args[0]->Replace("--", "");
			domain = main_case["domain"];

			if (process == "check") {
				checkSystem();
			}
			else if (process == "create") {
			}
			else if (process == "update") {
			}
			else if (process == "remove") {
			}
			else if (process == "list") {
				listPackage();
			}
			else {
				System::Environment::Exit(0);
			}
		}
	}

	void packagemng::checkSystem()
	{
		try {
			ManagementObjectSearcher ^mos = gcnew ManagementObjectSearcher("SELECT * FROM Win32_Product");
			for each (ManagementObject ^mo in mos->Get())
			{
				try {
					system_packages[mo["Name"]->ToString()] = mo["Version"]->ToString();
				}
				catch (Exception ^e) {
					logger->debugLog("packagemng: " + e->Message, 1801);
				}
			}

			for each (KeyValuePair<String^, String^> pkg in system_packages)
			{
				Console::WriteLine(pkg.Key);
				Console::WriteLine(pkg.Value);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("packagemng: " + e->Message, 1802);
		}

	}

	void packagemng::listPackage()
	{
		/*try
		{*/
		//Dictionary<String^, String^> ^installeds = gcnew Dictionary<String^, String^>();
		List<String^> ^keys = gcnew List<String^>(gcnew array<String^> {"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"});
		Dictionary<String^, String^> ^system_package_x86 = gcnew Dictionary<String^, String^>();
		Dictionary<String^, String^> ^system_packages_x64 = gcnew Dictionary<String^, String^>();

		utils->FindInstalls(RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64), keys, system_packages_x64);
		utils->FindInstalls(RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry32), keys, system_package_x86);

	/*	for each (KeyValuePair<String^, String^> pkg in system_packages_x64) {
			Console::WriteLine(pkg.Key);
		}*/

		String ^url = env->repo_server + "/windows/package.list";

		String ^packages = (gcnew WebClient())->DownloadString(url);

		String ^json = "[";
		StringReader ^reader = gcnew StringReader(packages);
		//try
		//{
		String ^line;
		//while ((line = reader->ReadLine()) != nullptr) {

		//	if (String::IsNullOrEmpty(line->Trim()))
		//		continue;

		//	array<String^> ^strs = line->Split(',');

		//	String ^package = strs[0]->Trim()->Replace("\\+", ""); // Console::WriteLine(package);

		//	String ^version = strs[1]->Trim()->Replace("\\+", ""); //Console::WriteLine(version);

		//	String ^arch = strs[2]->Trim()->Replace("\\+", ""); //Console::WriteLine(arch);

		//	String ^package_name = strs[3]->Trim()->Replace("\\+", ""); // Console::WriteLine(package_name);

		//	String ^description = strs[4]->Trim()->Replace("\\+", ""); //Console::WriteLine(description);

		//	String ^status = "not_installed"; // installed,not-installed

		//	if ((package == "dotnet40.exe") || (package == "dotnet40_core.exe")) {
		//		if (validate->checkDirectoryStartWith(env->system_path + "Windows\\Microsoft.NET\\Framework64", "v4.0*") == true) {
		//			status = "installed";
		//		}
		//	}
		//	else if (package == "dotnet45.exe") {
		//		if (validate->checkDirectoryStartWith(env->system_path + "Windows\\Microsoft.NET\\Framework64", "v4.5*") == true) {
		//			status = "installed";
		//		}
		//	}
		//	else if (package == "dotnet48.exe") {
		//		if (validate->checkDirectoryStartWith(env->system_path + "Windows\\Microsoft.NET\\Framework64", "v4.8*") == true) {
		//			status = "installed";
		//		}
		//	}
		//	else {
		//		try {
		//			package_name = package_name->Replace("+", "\\\+");
		//			Regex ^searchRegex = gcnew Regex("^" + package_name + ".*");
		//			for each (KeyValuePair<String^, String^> pkg in system_packages_x64) {
		//				if (searchRegex->IsMatch(pkg.Key)) {
		//					package_name = pkg.Key;
		//					status = "installed";
		//					system_packages->Remove(pkg.Key);
		//					break;
		//				}
		//			}

		//			for each (KeyValuePair<String^, String^> pkg in system_package_x86) {
		//				if (searchRegex->IsMatch(pkg.Key)) {
		//					package_name = pkg.Key;
		//					status = "installed";
		//					system_packages->Remove(pkg.Key);
		//					break;
		//				}
		//			}
		//		}
		//		catch (...) {
		//			//Console::WriteLine(package_name);
		//			continue;
		//		}

		//	}
		//	//Console::WriteLine(package_name);
		//	package_name = package_name->Replace("\\\+", "\+");

		//	json += "{";
		//	json += "\"package_name\":\"" + package_name + "\",";
		//	json += "\"version\":\"" + version + "\",";
		//	json += "\"description\":\"" + description + "\",";
		//	json += "\"status\":\"" + status + "\",";
		//	json += "\"arch\":\"" + arch + "\"";
		//	json += "},";
		//}

		for each (KeyValuePair<String^, String^> pkg_left in system_packages_x64)
		{
			//Console::WriteLine(pkg_left.Key);
			json += "{";
			json += "\"package_name\":\"" + pkg_left.Key->ToString() + "\",";
			json += "\"version\":\"" + pkg_left.Value->ToString() + "\",";
			json += "\"description\":\"" + pkg_left.Key->ToString() + "\",";
			json += "\"status\":\"installed\",";
			json += "\"arch\":\"x64\"";
			json += "},";
		}

		for each (KeyValuePair<String^, String^> pkg_left in system_package_x86)
		{
			//Console::WriteLine(pkg_left.Key);
			json += "{";
			json += "\"package_name\":\"" + pkg_left.Key->ToString() + "\",";
			json += "\"version\":\"" + pkg_left.Value->ToString() + "\",";
			json += "\"description\":\"" + pkg_left.Key->ToString() + "\",";
			json += "\"status\":\"installed\",";
			json += "\"arch\":\"x64\"";
			json += "},";
		}
		/*}
	catch (...) {
		logger->errorLog("Failed package list download", 1801);
		Console::WriteLine("Failed download");
	}
	finally
	{
		delete reader;
	}*/
		json = json->TrimEnd(',');
		json += "]";
		Console::WriteLine(json);
		/*	}
			catch (Exception ^e)
			{
				logger->debugLog("packagemng: " + e->Message, 1803);
			}*/

	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	packagemng ^mainProcess = gcnew packagemng();
	mainProcess->Main(Args);
	return 0;
}