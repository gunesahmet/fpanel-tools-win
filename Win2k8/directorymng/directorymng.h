#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../Windows/Headers/FileManager.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/Directories.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class directorymng
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
		static String ^user;
		static String ^new_user;
		static String ^force;
		static FPanel::Windows::Directories ^directories = gcnew FPanel::Windows::Directories();

		static void init();

		static void create();

		static void update();

		static void remove();

	};
}
