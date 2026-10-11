#include "networkmng.h"

using namespace System;
using namespace System::IO;
using namespace System::Net::NetworkInformation;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void networkmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);


		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");
		eth = main_case["int"];
		netmask = main_case["netmask"];
		ipaddr = main_case["ipaddr"];
		host = main_case["host"];
		domain = main_case["domain"];
		index = main_case["index"];
		default_gateway = main_case["gateway"];
		config = main_case["config"];

		if (process == "init") {
			init();
		}
		else if (process == "create") {
		}
		else if (process == "remove") {
		}
		else if (process == "show") {
			show();
		}
		else if (process == "show-default-ip") {
			showDefaultIp();
		}
		else if (process == "show-gw") {
			showGw();
		}
		else if (process == "hostname") {
			hostname(host, domain);
		}
		else if (process == "set-config") {
			setConfig(config);
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void networkmng::init()
	{
		try {
			Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
			RegistryKey ^myKey = localMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", true);
			if (myKey != nullptr) {
				myKey->SetValue("IPEnableRouter", "1", RegistryValueKind::DWord);
				myKey->Close();
			}
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1804);
		}

		try {
			String ^f_ipaddresses = utils->exec("wmic", "NICCONFIG GET IpAddress");
			String ^full_ipaddresses_r = f_ipaddresses->Replace("\n", "")->Replace("\r", "")->Replace("IPAddress", "")->Trim()->Replace("{", "")->Replace("}", "")->Replace("\t", "")->Replace(" ", "")->Replace("\\\"", ",")->Replace("\",\"", ",")->Replace("\"", "");
			array<String^> ^full_ipaddresses = full_ipaddresses_r->Split(',');

			String ^default_ipaddress = network->defaultIpAddress();
			String ^indexes = utils->exec("wmic", "nic where \"netconnectionid like '%'\" get interfaceindex");

			array<String^> ^index_arr = indexes->Replace("InterfaceIndex", "")->Trim()->Split('\n');

			for each (auto index in index_arr) {
				String ^indexing = index->Trim();
				if (String::IsNullOrEmpty(indexing)) {
					continue;
				}
				String ^ipaddresses = utils->exec("wmic", "NICCONFIG WHERE \"InterfaceIndex=" + indexing + " and IPEnabled=true\" GET IpAddress");
				String ^ipaddress = Regex::Replace(ipaddresses->Replace("IPAddress", "")->Trim(), "^\\s+$[\\n]*", "", RegexOptions::Multiline);
				ipaddress = ipaddress->Replace("\"", "")->Replace("{", "")->Replace("}", "")->Replace(",", "");
				array<String^> ^ipaddress_arr = ipaddress->Trim()->Split(' ');

				for (int i = 0; i <= ipaddress_arr->Length; i++) {
					try {
						if (ipaddress_arr[i]->Trim() == default_ipaddress)
							continue;
						if (validate->fileIsContains(env->network_conf, "/\b" + ipaddress_arr[i]->Trim() + "\b/") == false) {
							if (validate->validateIpAddress(ipaddress_arr[i])) {
								remove(indexing, ipaddress_arr[i]->Trim());
							}
						}
					}
					catch (Exception ^e) {
						logger->debugLog("networkmng: " + e->Message, 1804);
					}
				}
			}

			StreamReader ^sr = File::OpenText(env->network_conf);
			try {
				array<String^> ^lines = File::ReadAllLines(env->network_conf);
				Regex ^searchRegex = gcnew Regex("/\b" + default_ipaddress + "\b/");

				for each (String ^line in lines) {
					if (searchRegex->IsMatch(line)) {
						continue;
					}
					else {
						array<String^> ^config_arr = line->Split(',');

						String ^eth = config_arr[0]->Trim();
						String ^eth_index = config_arr[1]->Trim();
						String ^ip_addr = config_arr[2]->Trim();
						String ^ip_netmask = config_arr[3]->Trim();
						String ^ip_gateway = nullptr;
						//Console::WriteLine("ipaddress_arr[i]: {0}", ip_addr);

						try {
							if (config_arr->Length == 5) {
								ip_gateway = config_arr[4]->Trim();
							}
						}
						catch (...) {
							//Console::WriteLine(e);
						}
						finally {

						}
						create(eth_index, ip_addr, ip_netmask, ip_gateway);
					}
				}
				sr->Close();
			}
			catch (Exception ^e) {
				Console::WriteLine(e);
			}
			finally {
				delete sr;
			}

		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1804);
		}


	}

	void networkmng::create(String ^index, String ^ipaddr, String ^netmask, String ^default_gateway)
	{
		String ^result = "";
		try {
			if (String::IsNullOrEmpty(default_gateway))
				result = utils->exec("cmd", "/c netsh interface ipv4 add address \"" + index + "\" " + ipaddr + " " + netmask);
			else
				result = utils->exec("cmd", "/c netsh interface ipv4 add address \"" + index + "\" static " + ipaddr + " " + netmask + " " + default_gateway + " 266");
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1801);
		}
	}

	void networkmng::remove(String ^index, String ^ipaddr)
	{
		try {
			utils->exec("netsh", "interface ipv4 delete address \"" + index + "\" addr=" + ipaddr);

		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1802);
		}

	}

	void networkmng::show()
	{
		try {
			//String ^result = utils->exec("cmd", "/c nslookup myip.opendns.com. resolver1.opendns.com");
			//array<String^> ^items = result->Split(gcnew array<String^> {Environment::NewLine}, StringSplitOptions::RemoveEmptyEntries);
			//String ^default_ip_addr = items[items->Length - 1]->Replace("Address: ","")->Trim();
			String ^default_ip_addr = network->defaultIpAddress();

			String ^json = nullptr;
			String ^indexes = utils->exec("wmic", "nic where \"netconnectionid like '%'\" get interfaceindex");
			bool ip_addr_found = false;
			array<String^> ^index_arr = indexes->Split('\n');
			String ^encoded_eth;
			String ^indexing;

			int i = 0;
			for each (auto index in index_arr)
			{
				if (i == 0) {
					i++;
					continue;
				}

				indexing = index->Trim();
				if (String::IsNullOrEmpty(indexing)) {
					i++;
					continue;
				}
				String ^ethernet = utils->exec("wmic", "nic where interfaceindex=" + indexing + " get netconnectionid");

				String ^eth = Regex::Replace(ethernet->Replace("NetConnectionID", "")->Trim(), "^\\s+$[\\n]*", "", RegexOptions::Multiline);

				String ^ipaddresses = utils->exec("wmic", "NICCONFIG WHERE \"InterfaceIndex=" + indexing + " and IPEnabled=true\" GET IpAddress");
				String ^ipaddress = Regex::Replace(ipaddresses->Replace("IPAddress", "")->Trim(), "^\\s+$[\\n]*", "", RegexOptions::Multiline);
				ipaddress = ipaddress->Replace("\"", "")->Replace("{", "")->Replace("}", "")->Replace(",", "");
				array<String^> ^ipaddress_arr = ipaddress->Split(' ');

				String ^netmasks = utils->exec("wmic", "NICCONFIG WHERE \"InterfaceIndex=" + indexing + " and IPEnabled=true\" GET IpSubnet");
				netmasks = Regex::Replace(netmasks->Replace("IPSubnet", "")->Trim(), "^\\s+$[\\n]*", "", RegexOptions::Multiline);
				netmasks = netmasks->Replace("\"", "")->Replace("{", "")->Replace("}", "")->Replace(",", "");
				array<String^> ^netmasks_arr = netmasks->Split(' ');

				for (int k = 0; k <= ipaddress_arr->Length; k++) {
					try {
						String ^ip_addr = ipaddress_arr[k]->Trim();
						String ^netmask_addr = netmasks_arr[k]->Trim();
						if (String::IsNullOrEmpty(ip_addr))
							continue;
						if (ip_addr == default_ip_addr)
							ip_addr_found = true;

						if (validate->validateIpAddress(ip_addr) == false)
							continue;

						encoded_eth = utils->Base64Encode(eth);
						String ^gw = getGateway(ip_addr);

						if (String::IsNullOrEmpty(json))
							json = "[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + ip_addr + "\",\"" + netmask_addr + "\",\"" + gw + "\"]";
						else
							json = json + ",[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + ip_addr + "\",\"" + netmask_addr + "\",\"" + gw + "\"]";
					}
					catch (...) {
						continue;
					}

				}
			}
			if (ip_addr_found == false) {
				if (String::IsNullOrEmpty(json))
					json = "[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + default_ip_addr + "\",\"255.255.255.0\",\"\"]";
				else
					json = json + ",[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + default_ip_addr + "\",\"255.255.255.0\",\"\"]";
			}
			Console::WriteLine("[" + json + "]");
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1803);
		}
	}

	void networkmng::showDefaultIp()
	{
		try {
			String ^default_ip_addr = network->defaultIpAddress();
			Console::WriteLine(default_ip_addr);
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1804);
		}

	}

	void networkmng::showGw()
	{
		try {
			String ^default_ip_addr = network->defaultIpAddress();
			Console::WriteLine(getGateway(default_ip_addr));
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1805);

		}

	}

	void networkmng::hostname(String ^host, String ^domain)
	{
		try {
			String ^old_hostname = Environment::MachineName;
			utils->exec("netdom", "renamecomputer " + old_hostname + " /newname:" + host + " /force");

			Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
			RegistryKey ^dKey = localMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters", true);
			if (dKey != nullptr) {
				dKey->SetValue("NV Domain", domain, RegistryValueKind::String);
				dKey->SetValue("Domain", domain, RegistryValueKind::String);
				dKey->Close();
			}
			RegistryKey ^d2Key = localMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\DNSRegisteredAdapters", false);
			if (d2Key != nullptr) {
				array<String^> ^subkeys = d2Key->GetSubKeyNames();
				for (int i = 0; i < subkeys->Length; i++) {
					RegistryKey ^d3Key = localMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\DNSRegisteredAdapters\\" + subkeys[i], true);
					if (d3Key != nullptr) {
						d3Key->SetValue("PrimaryDomainName", domain, RegistryValueKind::String);
						d3Key->Close();
					}

				}
				d2Key->Close();
			}
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1806);
		}
	}

	String ^networkmng::getGateway(String ^ipaddress)
	{
		try {
			String ^gateway = utils->exec("route", "print -4");
			array<String^> ^gws = gateway->Split('\n');
			String ^result = nullptr;
			for each (String ^gw in gws)
			{
				if (System::Text::RegularExpressions::Regex::IsMatch(gw, "\\s0.0.0.0.*" + ipaddress)) {
					try {
						array<String^> ^gw_arr = gw->Replace("0.0.0.0", "")->Trim()->Split(' ');
						result = gw_arr[0];
					}
					catch (Exception ^e) {
						logger->debugLog("networkmng: " + e->Message, 1807);
					}
				}
			}
			return result;
		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1808);
		}
		return "";


	}

	bool networkmng::checkIpAddressOnSystem(array<String^> ^allips, String ^ipaddress)
	{
		try {
			for each (String ^s_ip in allips)
			{
				if (ipaddress == s_ip->Trim()) {
					return true;
				}
			}

		}
		catch (Exception ^e) {
			logger->debugLog("networkmng: " + e->Message, 1809);
		}
		return false;

	}

	void networkmng::setConfig(String ^config)
	{
		if (config == nullptr) {
			logger->errorLog("Missing parameter", 1804);
			System::Environment::Exit(1001);
		}
		String ^decoded_content = utils->exec(env->fcrypt, "-d --in=\"" + config + "\"");
		decoded_content = utils->Trim(decoded_content);
		utils->writeFile(env->network_conf, decoded_content);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	networkmng ^mainProcess = gcnew networkmng();
	mainProcess->Main(Args);
	return 0;
}