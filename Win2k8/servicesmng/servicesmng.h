#include "../env/Networking.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../env/Services.h"

using namespace System;
using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace Environments;

namespace Tools
{
	private ref class servicesmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Services ^serv = gcnew Services();

		static array<String^> ^web;
		static array<String^> ^database;
		static array<String^> ^dns;
		static array<String^> ^ftp;
		static array<String^> ^smtp;
		static array<String^> ^imap;
		static array<String^> ^pop;
		static array<String^> ^cron;
		static array<String^> ^ssh;
		static array<String^> ^spam;
		static array<String^> ^antivirus;


		static void createVariables();

		static void initService(String ^service, String ^command);

		static void showStatus();
	};
}

//.cpp file code:
