#pragma once
#include "Logger.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::DirectoryServices;

namespace FPanel
{
	namespace Windows
	{
		public ref class ApplicationPool
		{
		public:
			bool createApplicationPool(String ^name, String ^aspnet_ver, String ^cpu_limit, String ^username, String ^password, String ^appPoolMode, String ^aspNetCore)
			{
				bool status = false;
				Environments::FileLogger ^logger = gcnew Environments::FileLogger();
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^applicationPoolsSection = config->GetSection("system.applicationHost/applicationPools");

					ConfigurationElementCollection ^applicationPoolsCollection = applicationPoolsSection->GetCollection();
					ConfigurationElement ^addingElement = FindElement(applicationPoolsCollection, "add", "name", name);
					if (addingElement == nullptr) {
						ConfigurationElement ^addElement = applicationPoolsCollection->CreateElement("add");
						addElement["name"] = name;

						if (aspNetCore == "on") {
							addElement["enable32BitAppOnWin64"] = false;
							addElement["managedPipelineMode"] = "Integrated";
							addElement["managedRuntimeVersion"] = "";
						}
						else {
							addElement["enable32BitAppOnWin64"] = true;
							if (aspnet_ver == "v40") {
								addElement["managedRuntimeVersion"] = "v4.0";
							}
							else {
								addElement["managedRuntimeVersion"] = "v2.0";
							}
							addElement["managedPipelineMode"] = appPoolMode;
						}


						ConfigurationElement ^processModelElement = addElement->GetChildElement("processModel");
						processModelElement["identityType"] = "SpecificUser";
						processModelElement["userName"] = username;
						processModelElement["password"] = password;

						if (cpu_limit != "0") {
							ConfigurationElement ^failureElement = addElement->GetChildElement("failure");
							// failureElement["loadBalancerCapabilities"] = @"HttpLevel";
							ConfigurationElement ^cpuElement = addElement->GetChildElement("cpu");
							cpuElement["limit"] = Convert::ToInt32(cpu_limit);
							cpuElement["action"] = "KillW3wp";
							cpuElement["resetInterval"] = TimeSpan::Parse("00:05:00");
						}

						applicationPoolsCollection->Add(addElement);
						status = true;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("ApplicationPool::createApplicationPool Error: " + e->Message, 1005);
				}
				return status;
			}

			bool updateApplicationPool(String ^name, String ^appPoolMode, String ^aspnet_ver, String ^cpu_limit, String ^username, String ^password, String ^aspNetCore)
			{
				bool status = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^applicationPoolsSection = config->GetSection("system.applicationHost/applicationPools");
					ConfigurationElementCollection ^applicationPoolsCollection = applicationPoolsSection->GetCollection();

					ConfigurationElement ^addElement = FindElement(applicationPoolsCollection, "add", "name", name);
					if (addElement != nullptr) {

						if (aspNetCore == "on") {
							addElement["enable32BitAppOnWin64"] = false;
							addElement["managedPipelineMode"] = "integrated";
						}
						else {
							addElement["enable32BitAppOnWin64"] = true;
							if (aspnet_ver == "v40") {
								addElement["managedRuntimeVersion"] = "v4.0";
							}
							else {
								addElement["managedRuntimeVersion"] = "v2.0";
							}
							addElement["managedPipelineMode"] = appPoolMode;
						}


						ConfigurationElement ^processModelElement = addElement->GetChildElement("processModel");
						processModelElement["userName"] = username;
						processModelElement["password"] = password;

						if (cpu_limit != "0") {
							ConfigurationElement ^failureElement = addElement->GetChildElement("failure");
							// failureElement["loadBalancerCapabilities"] = @"HttpLevel";
							ConfigurationElement ^cpuElement = addElement->GetChildElement("cpu");
							cpuElement["limit"] = Convert::ToInt32(cpu_limit);
							cpuElement["action"] = "KillW3wp";
							cpuElement["resetInterval"] = TimeSpan::Parse("00:05:00");
						}
						else {
							ConfigurationElement ^failureElement = addElement->GetChildElement("failure");
							// failureElement["loadBalancerCapabilities"] = @"HttpLevel";
							ConfigurationElement ^cpuElement = addElement->GetChildElement("cpu");
							cpuElement["limit"] = nullptr;
						}
						status = true;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("ApplicationPool::updateApplicationPool Error: " + e->Message, 1006);
				}
				return status;
			}

			bool removeApplicationPool(String ^name)
			{
				bool status = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^applicationPoolsSection = config->GetSection("system.applicationHost/applicationPools");
					ConfigurationElementCollection ^applicationPoolsCollection = applicationPoolsSection->GetCollection();
					ConfigurationElement ^removeElement = FindElement(applicationPoolsCollection, "add", "name", name);
					if (removeElement != nullptr) {
						applicationPoolsCollection->Remove(removeElement);
					}
					serverManager->CommitChanges();
					status = true;
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error ApplicationPool::removeApplicationPool. Error: " + e->Message, 1105);
					status = false;
				}
				return status;
			}

			bool assignToSite(String ^siteName, String ^name)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);
					if (siteElement != nullptr) {
						ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

						ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
						if (applicationElement != nullptr) {
							applicationElement["applicationPool"] = name;
						}

						ConfigurationElement ^applicationDefaultsElement = siteElement->GetChildElement("applicationDefaults");
						applicationDefaultsElement["applicationPool"] = name;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error ApplicationPool::assignToSite. Error: " + e->Message, 1105);
					return false;
				}
				return true;
			}

			static String ^getApplicationPool(String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);

					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						return nullptr;

					String ^poolName = applicationElement["applicationPool"]->ToString();
					serverManager->CommitChanges();
					delete serverManager;
					return poolName;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error ApplicationPool::getApplicationPool. Error: " + e->Message, 1105);
					return {};
				}
			}

			bool checkPool(String ^name)
			{
				bool status = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^applicationPoolsSection = config->GetSection("system.applicationHost/applicationPools");
					ConfigurationElementCollection ^applicationPoolsCollection = applicationPoolsSection->GetCollection();
					ConfigurationElement ^checkElement = FindElement(applicationPoolsCollection, "add", "name", name);
					if (checkElement != nullptr) {
						status = true;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("ApplicationPool::checkPool Error: " + e->Message, 1005);
				}
				return status;
			}

		private:
			static String ^UppercaseFirst(String ^s)
			{
				if (String::IsNullOrEmpty(s)) {
					return String::Empty;
				}
				return Char::ToUpper(s[0]) + s->Substring(1);
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