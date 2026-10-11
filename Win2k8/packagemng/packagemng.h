#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/Services.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Management;
using namespace System::Net;
using namespace System::Text::RegularExpressions;
using namespace Environments;

namespace Tools
{
	private ref class packagemng
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
		static Dictionary<String^, String^> ^system_packages = gcnew Dictionary<String^, String^>();


		static void checkSystem();

		static void listPackage();
	};
}
