#pragma once
#include "MailEnable.h"
#include "HMail.h"

#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace Environments;

namespace Tools
{
	private ref class mailqueuemng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static Mail::Enable ^mailEnable = gcnew Mail::Enable();
		static HMail::Server ^hMail = gcnew HMail::Server();

		static Int32 start;
		static Int32 end;
		static String ^file;
		static String ^mail_queue_dir;
		static String ^mail_queue_msg_dir;
		static DateTime current_date = DateTime::Now;

		static bool mail_enable_status;
		static bool hmail_status;

		static void detectServer();

		static void queueLists();

		static void clearQueue();

		static void removeMessage();
	};
}