#include "domaliasmng.h"

using namespace System;
using namespace System::IO;
using namespace Microsoft::Web::Administration;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void domaliasmng::Main(array<String^> ^Args)
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
		domain = main_case["domain"];
		prefix = main_case["prefix"];
		alias = main_case["alias"];
		mail_serv = main_case["mail-serv"];
		web_serv = main_case["web-serv"];
		sync = main_case["sync"];
		new_alias = main_case["new-alias"];
		ipaddr = main_case["ipaddr"];

		if (process == "create") {
			create();
		}
		else if (process == "update") {
			update();
		}
		else if (process == "remove") {
			remove();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void domaliasmng::create()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(prefix) || String::IsNullOrEmpty(alias)) {
				logger->errorLog("Missing arguments", 1605);
				System::Environment::Exit(1605);
			}

			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found in iis", 1606);
				System::Environment::Exit(1606);
			}

			if (validate->isDomainExistsIis(alias) == true) {
				logger->errorLog(alias + " already exists in iis", 1607);
				System::Environment::Exit(1607);
			}

			if (web_serv == "on") {
				addBinding(domain, alias, ipaddr);
				addBinding(domain, "www." + alias, ipaddr);
			}

			if (mail_serv == "on") {
				// @TODO Yazýlacak mail enable için
			}

			bindSync(domain, alias);

		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 1610);
		}

	}

	void domaliasmng::update()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(alias) || String::IsNullOrEmpty(prefix)) {
				logger->errorLog("Missing arguments", 1605);
				System::Environment::Exit(1605);
			}

			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found in iis", 1606);
				System::Environment::Exit(1606);
			}

			if (validate->isDomainExistsIis(new_alias) == false) {
				logger->errorLog(new_alias + " already exists in iis", 1607);
				System::Environment::Exit(1607);
			}

			String ^ipaddr = getDomainIp(domain);
			if (mail_serv == "on") {
				// @TODO Yazýlacak mail enable için
			}

			if (sync == "on") {
				if (!String::IsNullOrEmpty(new_alias)) {
					removeFromDNS(alias);
					bindSync(domain, new_alias);
				}
				else {
					bindSync(domain, alias);
				}
			}
			else {
				removeFromDNS(alias);
			}

			if (web_serv == "on") {
				if (!String::IsNullOrEmpty(new_alias)) {
					if (validate->isDomainExistsIis(alias) == true) {
						// Eski alias kaldýrýlýyor
						removeBinding(domain, alias, ipaddr);
						removeBinding(domain, "www." + alias, ipaddr);
					}
					// Yeni alias ekleniyor
					addBinding(domain, new_alias, ipaddr);
					addBinding(domain, "www." + new_alias, ipaddr);
				}
				else {
					addBinding(domain, alias, ipaddr);
					addBinding(domain, "www." + alias, ipaddr);
				}
			}
			else {
				if (validate->isDomainExistsIis(alias) == true) {
					// Eski alias kaldýrýlýyor
					removeBinding(domain, alias, ipaddr);
					removeBinding(domain, "www." + alias, ipaddr);
				}
			}
		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 16611);
		}
	}

	void domaliasmng::remove()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(alias)) {
				logger->errorLog("Missing arguments", 1605);
				System::Environment::Exit(1605);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found in iis", 1606);
				System::Environment::Exit(1606);
			}
			String ^ipaddr = getDomainIp(domain);
			removeBinding(domain, alias, ipaddr);
			removeBinding(domain, "www." + alias, ipaddr);
		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 1612);
		}

	}

	ConfigurationElement ^domaliasmng::FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
	{
		for each (ConfigurationElement ^element in collection) {
			if (String::Equals(element->ElementTagName, elementTagName, StringComparison::OrdinalIgnoreCase)) {
				bool matches = true;
				for (int i = 0; i < keyValues->Length; i += 2) {
					Object ^o = element->GetAttributeValue(keyValues[i]);
					String ^value = nullptr;
					if (o != nullptr) {
						value = o->ToString();
					}

					if (!String::Equals(value, keyValues[i + 1], StringComparison::OrdinalIgnoreCase)) {
						matches = false;
						break;
					}
				}
				if (matches) {
					return element;
				}
			}
		}
		return nullptr;
	}

	void domaliasmng::addBinding(String ^domainname, String ^aliasname, String ^ipaddr)
	{
		try {
			ServerManager ^serverManager = gcnew ServerManager();
			Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

			ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

			ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

			ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domainname);
			if (siteElement == nullptr)
				throw gcnew InvalidOperationException(domain + " not found!");

			ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");

			ConfigurationElement ^bindingElement = bindingsCollection->CreateElement("binding");
			bindingElement["protocol"] = "http";
			bindingElement["bindingInformation"] = ipaddr + ":80:" + aliasname;
			bindingsCollection->Add(bindingElement);

			serverManager->CommitChanges();
			delete serverManager;
		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 1614);
		}
	}

	String ^domaliasmng::getDomainIp(String ^domainname)
	{
		try {
			ServerManager ^serverManager = gcnew ServerManager();
			Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

			ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

			ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

			ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domainname);
			if (siteElement == nullptr)
				throw gcnew InvalidOperationException(domain + " not found!");

			ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");
			String ^domain_ip = "";

			int i = 0;
			for each (ConfigurationElement ^field in bindingsCollection) {
				if (i == 0) {
					String ^domain_data = field->GetAttributeValue("bindingInformation")->ToString();
					array<String^> ^domain_data_splt = domain_data->Split(':');
					domain_ip = domain_data_splt[0];
				}
			}
			return domain_ip;
			delete serverManager;
		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 1615);
			return "";
		}
	}

	void domaliasmng::removeBinding(String ^domainname, String ^aliasname, String ^ipaddr)
	{
		ServerManager ^serverManager = gcnew ServerManager();
		Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
		ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
		ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

		ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domainname);
		if (siteElement == nullptr)
			throw gcnew InvalidOperationException("Element not found!");

		ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");

		ConfigurationElement ^bindingElement = FindElement(bindingsCollection, "binding", "protocol", "http", "bindingInformation", ipaddr + ":80:" + aliasname);
		if (bindingElement == nullptr)
			throw gcnew InvalidOperationException("Element not found!");

		bindingsCollection->Remove(bindingElement);

		serverManager->CommitChanges();
		delete serverManager;
	}

	void domaliasmng::bindSync(String ^domainname, String ^aliasname)
	{
		try {
			utils->appendFile(env->dns_zone_conf, "zone \"" + aliasname + "\" { type master; file \"" + env->dns_zone_dir->Replace("\\", "/") + aliasname + "\";};");
			StreamReader ^sr = File::OpenText(env->dns_zone_dir + domainname);
			try {
				array<String^> ^lines = File::ReadAllLines(env->dns_zone_dir + domainname);
				StreamWriter ^sw = gcnew StreamWriter(env->dns_zone_dir + aliasname);
				try {
					for each (String ^line in lines)
					{
						sw->WriteLine(line->Replace(domainname, aliasname));
					}
					sw->Close();
				}
				finally {
					delete sw;
				}
				sr->Close();
			}
			finally {
				delete sr;
			}
		}
		catch (Exception ^e) {
			logger->debugLog("domaliasmng: " + e->Message, 1617);
		}
	}

	void domaliasmng::removeFromDNS(String ^aliasname)
	{
		utils->removeLineFromFile(env->dns_zone_conf, "^zone.*\"" + aliasname + "\"");
		utils->removeFile(env->dns_zone_dir + aliasname);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	domaliasmng ^mainProcess = gcnew domaliasmng();
	mainProcess->Main(Args);
	return 0;
}