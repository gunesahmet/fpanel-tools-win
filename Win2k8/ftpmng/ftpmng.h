#include "../env/Utils.h"
#include "../env/Logger.h"
#include "../env/Env.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../Windows/Headers/FtpSettings.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class ftpmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static FileLogger ^logger = gcnew FileLogger();
		static Env ^env = gcnew Env();
		static FPanel::Windows::FtpSettings ^ftp_settings = gcnew FPanel::Windows::FtpSettings();
		static Validations ^validate = gcnew Validations();
		static String ^domain;
		static String ^sess_id;
		static String ^timeout_idle;
		static String ^timeout_transfer;
		static String ^max_instance;
		static String ^max_clients;
		static String ^message;


		static void activeSessions();

		static void killSession();

		static void ftpSettings();

	};
}