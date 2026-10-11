//.h file code:

#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class crontabmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static DateTime dateTime = DateTime::UtcNow.Date;


		static void createCronJob(String ^minute, String ^hour, String ^day, String ^week, String ^month, String ^timestamp, String ^username, String ^start_date, String ^job, String ^domain, String ^password);

		static void removeCronJob(String ^username, String ^timestamp);

		static void removePermCronJob(String ^username);
		static void test();
	};
}
