#include "adminmng.h"

using namespace Microsoft::VisualBasic::Devices;
using namespace Microsoft::Win32;
using namespace System;
using namespace System::IO;
using namespace LSA;

namespace Tools
{
	void adminmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;

		logger->debugLog(str, 999);

		if (utils->isAdmin() == false) {
			Console::WriteLine("You have not right to access.");
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		if (Args->Length > 0) {
			Arguments ^main_case = gcnew Arguments(Args);
			String ^username = main_case["user"];
			String ^password = main_case["passwd"];
			String ^cpu_usage = main_case["cpu"];
			String ^process = Args[0]->Replace("--", "");
			if (process == "conf") {
				createSysConf();
			}
			else if (process == "init") {
				init(false);
			}
			else if (process == "reset-factory") {
				init(true);
			}
			else if (process == "sys-relax") {
			}
			else if (process == "reboot") {
				reboot();
			}
			else if (process == "shutdown") {
				shutdown();
			}
			else if (process == "sysinfo") {
				sysInfo(cpu_usage);
			}
			else if (process == "create-support") {
				createSupportUser(username, password);
			}
			else if (process == "remove-support") {
				removeSupportUser(username);
			}
			else if (process == "cli") {
				//TODO: Yazýlacak gereklimi kontrol edilecek
			}
			else {
				System::Environment::Exit(0);
			}
		}
		System::Environment::Exit(0);
	}

	String ^adminmng::systemInfoNetwork()
	{
		Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
		RegistryKey ^dKey = localMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters", true);
		String ^fqdn_domain;
		if (dKey != nullptr) {
			fqdn_domain = dKey->GetValue("Domain")->ToString();
			dKey->Close();
		}
		return System::Net::Dns::GetHostName() + "." + fqdn_domain;
	}

	void adminmng::createSysConf()
	{
		try {
			String ^default_ip_address = network->defaultIpAddress();
			String ^cpu_model = cpuModel();
			String ^full_version = (gcnew ComputerInfo())->OSFullName;

			String ^available_2008 = "Windows Server 2008";
			String ^available_2012 = "Windows Server 2012";
			String ^available_2008_r2 = "Windows Server 2008 R2";
			String ^available_2012_r2 = "Windows Server 2012 R2";

			String ^server_version_core = "";
			String ^server_version_r = "";
			String ^server_version = "";

			if (full_version->Contains(available_2012)) {
				server_version = "2012";
			}
			else if (full_version->Contains(available_2008)) {
				server_version = "2008";
			}

			String ^sys_conf_content = "IPADDR=" + default_ip_address + "\n" + "OS_TYPE=" + full_version + "\n" + "OS_VERSION=" + server_version + "\n" + "OS_ARCH=x64\n" + "CPU_MODEL=" + cpu_model;

			System::IO::StreamWriter ^sys_conf = gcnew System::IO::StreamWriter(env->sysconf);
			sys_conf->WriteLine(sys_conf_content);
			sys_conf->Close();
		}
		catch (Exception ^e) {
			logger->debugLog("adminmng:" + e->Message, 3507);
		}

	}

	String ^adminmng::cpuModel()
	{
		auto localMachine = Registry::LocalMachine;
		RegistryKey ^processor_name = localMachine->OpenSubKey("Hardware\\Description\\System\\CentralProcessor\\0", RegistryKeyPermissionCheck::ReadSubTree); //This registry entry contains entry for processor info.
		if (processor_name != nullptr) {
			if (processor_name->GetValue("ProcessorNameString") != nullptr) {
				return processor_name->GetValue("ProcessorNameString")->ToString();
			}
		}
		return "Undefined";
	}

	void adminmng::init(bool reset)
	{
		try {
			// DNS default dosyalarý yerlerine yerleþtirilecek
			utils->exec(env->networkmng, "--init");
			utils->exec(env->logmng, "--init --comp=on --clean-err=off --day=30");
			utils->exec(env->websrvmng, "--init");
			utils->exec(env->usermng, "--init");
			utils->exec(env->pagemng, "--init --ipaddr=" + network->defaultIpAddress());
			utils->exec(env->servicesmng, "--dns --proc=reload");
			FPanel::Windows::SecEdit ^sce = gcnew FPanel::Windows::SecEdit();
			sce->init();
		}
		catch (Exception ^e) {
			logger->errorLog("adminmng:" + e->Message, 3508);
		}
	}

