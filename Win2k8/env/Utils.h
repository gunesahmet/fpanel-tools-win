#pragma once
#include "FException.h"
#include <vector>
#include "../Windows/Headers/Logger.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Principal;
using namespace System::Text;
using namespace System::Text::RegularExpressions;
using namespace System::Threading;
using namespace Microsoft::Win32;

namespace Environments
{
	public ref class FUtils
	{
	public:
		String ^exit_code;

		bool isAdmin();

		DateTime UnixTimestampToDateTime(double unixTime);

		String ^exec(String ^command, String ^parameters);

		Int64 unixTimeStamp();

		bool moveFile(String ^from, String ^there);

		bool writeFile(String ^file, String ^data);

		bool copyFolder(String ^source, String ^target, bool sub_dir);

		bool copyFile(String ^from, String ^there);

		bool appendFile(String ^file, String ^str);

		bool removeFile(String ^file);

		bool removeFolder(String ^folder, bool sub);

		bool createFolder(String ^folder);

		bool removeLineFromFile(String ^file, String ^str);

		void replaceLine(String ^file, String ^needed, String ^haystack);

		void replaceAll(String ^%str, String ^old_str, String ^new_str);
		
		bool iniReplace(String ^%str, String ^old_str, String ^new_str);
		
		String ^defaultIpAddress();

		String ^Trim(String ^str);

		String ^Base64Encode(String ^str);

		String ^Base64Decode(String ^str);

		void FindInstalls(RegistryKey ^regKey, System::Collections::Generic::List<String^> ^keys, Dictionary<String^, String^> ^%installed);

		array<String^> ^split(String ^str, array<Char> ^separators);
	};

}