#include "protectmng.h"

using namespace System;

namespace Tools
{
	void protectmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str,999);
		if (utils->isAdmin() == false)
		{
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0)
		{
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");
		domain = main_case["domain"];
		prefix = main_case["prefix"];
		if (process == "init")
		{
		}
		else if (process == "create")
		{
		}
		else if (process == "update")
		{
		}
		else if (process == "remove")
		{
		}
		else
		{
				System::Environment::Exit(0);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	protectmng ^mainProcess = gcnew protectmng();
	mainProcess->Main(Args);
	return 0;
}