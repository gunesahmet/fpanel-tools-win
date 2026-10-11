#include "../env/Utils.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"

using namespace System;
using namespace Environments;

namespace Tools
{
	private ref class protectmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static FileLogger ^logger = gcnew FileLogger();
		static Env ^env = gcnew Env();
		static Validations ^validate = gcnew Validations();
		static String ^domain;
		static String ^prefix;
	};
}