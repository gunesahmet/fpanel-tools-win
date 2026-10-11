#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;

namespace FPanel
{
	namespace Windows
	{
		private ref class AspNetDomain
		{
		private:
			static String ^domain;

		public:
			void save(String ^siteName, String ^defaultLanguage, String ^switchDebug,
				String ^culture, String ^fileEncoding, String ^requestEncoding,
				String ^responseEncoding, String ^uiCulture, String ^customErrorMode,
				String ^trustLevel, String ^sessionAuth, String ^sessionTimeout,
				String ^connectionString, String ^connectionStringName,
				String ^connectionStringProvider, String ^statusCode, String ^redirectUrl)
			{
				domain = siteName;
				this->compilation(defaultLanguage, switchDebug);
				this->globalization(culture, fileEncoding, requestEncoding, responseEncoding, uiCulture);
				this->customErrorSetting(customErrorMode);
				this->addCustomError(statusCode, redirectUrl);
				this->dotNetTrustLevel(trustLevel);
				this->sessionSettings(sessionAuth, sessionTimeout);
				this->addConnectionString(connectionString, connectionStringName, connectionStringProvider);
			}

			void compilation(String ^defaultLanguage, String ^switchDebug)
			{
				bool debug = false;
				if (switchDebug != nullptr) {
					if (switchDebug == "true") {
						debug = true;
					}
				}
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^compilationSection = config->GetSection("system.web/compilation", domain);
					if (defaultLanguage != nullptr) {
						compilationSection["defaultLanguage"] = defaultLanguage;
					}
					if (switchDebug != nullptr) {
						compilationSection["debug"] = debug;
					}

					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetDomain::compilation. Error: " + e->Message, 1105);
				}
			}

			void globalization(String ^culture, String ^fileEncoding, String ^requestEncoding, String ^responseEncoding, String ^uiCulture)
			{
				if (culture == nullptr) {
					culture = "";
				}
				if (uiCulture == nullptr) {
					uiCulture = "";
				}
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();

					ConfigurationSection ^globalizationSection = config->GetSection("system.web/globalization", domain);
					globalizationSection["culture"] = culture;
					globalizationSection["uiCulture"] = uiCulture;
					globalizationSection["fileEncoding"] = fileEncoding;
					globalizationSection["requestEncoding"] = requestEncoding;
					globalizationSection["responseEncoding"] = responseEncoding;
					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetDomain::globalization. Error: " + e->Message, 1105);
				}
			}

			void customErrorSetting(String ^customErrorMode)
			{
				if (customErrorMode != nullptr) {
					try {
						String ^errorMode;
						if (customErrorMode == "On") {
							errorMode = "On";
						}
						else if (customErrorMode == "RemoteOnly") {
							errorMode = "RemoteOnly";
						}
						else if (customErrorMode == "Off") {
							errorMode = "Off";
						}
						else {
							errorMode = "Off";
						}
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^customErrorsSection = config->GetSection("system.web/customErrors", domain);
						customErrorsSection["mode"] = errorMode;
						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error AspNetDomain::customErrorSetting. Error: " + e->Message, 1105);
					}
				}
			}

			void addCustomError(String ^statusCode, String ^redirectUrl)
			{
				if ((statusCode != nullptr) && ((redirectUrl != nullptr))) {
					try {
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);

						ConfigurationSection ^customErrorsSection = config->GetSection("system.web/customErrors", domain);
						ConfigurationElementCollection ^customErrorsCollection = customErrorsSection->GetCollection();

						ConfigurationElement ^errorElement = customErrorsCollection->CreateElement("error");
						errorElement["redirect"] = redirectUrl;
						errorElement["statusCode"] = Convert::ToInt32(statusCode);
						customErrorsCollection->Add(errorElement);
						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error AspNetDomain::addCustomError. Error: " + e->Message, 1105);
					}
				}

			}

			void removeCustomError(String ^siteName, String ^statusCode)
			{
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
					ConfigurationSection ^customErrorsSection = config->GetSection("system.web/customErrors", siteName);

					ConfigurationElementCollection ^customErrorsCollection = customErrorsSection->GetCollection();

					ConfigurationElement ^errorElement = FindElement(customErrorsCollection, "error", "statusCode", statusCode);
					if (errorElement == nullptr)
						System::Environment::Exit(1);

					customErrorsCollection->Remove(errorElement);

					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetDomain::removeCustomError. Error: " + e->Message, 1105);
				}
			}

			void dotNetTrustLevel(String ^trustLevel)
			{
				try {
					if (trustLevel != nullptr) {
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^trustSection = config->GetSection("system.web/trust", domain);
						trustSection["level"] = trustLevel;
						serverMgr->CommitChanges();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error AspNetDomain::dotNetTrustLevel. Error: " + e->Message, 1105);
				}
			}

			void sessionSettings(String ^sessionAuth, String ^sessionTimeout)
			{
				if ((sessionAuth != nullptr) && (sessionTimeout != nullptr)) {
					if (sessionAuth == "0") {
						sessionAuth = "None";
					}
					else if (sessionAuth == "1") {
						sessionAuth = "Windows";
					}
					else if (sessionAuth == "2") {
						sessionAuth = "Passport";
					}
					else if (sessionAuth == "3") {
						sessionAuth = "Forms";
					}
					try {
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^authenticationSection = config->GetSection("system.web/authentication", domain);
						authenticationSection["mode"] = sessionAuth;
						Microsoft::Web::Administration::Configuration ^configSession = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^sessionStateSection = configSession->GetSection("system.web/sessionState", domain);
						sessionStateSection["timeout"] = TimeSpan::Parse(sessionTimeout);

						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error AspNetDomain::sessionSettings. Error: " + e->Message, 1105);
					}
				}
			}

			void addConnectionString(String ^connectionString, String ^connectionStringName, String ^connectionStringProvider)
			{
				if ((connectionStringName != nullptr) && (connectionString != nullptr)) {
					try {
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^connectionStringsSection = config->GetSection("connectionStrings", domain);
						ConfigurationElementCollection ^connectionStringsCollection = connectionStringsSection->GetCollection();
						ConfigurationElement ^addElement = connectionStringsCollection->CreateElement("add");

						addElement["connectionString"] = connectionString;
						addElement["name"] = connectionStringName;
						addElement["providerName"] = connectionStringProvider;
						connectionStringsCollection->Add(addElement);
						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error AspNetDomain::addConnectionString. Error: " + e->Message, 1105);
					}
				}
			}

			void removeConnectionString(String ^siteName, String ^connectionStringName)
			{
				if (connectionStringName != nullptr) {
					try {
						ServerManager ^serverMgr = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetWebConfiguration(nullptr);
						ConfigurationSection ^connectionStringsSection = config->GetSection("connectionStrings", siteName);
						ConfigurationElementCollection ^connectionStringsCollection = connectionStringsSection->GetCollection();

						ConfigurationElement ^addElement = FindElement(connectionStringsCollection, "add", "name", connectionStringName);
						if (addElement == nullptr)
							System::Environment::Exit(2);

						connectionStringsCollection->Remove(addElement);
						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error AspNetDomain::removeConnectionString. Error: " + e->Message, 1105);
					}
				}
			}

		private:
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

		private:
			void InitializeInstanceFields()
			{
			}

		public:
			AspNetDomain()
			{
				InitializeInstanceFields();
			}
		};
	}
}