#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Net;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class Network
		{
		public:
			void getNetworkList()
			{
				String ^strHostName = Dns::GetHostName();

				// Find host by name
				IPHostEntry ^iphostentry = Dns::GetHostByName(strHostName);

				// Enumerate IP addresses
				int nIP = 0;
				for each (IPAddress ^ipaddress in iphostentry->AddressList)
				{
					Console::WriteLine(ipaddress);
				}
			}
		};
	}
}