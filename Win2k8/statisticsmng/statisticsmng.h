#pragma once
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../env/Env.h"
#include "../env/Utils.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class statisticsmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static String ^domain;
		static String ^domainUser;
		static String ^lastInit;
		static String ^serverAddress;
		static String ^statisticsRandom;
		static String ^statisticsTime;
		static String ^updateRandom;
		static String ^updateTime;
		static String ^updateAuto;

		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static FileLogger ^logger = gcnew FileLogger();

		static void CalculateSystemCheckFlag();
		static void CalculateSystem();
		static void CalculateDomainCheckFlag();
		static void CalculateDomain();
		static void SetConfigCheckFlag();
		static void SetConfig();
	};
}
