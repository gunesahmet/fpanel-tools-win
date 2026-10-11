#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/Vhost.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class subvhostmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static FPanel::Windows::Vhost ^vhost = gcnew FPanel::Windows::Vhost();

		static String ^user;
		static String ^domain;
		static String ^prefix;

		static String ^subdomain;
		static String ^new_user;
		static String ^ipaddr;
		static String ^old_ipaddr;
		static String ^ssl;
		static String ^php;
		static String ^perl;
		static String ^python;
		static String ^cgi;
		static String ^asp;
		static String ^aspnet;
		static String ^asp_net_core;
		static String ^aspnet_ver;
		static String ^php_ver;
		static String ^passwd;

		static void create();

		static void update();

		static void remove();

		static void suspend();

		static void unsuspend();
	};
}
