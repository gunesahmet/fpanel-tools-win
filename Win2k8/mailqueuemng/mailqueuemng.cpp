#include "mailqueuemng.h"

using namespace System;
using namespace System::IO;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void mailqueuemng::Main(array<String^> ^Args)
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


		detectServer();

		if (mail_enable_status) {
			String ^file = "C:\\fpanel\\mail\\bin\\MailEnable.Administration.dll";
			{
				try {
					//System.Reflection.Assembly.ReflectionOnlyLoadFrom(file);
					System::Reflection::Assembly::LoadFrom(file);
				}
				catch (Exception ^ee) {
					Console::WriteLine("MailEnable not installed on this server.");
				}
				System::Environment::Exit(1);
			}
			mailEnable->setEnv(env);
			mailEnable->setLogger(logger);
		}
		else if (hmail_status) {
			String ^file = "C:\\fpanel\\mail\\Bin\\Interop.hMailServer.dll";
			{
				try {
					//System.Reflection.Assembly.ReflectionOnlyLoadFrom(file);
					System::Reflection::Assembly::LoadFrom(file);
				}
				catch (Exception ^ee) {
					Console::WriteLine("hMail Server not installed on this server.");
				}
				System::Environment::Exit(1);
			}
			hMail->setEnv(env);
			hMail->setLogger(logger);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}


		String ^process = Args[0]->Replace("--", "");

		try {
			start = Convert::ToInt32(main_case["start"]);
			end = Convert::ToInt32(main_case["end"]);
		}
		catch (...) {
			start = 0;
			end = 0;
		}

		file = main_case["file"];

		if (process == "queue") {
			queueLists();
		}
		else if (process == "clear") {
			clearQueue();
		}
		else if (process == "remove") {
			removeMessage();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void mailqueuemng::detectServer()
	{
		mail_enable_status = validate->fileIsContains(env->sys_packages, "mail_enable");
		hmail_status = validate->fileIsContains(env->sys_packages, "hmail");
	}


	void mailqueuemng::queueLists()
	{
		if (mail_enable_status) {
			mailEnable->queue(start, end);
		}
		else if (hmail_status) {
			hMail->queue(start, end);
		}
	}

	void mailqueuemng::clearQueue()
	{
		if (mail_enable_status) {
			mailEnable->clearQueue();
		}
		else if (hmail_status) {
			hMail->clearQueue();
		}
	}

	void mailqueuemng::removeMessage()
	{
		if (mail_enable_status) {
			mailEnable->removeMessage(file);
		}
		else if (hmail_status) {
			hMail->removeMessage(file);
		}
	}

}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	mailqueuemng ^mainProcess = gcnew mailqueuemng();
	mainProcess->Main(Args);
	return 0;
}