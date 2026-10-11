#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"

using namespace Microsoft::Win32;
using namespace System;
using namespace System::Globalization;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace Environments;


namespace Tools
{


	private ref class datemng
	{
		[StructLayout(LayoutKind::Sequential)]
		value class SYSTEMTIME
		{
		public:
			UInt16 wYear;
			UInt16 wMonth;
			UInt16 wDayOfWeek;
			UInt16 wDay;
			UInt16 wHour;
			UInt16 wMinute;
			UInt16 wSecond;
			UInt16 wMilliseconds;
		};

	private:
		static String^ getCurrentRegion();
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static DateTime dateTime = DateTime::UtcNow.Date;
		[DllImport("kernel32.dll")]
		static UInt32 SetSystemTime(SYSTEMTIME %lpSystemTime);

		static void setTime(String ^time);

		static void setDate(String ^date);

		static void setTimeZone(String ^timezone, String ^ntp_server);

		static void showDate();
	public:
		static void Main(array<String^> ^Args);


	};


}


