#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/Services.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/ApplicationPool.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	public ref class apppoolmng
	{
	public:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Services ^services = gcnew Services();
		static FPanel::Windows::ApplicationPool ^application_pool = gcnew FPanel::Windows::ApplicationPool();
		static String ^prefix;
		static String ^user;
		static String ^passwd;
		static String ^domain;
		static String ^aspnet_ver;
		static String ^cpu_limit;
		static String ^pool_mode;
		static String ^asp_net_core;

		int main(array<String^> ^Args)
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
			user = main_case["user"];
			passwd = main_case["passwd"];
			domain = main_case["domain"];
			aspnet_ver = main_case["aspnet-ver"];
			cpu_limit = main_case["cpu-limit"];
			pool_mode = main_case["pool-mode"];
			asp_net_core = main_case["asp-net-core"];

			if (process == "create") {
				create();
			}
			else if (process == "update") {
				update();
			}
			else if (process == "remove") {
				remove();
			}
			else if (process == "suspend") {
				suspend();
			}
			else if (process == "unsuspend") {
				unsuspend();
			}
			else {
				System::Environment::Exit(0);
			}
			return 0;
		}

	private:
		void apppoolmng::create()
		{
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(user) || String::IsNullOrEmpty(passwd)) {
				logger->errorLog("Missing arguments", 2801);
				System::Environment::Exit(2801);
			}
			application_pool->createApplicationPool(domain + "(Application Pool)", aspnet_ver, cpu_limit, user, passwd, pool_mode, asp_net_core);
			application_pool->assignToSite(domain, domain + "(Application Pool)");
		}

		void apppoolmng::update()
		{
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(user) || String::IsNullOrEmpty(passwd)) {
				logger->errorLog("Missing arguments", 2801);
				System::Environment::Exit(2801);
			}
			application_pool->updateApplicationPool(domain + "(Application Pool)", pool_mode, aspnet_ver, cpu_limit, user, passwd, asp_net_core);

		}

		void apppoolmng::remove()
		{
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Missing arguments", 2801);
				System::Environment::Exit(2701);
			}
			application_pool->removeApplicationPool(domain + "(Application Pool)");
		}

		void apppoolmng::suspend()
		{

		}

		void apppoolmng::unsuspend()
		{

		}
	};
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	apppoolmng ^admin = gcnew apppoolmng();
	admin->main(Args);
	return 0;
}