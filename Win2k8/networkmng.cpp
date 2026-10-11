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
		if (File::Exists("c:\\fpanel\\core\\bin\\.debug"))
		{
			try
			{
				String ^parameters = String::Join(" ", Args);
				String ^str = System::AppDomain::CurrentDomain->FriendlyName + " " + parameters;

				StreamWriter ^stream = (gcnew FileInfo("c:\\fpanel\\tmp\\params.logs"))->AppendText();
				try
				{
					stream->WriteLine(str);
				}
				finally
				{
					delete stream;
				}
			}
			catch (Exception ^e1)
			{
			}
		}
		if (utils->isAdmin() == false)
		{
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0)
		{
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

		if (process == "init")
		{
				init();
		}
		else if (process == "create")
		{
		}
		else if (process == "remove")
		{
		}
		else if (process == "show")
		{
				show();
		}
		else if (process == "show-route")
		{
				showRoute();
		}
		else if (process == "show-gw")
		{
				showGw();
		}
		else if (process == "hostname")
		{
				hostname();
		}
		else
		{
				System::Environment::Exit(0);
		}
	}

	void networkmng::init()
	{
		try
		{
			String ^f_ipaddresses = utils->exec("wmic", "NICCONFIG GET IpAddress");
			String ^full_ipaddresses_r = f_ipaddresses->Replace("\n", "")->Replace("\r", "")->Replace("IPAddress", "")->Trim()->Replace("{", "")->Replace("}", "")->Replace("\t", "")->Replace(" ", "")->Replace("\\\"", ",")->Replace("\",\"", ",")->Replace("\"","");
			array<String^> ^full_ipaddresses = full_ipaddresses_r->Split(',');

			String ^default_ipaddress = network->defaultIpAddress();
			String ^indexes = utils->exec("wmic", "nic where \"netconnectionid like '%'\" get interfaceindex");

			array<String^> ^index_arr = indexes->Replace("InterfaceIndex", "")->Trim()->Split('\n');
			for each (auto index in index_arr)
			{
				String ^indexing = index->Trim();
				if (indexing == nullptr)
				{
					continue;
				}

				String ^ipaddresses = utils->exec("wmic", "NICCONFIG WHERE \"InterfaceIndex=" + indexing + " and IPEnabled=true\" GET IpAddress");
				String ^ipaddress = Regex::Replace(ipaddresses->Replace("IPAddress", "")->Trim(), "^\\s+$[\\n]*", "", RegexOptions::Multiline);
				ipaddress = ipaddress->Replace("\"", "")->Replace("{", "")->Replace("}", "")->Replace(",", "");
				array<String^> ^ipaddress_arr = ipaddress->Split(' ');

				for (int i = 0; i <= ipaddress_arr->Length; i++)
				{
					try
					{
						//Console.WriteLine("Default IP: " + default_ipaddress);
						if (ipaddress_arr[i]->Trim() == default_ipaddress)
							continue;

						if (validate->fileIsContains(env->network_conf, "/\b" + ipaddress_arr[i]->Trim() + "\b/") == false)
						{
							if (validate->validateIpAddress(ipaddress_arr[i]))
							{
								//Console.WriteLine("Silinecek: " + ipaddress_arr[i]);
								remove(indexing, ipaddress_arr[i]->Trim());
							}
						}
					}
					catch (Exception ^e)
					{

					}
				}
			}


			StreamReader ^sr = File::OpenText(env->network_conf);
			try
			{
				array<String^> ^lines = File::ReadAllLines(env->network_conf);
				Regex ^searchRegex = gcnew Regex("/\b" + default_ipaddress + "\b/");

				for each (String ^line in lines)
				{

					if (searchRegex->IsMatch(line))
					{
						continue;
					}
					else
					{
						array<String^> ^config_arr = line->Split(',');

						String ^eth = utils->Base64Decode(config_arr[0]->Trim());
						String ^eth_index = config_arr[1]->Trim();
						String ^ip_addr = config_arr[2]->Trim();
						String ^ip_netmask = config_arr[3]->Trim();
						String ^ip_gateway = nullptr;

						try
						{
							ip_gateway = config_arr[4]->Trim();
						}
						catch (Exception ^e)
						{
						}
						if (checkIpAddressOnSystem(full_ipaddresses, ip_addr) == false)
						{
							create(eth_index, ip_addr, ip_netmask, ip_gateway);
						}
					}
				}
				sr->Close();
			}
			finally
			{
				delete sr;
			}

		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1804);
		}
	}

	void networkmng::create(String ^index, String ^ipaddr, String ^netmask, String ^default_gateway)
	{
		String ^result = "";
		try
		{
			if (String::IsNullOrEmpty(default_gateway))
				result = utils->exec("cmd", "/c netsh interface ipv4 add address \"" + index + "\" " + ipaddr + " " + netmask);
			else
				result = utils->exec("cmd", "/c netsh interface ipv4 add address \"" + index + "\" static " + ipaddr + " " + netmask + " " + default_gateway + " 266");
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1801);
		}
	}

	void networkmng::remove(String ^index, String ^ipaddr)
	{
		try
		{
			utils->exec("netsh", "interface ipv4 delete address \"" + index + "\" addr=" + ipaddr);

		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1802);
		}

	}

	void networkmng::show()
	{
		try
		{
			// Interface, Index, Ip Adress, Netmask, Gateway
			String ^json = nullptr;
			String ^indexes = utils->exec("wmic", "nic where \"netconnectionid like '%'\" get interfaceindex");

			array<String^> ^index_arr = indexes->Split('\n');

			int i = 0;
			for each (auto index in index_arr)
			{
				if (i == 0)
				{
					i++;
					continue;
				}

				String ^indexing = index->Trim();
				if (indexing == nullptr)
				{
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


				for (int k = 0; k <= ipaddress_arr->Length; k++)
				{
					try
					{
						String ^ip_addr = ipaddress_arr[k]->Trim();
						String ^netmask_addr = netmasks_arr[k]->Trim();
						if (ip_addr == nullptr)
							continue;
						if (validate->validateIpAddress(ip_addr) == false)
						{
							continue;
						}
						auto encoded_eth = utils->Base64Encode(eth);
						String ^gw = getGateway(ip_addr);
						if (json == nullptr)
							json = "[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + ip_addr + "\",\"" + netmask_addr + "\",\"" + gw + "\"]";
						else
							json = json + ",[\"" + indexing + "\",\"" + encoded_eth + "\",\"" + ip_addr + "\",\"" + netmask_addr + "\",\"" + gw + "\"]";
					}
					catch (Exception ^e)
					{
						continue;
					}

				}

			}
			Console::WriteLine("[" + json + "]");
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1803);
		}
	}

	void networkmng::showRoute()
	{
		try
		{
			String ^default_ip_addr = network->defaultIpAddress();
			Console::WriteLine(default_ip_addr);
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1804);

		}

	}

	void networkmng::showGw()
	{
		try
		{
			String ^default_ip_addr = network->defaultIpAddress();
			Console::WriteLine(getGateway(default_ip_addr));
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1805);

		}

	}

	void networkmng::hostname()
	{
		try
		{
			String ^old_hostname = Environment::MachineName;
			utils->exec("netdom", "renamecomputer " + old_hostname + " /newname:" + host + " /force");
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1806);
		}
	}

	String ^networkmng::getGateway(String ^ipaddress)
	{
		try
		{
			String ^gateway = utils->exec("route", "print -4");
			array<String^> ^gws = gateway->Split('\n');
			String ^result = nullptr;
			for each (String ^gw in gws)
			{
				if (System::Text::RegularExpressions::Regex::IsMatch(gw, "\\s0.0.0.0.*" + ipaddress))
				{
					try
					{
						array<String^> ^gw_arr = gw->Replace("0.0.0.0", "")->Trim()->Split(' ');
						result = gw_arr[0];
					}
					catch (Exception ^e)
					{
						logger->debugLog("networkmng: " + e->Message, 1807);
					}
				}
			}
			return result;
		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1808);
		}
		return "";


	}

	bool networkmng::checkIpAddressOnSystem(array<String^> ^allips, String ^ipaddress)
	{
		try
		{
			for each (String ^s_ip in allips)
			{
				if (ipaddress == s_ip->Trim())
				{
					return true;
				}
			}

		}
		catch (Exception ^e)
		{
			logger->debugLog("networkmng: " + e->Message, 1809);
		}
		return false;

	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	networkmng ^mainProcess = gcnew networkmng();
	mainProcess->Main(Args);
	return 0;
}