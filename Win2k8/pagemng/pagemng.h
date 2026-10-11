#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/Services.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class pagemng
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
		static String ^domain;
		static String ^prefix;
		static String ^ipaddr;
		static String ^user;
		static String ^php;
		static String ^python;
		static String ^perl;
		static String ^asp;
		static String ^aspnet;


		static void setDomain();

		static void init();
	};
}
