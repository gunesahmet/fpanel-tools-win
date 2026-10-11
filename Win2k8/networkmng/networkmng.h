#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/Services.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace System::Net::NetworkInformation;
using namespace System::Text::RegularExpressions;
using namespace Environments;

namespace Tools
{
	private ref class networkmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Services ^services = gcnew Services();

		static String ^eth;
		static String ^netmask;
		static String ^ipaddr;
		static String ^host;
		static String ^domain;
		static String ^index;
		static String ^default_gateway;
		static String ^config;


		static void init();

		static void create(String ^index, String ^ipaddr, String ^netmask, String ^default_gateway);

		static void remove(String ^index, String ^ipaddr);

		static void show();

		static void showDefaultIp();

		static void showGw();

		static void hostname(String^ host, String ^domain);

		static String ^getGateway(String ^ipaddress);

		static bool checkIpAddressOnSystem(array<String^> ^allips, String ^ipaddress);

		static void setConfig(String ^config);
	};
}