	void adminmng::reboot()
	{
		utils->exec("shutdown", "-t 0 -r -f");
	}

	void adminmng::shutdown()
	{
		utils->exec("shutdown", "-t 0 -s -f");
	}

	void adminmng::sysInfo(String ^cpu_usage)
	{
		try {
			String ^system_informations = utils->exec(env->systeminfo, nullptr);
			system_informations = system_informations->Replace(" ", "#");
			String ^full_version = (gcnew ComputerInfo())->OSFullName;

			array<String^> ^system_informations_arr = system_informations->Split(',');
			String ^os_type = full_version;
			String ^os_arch = "x64";
			int uptime = Int32::Parse(system_informations_arr[5]->Replace("#", ""));

			String ^cpu_model = system_informations_arr[9]->Replace("#", " ")->Trim();
			int cpu_used = Int16::Parse(system_informations_arr[0]->Replace("#", ""));
			int cpu_free = 100 - cpu_used;

			Int64 mem_total = Convert::ToInt64(system_informations_arr[2]);
			Int64 mem_used = Convert::ToInt64(system_informations_arr[3]->Replace("#", ""));
			Int64 mem_free = Convert::ToInt64(system_informations_arr[4]->Replace("#", ""));

			Int64 disk_total = Convert::ToInt64(system_informations_arr[6]->Replace("#", ""));
			Int64 disk_used = Convert::ToInt64(system_informations_arr[7]->Replace("#", ""));
			Int64 disk_free = Convert::ToInt64(system_informations_arr[8]->Replace("#", ""));

			String ^hostname = systemInfoNetwork();
			String ^ipaddr = network->defaultIpAddress();
			String ^total_traffic = network->totalTraffic();

			String ^json = "{\"system\":{\"os\":\"" + os_type + "\",\"kernel\":\"" + os_arch + "\"},\"uptime\":\"" + uptime + "\",\"cpu_info\":\"" + cpu_model + "\",\"cpu\":{\"total\":\"100\",\"used\":\"" + cpu_usage + "\",\"free\":\"" + cpu_free + "\"},\"loadavg\":{\"min1\":\"0.00\",\"min5\":\"0.00\",\"min15\":\"0.00\"},\"memory\":{\"total\":\"" + mem_total + "\",\"used\":\"" + mem_used + "\",\"free\":\"" + mem_free + "\"},\"disk\":{\"total\":\"" + disk_total + "\",\"used\":\"" + disk_used + "\",\"free\":\"" + disk_free + "\"},\"network\":{\"hostname\":\"" + hostname + "\",\"ipaddr\":\"" + ipaddr + "\",\"traffic\":\"" + total_traffic + "\"}}";
			//string json = "";
			Console::WriteLine(json);
		}
		catch (Exception ^e) {
			logger->debugLog("adminmng:" + e->Message, 3508);
		}


	}

	void adminmng::createSupportUser(String ^username, String ^password)
	{
		try {
			if ((username == nullptr) || (password == nullptr)) {
				logger->errorLog("Username or Password is null", 3500);
				System::Environment::Exit(3500);
			}

			if (validate->checkUser(username) == true) {
				logger->errorLog(username + " already exists", 3501);
				System::Environment::Exit(3501);
			}
			utils->exec("net", "user " + username + " " + password + " /add /comment:\"FPanel Suport User\"");
			utils->exec("wmic", "useraccount where \"name='" + username + "'\" set PasswordExpires=False");
			utils->exec("net", "group \"Administrators\" " + username + " /add");
			utils->exec("net", "group \"Users\" " + username + " /add");
			logger->infoLog(username + " created for support", 3505);
		}
		catch (Exception ^e) {
			logger->debugLog("adminmng:" + e->Message, 3509);
		}


	}

	void adminmng::removeSupportUser(String ^username)
	{
		try {
			if (username == nullptr) {
				logger->errorLog("Username oris null", 3500);
				System::Environment::Exit(3500);
			}
			if (validate->checkUser(username) == false) {
				logger->errorLog(username + " not found", 3502);
				System::Environment::Exit(3502);
			}
			utils->exec("net", "user " + username + " /delete");
			logger->infoLog(username + " removed", 3506);
		}
		catch (Exception ^e) {
			logger->debugLog("adminmng:" + e->Message, 3510);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	adminmng ^mainProcess = gcnew adminmng();
	mainProcess->Main(Args);
	return 0;
}