#include "Utils.h"

using namespace System;
using namespace System::IO;
using namespace System::Net::NetworkInformation;
using namespace System::Net::Sockets;
using namespace System::Text::RegularExpressions;

namespace Environments
{
	public ref class Networking
	{
	private:
		FUtils ^utils;

	public:
		String ^defaultIpAddress();
		String ^totalTraffic();

	private:
		void InitializeInstanceFields();

	public:
		Networking()
		{
			InitializeInstanceFields();
		}
	};
}
