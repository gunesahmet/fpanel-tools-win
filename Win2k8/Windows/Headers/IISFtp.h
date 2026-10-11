#pragma once
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class IISFtp
		{
		public:
			void createDefaultFTPSite()
			{
				if (checkFtpSite() == false) {
					try {
						ServerManager ^serverManager = gcnew ServerManager();
						Site ^site = serverManager->Sites->Add("Default FTP Site", "ftp", "*:21:" + "", "c:\\inetpub\\ftproot");
						site->ApplicationDefaults->EnabledProtocols = "ftp";

						// Log file initializing 
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
						ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

						// Ayarlar kaydediliyor
						serverManager->CommitChanges();
						delete serverManager;
					}
					catch (Exception ^e) {
						Logger::errorLog("Error IISFtp::createDefaultFTPSite. Error: " + e->Message, 1105);
					}
					this->ftpSettings();
					this->DefaultFtpSiteSettings();
				}
			}

			void ftpSettings()
			{
				if (checkFtpSite() == true) {
					try {
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

						ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

						ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

						ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default FTP Site");
						if (siteElement == nullptr)
							throw gcnew InvalidOperationException("Element not found!");


						ConfigurationElement ^ftpServerElement = siteElement->GetChildElement("ftpServer");

						ConfigurationElement ^securityElement = ftpServerElement->GetChildElement("security");

						ConfigurationElement ^sslElement = securityElement->GetChildElement("ssl");
						sslElement["controlChannelPolicy"] = "SslAllow";
						sslElement["dataChannelPolicy"] = "SslAllow";

						ConfigurationElement ^authenticationElement = securityElement->GetChildElement("authentication");

						ConfigurationElement ^basicAuthenticationElement = authenticationElement->GetChildElement("basicAuthentication");
						basicAuthenticationElement["enabled"] = true;

						ConfigurationElement ^userIsolationElement = ftpServerElement->GetChildElement("userIsolation");
						userIsolationElement["mode"] = "IsolateRootDirectoryOnly";

						serverManager->CommitChanges();
						delete serverManager;
					}
					catch (Exception ^e) {
						Logger::errorLog("Error IISFtp::ftpSettings. Error: " + e->Message, 1105);
					}
				}

			}

			void DefaultFtpSiteSettings()
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^authorizationSection = config->GetSection("system.ftpServer/security/authorization", "Default FTP Site");

					ConfigurationElementCollection ^authorizationCollections = authorizationSection->GetCollection();

					ConfigurationElement ^foundedAuthorizationCollection;
					bool found = false;
					for each (ConfigurationElement ^authorizationCollection in authorizationCollections) {
						if (String::Equals(authorizationCollection->GetAttributeValue("users")->ToString(), "*", StringComparison::OrdinalIgnoreCase)) {
							found = true;
							foundedAuthorizationCollection = authorizationCollection;
							break;
						}
					}
					if (found) {
						foundedAuthorizationCollection->SetAttributeValue("accessType", "Allow");
						foundedAuthorizationCollection->SetAttributeValue("permissions", "Read, Write");
					}
					else {
						ConfigurationElement ^addElement = authorizationCollections->CreateElement("add");
						addElement["accessType"] = "Allow";
						addElement["users"] = "*";
						addElement["permissions"] = "Read, Write";
						authorizationCollections->Add(addElement);
					}

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISFtp::DefaultFtpSiteSettings. Error: " + e->Message, 1105);
				}
			}

		private:
			static bool checkFtpSite()
			{
				bool status = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default FTP Site");
					if (siteElement == nullptr)
						status = false;
					else
						status = true;
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISFtp::checkFtpSite. Error: " + e->Message, 1105);
				}
				return status;
			}

			static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
			{
				for each (ConfigurationElement ^element in collection)
				{
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
		};
	}
}