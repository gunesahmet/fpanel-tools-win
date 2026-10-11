#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Utils.h"
#include "../env/Logger.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/FileManager.h"


using namespace Microsoft::Win32;
using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;
using namespace Environments;

namespace Tools
{
	private ref class permissionmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static FileLogger ^logger = gcnew FileLogger();
		static FPanel::Windows::FileManager ^file_manager = gcnew FPanel::Windows::FileManager();
		static Env ^env = gcnew Env();

		static String ^domain;
		static String ^prefix;
		static String ^file;
		static String ^allow;
		static String ^deny;
		static String ^permission;
		static String ^user;


		static void getPermission();

		static void setPermission();

	};
}
