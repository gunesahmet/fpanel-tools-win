#include "statisticsmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void statisticsmng::Main(array<String^> ^Args)
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
		domainUser = main_case["domain-user"];
		lastInit = main_case["last-init"];
		serverAddress = main_case["server-addr"];
		statisticsRandom = main_case["statistics-random"];
		statisticsTime = main_case["statistics-time"];
		updateRandom = main_case["update-random"];
		updateTime = main_case["update-time"];
		updateAuto = main_case["update-auto"];
		
		if (process == "init") {
			CalculateSystemCheckFlag();
			CalculateSystem();
		}
		else if (process == "init-domain") {
			CalculateDomainCheckFlag();
			CalculateDomain();
		}
		else if (process == "set-config") {
			SetConfigCheckFlag();
			SetConfig();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void statisticsmng::CalculateSystemCheckFlag()
	{

		logger->infoLog("System statistics calculated", 2702);
	}
	void statisticsmng::CalculateSystem()
	{
		logger->infoLog(domain + " updated", 2702);
	}

	void statisticsmng::CalculateDomainCheckFlag()
	{
		logger->infoLog(domain + " updated", 2702);
	}
	void statisticsmng::CalculateDomain()
	{
		logger->infoLog(domain + " updated", 2702);
	}

	void statisticsmng::SetConfigCheckFlag()
	{
		logger->infoLog(domain + " updated", 2702);
	}
	void statisticsmng::SetConfig()
	{
		logger->infoLog(domain + " updated", 2702);
	}

}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	statisticsmng ^mainProcess = gcnew statisticsmng();
	mainProcess->Main(Args);
	return 0;
}