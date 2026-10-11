#include "ftpmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{

	void ftpmng::Main(array<String^> ^Args)
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
		sess_id = main_case["pid"];
		timeout_idle = main_case["timeout-idle"];
		timeout_transfer = main_case["timeout-transfer"];
		max_instance = main_case["max-instances"];
		max_clients = main_case["max-clients"];
		message = main_case["message"];

		if (process == "sessions") {
			activeSessions();
		}
		else if (process == "kill") {
			killSession();
		}
		else if (process == "settings") {
			ftpSettings();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void ftpmng::activeSessions()
	{
		ftp_settings->Sessions();
	}

	void ftpmng::killSession()
	{
		ftp_settings->kill(sess_id);
	}

	void ftpmng::ftpSettings()
	{
		ftp_settings->Set(timeout_idle, timeout_transfer, max_instance, max_clients, message);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	ftpmng ^mainProcess = gcnew ftpmng();
	mainProcess->Main(Args);
	return 0;
}