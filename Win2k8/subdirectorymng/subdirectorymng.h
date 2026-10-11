#include "../env/Utils.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/SubDirectory.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	public ref class subdirectorymng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static FPanel::Windows::SubDirectory ^sub_directories = gcnew FPanel::Windows::SubDirectory();

		static String ^domain;
		static String ^prefix;
		static String ^user;
		static String ^ftp_type;
		static String ^subdomain;
		static String ^sub_user;
		static String ^new_sub_user;


		static void create();

		static void update();

		static void remove();
	};
}
