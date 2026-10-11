#include "datemng.h"

using namespace Microsoft::Win32;
using namespace System;
using namespace System::Globalization;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;


namespace Tools
{
	String^ datemng::getCurrentRegion()
	{
		try {
			Microsoft::Win32::RegistryKey ^localMachine = Registry::CurrentUser;
			RegistryKey ^myKeyRbl = localMachine->OpenSubKey("SOFTWARE\\Control Panel\\International", true);
			String ^bl = myKeyRbl->GetValue("LocaleName")->ToString();
			myKeyRbl->Close();
			return bl;
		}
		catch (...) {
			return "en-US";
		}
	}

	void datemng::Main(array<String^> ^Args)
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
		String ^time = main_case["time"];
		String ^date = main_case["date"];
		String ^timezone = main_case["zone"];
		String ^ntp_server = main_case["ntp-server"];

		// Format sunucu diline ve bölgesine göre deðiþiyor. Öðrenmenin bir yolunu bulmalýsýn
		// Format TR için dd.mm.yyyy

		if (process == "set-time") {
			setTime(time);
		}
		else if (process == "set-date") {
			setDate(date);
		}
		else if (process == "set-timezone") {
			setTimeZone(timezone, ntp_server);
		}
		else if (process == "show-date") {
			showDate();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void datemng::setTime(String ^time)
	{
		try {
			if (String::IsNullOrEmpty(time)) {
				logger->errorLog("Time format is null", 7001);
				System::Environment::Exit(7001);
			}
			System::Diagnostics::ProcessStartInfo ^proc = gcnew System::Diagnostics::ProcessStartInfo();
			proc->UseShellExecute = true;
			proc->WorkingDirectory = "C:\\Windows\\System32";
			proc->CreateNoWindow = true;
			proc->FileName = "C:\\Windows\\System32\\cmd.exe";
			proc->Verb = "runas";
			proc->Arguments = "/C time " + time;
			try {
				System::Diagnostics::Process::Start(proc);
			}
			catch (...) {
			}

			logger->infoLog("Server time changed", 7001);
		}
		catch (Exception ^e) {
			logger->debugLog("datemng:" + e->Message, 7004);
		}
	}

	void datemng::setDate(String ^date)
	{
		try {
			if (date == nullptr) {
				logger->errorLog("Date format is null", 7002);
				System::Environment::Exit(7002);
			}
			DateTime dtn = DateTime::ParseExact(date->ToString(), "yyyyMMdd", CultureInfo::InvariantCulture);

			String ^s = dtn.ToString("dd/MM/yyyy", CultureInfo::InvariantCulture);


			array<String^> ^date_split = s->Split('/');

			short day = Convert::ToInt16(date_split[0]);
			short month = Convert::ToInt16(date_split[1]);
			short year = Convert::ToInt16(date_split[2]);

			DateTime dt = DateTime::Now;
			DateTime utcdt = dt.ToUniversalTime();
			String ^format = "HH:mm:ss";
			String ^utcstr = utcdt.ToString(format);


			array<String^> ^current_time_splt = utcstr->Split(':');
			short current_hour = Convert::ToInt16(current_time_splt[0]);
			short current_min = Convert::ToInt16(current_time_splt[1]);
			short current_sec = Convert::ToInt16(current_time_splt[2]);

			SYSTEMTIME st = SYSTEMTIME();
			st.wYear = year;
			st.wMonth = month;
			st.wDay = day;

			SetSystemTime(st);

			logger->infoLog("Server date changed", 7002);
		}
		catch (Exception ^e) {
			logger->debugLog("datemng:" + e->Message, 7005);
		}
	}

	void datemng::setTimeZone(String ^timezone, String ^ntp_server)
	{
		try {
			if (String::IsNullOrEmpty(timezone)) {
				logger->errorLog("Timezone format is null", 7003);
				System::Environment::Exit(7003);
			}

			utils->exec("tzutil", "/s \"" + timezone + "\"");

			if (!String::IsNullOrEmpty(ntp_server)) {
				Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
				RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DateTime\\Servers", true);
				
				if (myKey != nullptr) {

					String ^selected_ntp;
					for each(String ^key in myKey->GetValueNames()) {
						if (String::IsNullOrEmpty(key)) {// This is default
							continue;
						}
						if (myKey->GetValue(key)->ToString()->Equals(ntp_server->Trim())) {
							selected_ntp = key;
							break;
						}
					}
					if (String::IsNullOrEmpty(selected_ntp)) {
						myKey->SetValue("", selected_ntp, RegistryValueKind::String);
					}
					myKey->Close();
				}
			}
			logger->infoLog("Server timezone changed", 7003);
		}
		catch (Exception ^e) {
			logger->debugLog("datemng:" + e->Message, 7006);
		}
	}

	void datemng::showDate()
	{
		String ^selected_server_name;
		try {
			Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
			RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DateTime\\Servers", true);
			if (myKey != nullptr) {
				String ^default_server_id = myKey->GetValue("")->ToString();
				selected_server_name = myKey->GetValue(default_server_id)->ToString();
				myKey->Close();
			}
		}
		catch (Exception ^e) {
			logger->debugLog("datemng:" + e->Message, 7008);
		}

		try {
			CultureInfo ^en = gcnew CultureInfo("en-US");
			Thread::CurrentThread->CurrentCulture = en;

			auto LocalMachine = Registry::LocalMachine;
			RegistryKey ^key = LocalMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation");
			String ^localZone = safe_cast<String^>(key->GetValue("TimeZoneKeyName"));
			if (String::IsNullOrEmpty(localZone))
				localZone = safe_cast<String^>(key->GetValue("StandardName"));
			key->Close();

			DateTime dt = DateTime::Now;
			String ^json;
			DateTime utcdt = dt.ToLocalTime();
			String ^format = "MM/dd/yyyy HH:mm:ss";
			String ^utcstr = utcdt.ToString(format);
			array<String^> ^splitted = utcstr->Split(' ');
			json = "{\"date\":\"" + splitted[0] + "\",\"time\":\"" + splitted[1] + "\",\"timezone\":\"" + localZone + "\",\"ntp_address\":\"" + selected_server_name + "\"}";
			Console::WriteLine(json);
		}
		catch (Exception ^e) {
			logger->debugLog("datemng:" + e->Message, 7007);
		}
	}

}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	datemng ^bckup = gcnew datemng();
	bckup->Main(Args);
	return 0;
}