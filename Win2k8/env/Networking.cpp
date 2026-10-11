#include "Stdafx.h"
#include "Networking.h"

namespace Environments
{
	String ^Networking::defaultIpAddress()
	{
		List<String^> ^ipAddrList = gcnew List<String^>();
		String ^output = "127.0.0.1";
		for each (NetworkInterface ^item in NetworkInterface::GetAllNetworkInterfaces()) {
			if (item->NetworkInterfaceType == NetworkInterfaceType::Ethernet && item->OperationalStatus == OperationalStatus::Up) {
				for each (UnicastIPAddressInformation ^ip in item->GetIPProperties()->UnicastAddresses) {
					if (ip->Address->AddressFamily == AddressFamily::InterNetwork) {
						ipAddrList->Add(ip->Address->ToString());
					}
				}
			}
		}

		// This is for google cloud or aws or azure
		/*String ^result = utils->exec("cmd", "/c nslookup myip.opendns.com. resolver1.opendns.com");
		array<String^> ^items = result->Split(gcnew array<String^> {Environment::NewLine}, StringSplitOptions::RemoveEmptyEntries);
		Console::WriteLine(items[items->Length - 1]->Replace("Address: ","")->Trim());*/
		return ipAddrList[0];
	}

	String ^Networking::totalTraffic()
	{
		long long total_traffic = 0;
		for each (NetworkInterface ^item in NetworkInterface::GetAllNetworkInterfaces()) {
			if (item->NetworkInterfaceType == NetworkInterfaceType::Ethernet && item->OperationalStatus == OperationalStatus::Up) {
				total_traffic += item->GetIPv4Statistics()->BytesReceived;
			}
		}
		return total_traffic.ToString();
	}

	void Networking::InitializeInstanceFields()
	{
		utils = gcnew FUtils();
	}
}