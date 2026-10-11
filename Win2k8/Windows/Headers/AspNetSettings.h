#pragma once
#include "Logger.h"
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class AspNetSettings
		{
		public:
			void aspnetSettings(String ^siteName, String ^parameters)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.web/compilation");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);
					if (siteElement == nullptr)
						System::Environment::Exit(7102);

					ConfigurationElement ^ftpServerElement = siteElement->GetChildElement("ftpServer");

					ConfigurationElement ^securityElement = ftpServerElement->GetChildElement("security");

					ConfigurationElement ^sslElement = securityElement->GetChildElement("ssl");
					sslElement["controlChannelPolicy"] = "SslAllow";
					sslElement["dataChannelPolicy"] = "SslAllow";

					ConfigurationElement ^authenticationElement = securityElement->GetChildElement("authentication");

					ConfigurationElement ^anonymousAuthenticationElement = authenticationElement->GetChildElement("anonymousAuthentication");
					anonymousAuthenticationElement["enabled"] = false;

					ConfigurationElement ^basicAuthenticationElement = authenticationElement->GetChildElement("basicAuthentication");
					basicAuthenticationElement["enabled"] = true;

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetSettings::aspnetSettings. Error: " + e->Message, 1105);
				}
			}

		private:
			static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
			{
				try {
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
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetSettings::Find. Error: " + e->Message, 1105);
				}
				return nullptr;

			}

		};
	}
}