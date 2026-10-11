#include "../env/Logger.h"
#include "../env/Networking.h"
#include "../env/Env.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/SecEdit.h"
#include "../Windows/Headers/Security.h"

using namespace Microsoft::VisualBasic::Devices;
using namespace Microsoft::Win32;
using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class adminmng
	{
	public:
		static void Main(array<String^> ^Args);
	private:
		static FUtils ^utils = gcnew FUtils();
		static FileLogger ^logger = gcnew FileLogger();
		static Networking ^network = gcnew Networking();
		static Env ^env = gcnew Env();
		static Validations ^validate = gcnew Validations();

		static String ^systemInfoNetwork();

		static void createSysConf();

		static String ^cpuModel();

		static void init(bool reset);

		static void reboot();

		static void shutdown();

		static void sysInfo(String ^cpu_usage);

		static void createSupportUser(String ^username, String ^password);

		static void removeSupportUser(String ^username);

	};
}
