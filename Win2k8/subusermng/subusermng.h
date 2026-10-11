//.h file code:

#include "../env/Utils.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/User.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class subusermng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static FPanel::Windows::User ^users = gcnew FPanel::Windows::User();

		static String ^domain;
		static String ^prefix;
		static String ^user;
		static String ^passwd;
		static String ^subdomain;
		static String ^sub_user;
		static String ^new_sub_user;


		static void create();

		static void update();

		static void remove();

		static void suspend();

		static void unsuspend();
	};
}
