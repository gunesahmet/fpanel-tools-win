#include "mailmng.h"

namespace Tools
{
	void mailmng::Main(array<String^> ^Args)
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
		domain = main_case["domain"];
		mail_addr = main_case["mail-addr"];
		passwd = main_case["passwd"];
		quota = main_case["quota"];
		options = main_case["options"];
		forward = main_case["forward"];
		filename = main_case["file"];
		drop = main_case["drop"];
		new_passwd = main_case["new-passwd"];
		drop = main_case["drop"];
		mail_alias = main_case["mail-alias"];
		mail_forward = main_case["mail-forward"];
		mail_status = main_case["status"];

		if (process == "create") {
			createDomain();
		}
		else if (process == "update") {
			updateDomain();
		}
		else if (process == "remove") {
			removeDomain();
		}
		else if (process == "create-mail") {
			createMailBox();
		}
		else if (process == "update-mail") {
			updateMailBox();
		}
		else if (process == "remove-mail") {
			removeMailBox();
		}
		else if (process == "create-mail-alias") {
			createMailAlias();
		}
		else if (process == "remove-mail-alias") {
			removeMailAlias();
		}
		else if (process == "suspend") {
			suspendDomain();
		}
		else if (process == "unsuspend") {
			unsuspendDomain();
		}
		else if (process == "suspend-mail") {
			suspendMail();
		}
		else if (process == "unsuspend-mail") {
			unsuspendMail();
		}
		else if (process == "create-mail-forward") {
			createMailForward();
		}
		else if (process == "remove-mail-forward") {
			removeMailForward();
		}
		else if (process == "domain-settings") {
			domainSettings();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void mailmng::detectServer()
	{
		mail_enable_status = validate->fileIsContains(env->sys_packages, "mail_enable");
		hmail_status = validate->fileIsContains(env->sys_packages, "hmail");
	}

	void mailmng::createDomain()
	{
		if (mail_enable_status) {
			mailEnable->createDomain(domain, quota);
		}
		else if (hmail_status) {
			hMail->createDomain(domain, quota);
		}
	}

	void mailmng::updateDomain()
	{
		if (mail_enable_status) {
			mailEnable->updateDomain(domain, quota);
		}
		else if (hmail_status) {
			hMail->updateDomain(domain, quota);
		}
	}

	void mailmng::removeDomain()
	{
		if (mail_enable_status) {
			mailEnable->removeDomain(domain);
		}
		else if (hmail_status) {
			hMail->removeDomain(domain);
		}
	}

	void mailmng::createMailBox()
	{
		if (mail_enable_status) {
			mailEnable->createMailBox(mail_addr, passwd, quota);
		}
		else if (hmail_status) {
			hMail->createMailBox(mail_addr, passwd, quota);
		}
	}

	void mailmng::updateMailBox()
	{
		if (mail_enable_status) {
			mailEnable->updateMailBox(mail_addr, quota, new_passwd);
		}
		else if (hmail_status) {
			hMail->updateMailBox(mail_addr, quota, new_passwd);
		}
	}

	void mailmng::removeMailBox()
	{
		if (mail_enable_status) {
			mailEnable->removeMailBox(mail_addr);
		}
		else if (hmail_status) {
			hMail->removeMailBox(mail_addr);
		}
	}

	void mailmng::createMailAlias()
	{
		if (mail_enable_status) {
			mailEnable->createMailAlias(mail_addr, mail_alias);
		}
		else if (hmail_status) {
			hMail->createMailAlias(mail_addr, mail_alias);
		}
	}

	void mailmng::removeMailAlias()
	{
		if (mail_enable_status) {
			mailEnable->removeMailAlias(mail_addr, mail_alias);
		}
		else if (hmail_status) {
			hMail->removeMailAlias(mail_addr, mail_alias);
		}
	}

	void mailmng::suspendDomain()
	{
		if (mail_enable_status) {
			mailEnable->suspendDomain(domain);
		}
		else if (hmail_status) {
			hMail->suspendDomain(domain);
		}
	}

	void mailmng::unsuspendDomain()
	{
		if (mail_enable_status) {
			mailEnable->unsuspendDomain(domain);
		}
		else if (hmail_status) {
			hMail->unsuspendDomain(domain);
		}
	}

	void mailmng::suspendMail()
	{
		if (mail_enable_status) {
			mailEnable->suspendMail(mail_addr);
		}
		else if (hmail_status) {
			hMail->suspendMail(mail_addr);
		}
	}

	void mailmng::unsuspendMail()
	{
		if (mail_enable_status) {
			mailEnable->unsuspendMail(mail_addr);
		}
		else if (hmail_status) {
			hMail->unsuspendMail(mail_addr);
		}
	}

	void mailmng::createMailForward()
	{
		if (mail_enable_status) {
			mailEnable->createMailForward(mail_addr, mail_forward);
		}
		else if (hmail_status) {
			hMail->createMailForward(mail_addr, mail_forward);
		}
	}

	void mailmng::removeMailForward()
	{
		if (mail_enable_status) {
			mailEnable->removeMailForward(mail_addr);
		}
		else if (hmail_status) {
			hMail->removeMailForward(mail_addr);
		}
	}

	void mailmng::domainSettings()
	{
		if (mail_enable_status) {
			mailEnable->domainSettings(domain, options, mail_addr);
		}
		else if (hmail_status) {
			hMail->domainSettings(domain, options, mail_addr);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	mailmng ^mainProcess = gcnew mailmng();
	mainProcess->Main(Args);
	return 0;
}