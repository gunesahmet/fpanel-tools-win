//.h file code:

#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"


using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Text::RegularExpressions;
using namespace Environments;

namespace Tools
{
	private ref class dnsmng
	{
		//[DllImport("kernel32.dll")]
		//static extern bool FreeConsole();
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static String ^domain;
		static String ^new_domain;
		static String ^ipaddr;
		static String ^old_ipaddr;
		static String ^domain_alias_list;
		static String ^type;
		static String ^host;
		static String ^value;
		static String ^new_host;
		static String ^new_value;
		static String ^domain_list;
		static String ^opt;
		static String ^port;
		static String ^weight;
		static String ^old_host;
		static String ^old_type;
		static String ^old_value;
		static String ^zone_data;

		static String ^executed_command;


		static Int64 soaSerial();

		static void init();

		static void create();

		static void update();

		static void remove();

		static void createRecord();

		static void updateRecord();

		static void removeRecord();

		static void changeIpAddr();

		static void restore();

		static void restoreTemplate();

		static void createRecordTemplate();

		static void updateRecordTemplate();

		static void removeRecordTemplate();

		static void createDnsZone(String ^domain, String ^ipaddr);

		static void changeSoaRecords(String ^domainname);

		static void changeSoaSerial(String ^domainname);

		static array<String^> ^domainAliases(String ^domainname);
	};
}
