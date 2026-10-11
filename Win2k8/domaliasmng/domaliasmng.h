//.h file code:

#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace Environments;
using namespace Microsoft::Web::Administration;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	private ref class domaliasmng
	{
	public:
		static void Main(array<String^> ^Args);
	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static String ^domain;
		static String ^prefix;
		static String ^alias;
		static String ^mail_serv;
		static String ^web_serv;
		static String ^sync;
		static String ^new_alias;
		static String ^ipaddr;


		static void create();

		static void update();

		static void remove();

		static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues);

		static void addBinding(String ^domainname, String ^aliasname, String ^ipaddr);

		static String ^getDomainIp(String ^domainname);

		static void removeBinding(String ^domainname, String ^aliasname, String ^ipaddr);

		static void bindSync(String ^domainname, String ^aliasname);

		static void removeFromDNS(String ^aliasname);
	};
}