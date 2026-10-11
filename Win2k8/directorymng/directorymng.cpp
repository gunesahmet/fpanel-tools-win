#include "directorymng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void directorymng::Main(array<String^> ^Args)
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
		prefix = main_case["prefix"];
		user = main_case["user"];
		new_user = main_case["new-user"];
		force = main_case["force"];
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
		else {
			System::Environment::Exit(0);
		}
	}

	void directorymng::init()
	{

	}

	void directorymng::create()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(user) || String::IsNullOrEmpty(prefix)) {
				logger->errorLog("Domain or username is null", 1500);
				System::Environment::Exit(1500);
			}

			if (validate->checkUser(user) == false) {
				logger->errorLog(user + " user not found", 1502);
				System::Environment::Exit(1502);
			}

			if (validate->checkDirectory(env->vhosts_dir + prefix) == true) {
				logger->errorLog(domain + "'s " + prefix + " directory already exists", 1503);
				System::Environment::Exit(1503);
			}

			String ^path = env->vhosts_dir + prefix;;
			String ^ftpPath = "c:\\inetpub\\ftproot\\localuser\\" + user;
			directories->create(domain, path, user, ftpPath);
		

			FPanel::Windows::FileManager ^fm = gcnew FPanel::Windows::FileManager();

			fm->ReadAndExecute(path + "\\vhost", env->pool_user, AccessControlType::Allow);
			fm->Write(path + "\\vhost", env->pool_user, AccessControlType::Allow);

			Directory::CreateDirectory(env->panel_domain_configs_dir + domain);
			Directory::CreateDirectory(env->panel_domain_configs_dir + domain + "\\php");
			Directory::CreateDirectory(env->panel_domain_configs_dir + domain + "\\ssl");

			// c:\fpanel\core\bin\permissionmng.exe --set-permission --user=domain2 --domain=domain2.com --file=\\vhost --prefix=ft8eisas --permission=F --allow=on --deny=off --cpu=63
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\vhost --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\vhost\\httpdoc --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\vhost\\sessions --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\cgi-bin --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\error-docs --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\stats --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			utils->exec(env->permissionmng, "--set-permission --user=" + user + " --domain=" + domain + " --file=\\subdomains --prefix=" + prefix + " --permission=F --allow=on --deny=off");
			logger->infoLog(domain + " folders created", 1501);
		}
		catch (Exception ^e) {
			logger->debugLog("directorymng:" + e->Message, 1504);
		}

	}

	void directorymng::update()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(new_user) || String::IsNullOrEmpty(user) || String::IsNullOrEmpty(prefix)) {
				logger->errorLog("Domain or username is null", 1500);
				System::Environment::Exit(1500);
			}

			if (validate->checkUser(new_user) == false) {
				logger->errorLog(new_user + " user not found", 1502);
				System::Environment::Exit(1502);
			}

			if (validate->checkDirectory(env->vhosts_dir + prefix) == false) {
				logger->errorLog(domain + " directory not found", 1503);
				System::Environment::Exit(1503);
			}


			String ^path = env->vhosts_dir + prefix;;
			String ^ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + user;
			String ^new_ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + new_user;

			directories->update(domain, path, user, ftp_path, new_user, new_ftp_path);
			logger->infoLog(domain + "'s " + prefix + " folders updated", 1502);
		}
		catch (Exception ^e) {
			logger->debugLog("directorymng:" + e->Message, 1505);
		}

	}

	void directorymng::remove()
	{
		try {
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Domain or username is null", 1500);
				System::Environment::Exit(1500);
			}

			if (validate->checkDirectory(env->vhosts_dir + domain) == false) {
				logger->errorLog(domain + " directory not found", 1503);
				System::Environment::Exit(1503);
			}

			String ^path = env->vhosts_dir + prefix;;
			String ^ftp_path = "c:\\inetpub\\ftproot\\localuser\\" + user;
			directories->remove(path, ftp_path);
			utils->removeFolder(env->panel_domain_configs_dir + domain, true);

			//logger->debugLog("Status: " + status,999);

			logger->infoLog(domain + " folders removed", 1503);
		}
		catch (Exception ^e) {
			logger->debugLog("directorymng:" + e->Message, 1506);
		}

	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	directorymng ^mainProcess = gcnew directorymng();
	mainProcess->Main(Args);
	return 0;
}