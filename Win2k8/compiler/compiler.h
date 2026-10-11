#include "../env/MainCase.h"
#include "../env/Utils.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class compiler
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();

		static void compile(String ^path, String ^target_path);

		static void compileTool(String ^path,String ^file, String ^target_path);

	};
}

