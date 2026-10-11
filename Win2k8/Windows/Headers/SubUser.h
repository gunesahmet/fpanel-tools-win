#pragma once
#include "Logger.h"
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::DirectoryServices;
using namespace System::Security::Principal;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class SubUser
		{
		private:
			String ^system_user;
			String ^system_group;
			String ^default_vhosts;

		public:
			void create(String ^username, String ^password, String ^domain, String ^subdomain)
			{
				bool status = this->checkUser(username);

				if (status == false) {
					try {
						int combinedFlag = 0;
						// Password Can not change
						int ADS_UF_PASSWD_CANT_CHANGE = 0x000000040;

						// Password Never Expires
						int ADS_UF_DONT_EXPIRE_PASSWD = 0x00010000;
						combinedFlag = ADS_UF_DONT_EXPIRE_PASSWD | ADS_UF_PASSWD_CANT_CHANGE;

						DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
						DirectoryEntries ^entries = local->Children;

						DirectoryEntry ^user = entries->Add(username, "User");
						user->Properties["FullName"]->Add(username);
						user->Properties["HomeDirectory"]->Value = default_vhosts + "\\" + domain + "\\subdomains" + subdomain;
						user->Invoke("SetPassword", password);
						user->Invoke("Put", gcnew array<Object^> {"userFlags", combinedFlag});
						user->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create" + ". Error: " + e->Message, 1105);
					}
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":create", 1105);
				}
			}

			void update(String ^username, String ^newPassword, String ^newUsername)
			{
				bool status = this->checkUser(username);

				if (status == true) {
					try {
						DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
						DirectoryEntry ^user = local->Children->Find(username);
						if (newPassword != nullptr) {
							user->Invoke("SetPassword", newPassword);
						}

						if (newUsername != nullptr) {
							user->Rename(newUsername);
						}

						user->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":update" + ". Error: " + e->Message, 1105);
					}
				}
			}

			void remove(String ^username)
			{
				bool status = this->checkUser(username);

				if (status == true) {
					try {
						DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
						DirectoryEntries ^users = local->Children;
						DirectoryEntry ^user = users->Find(username);
						users->Remove(user);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":remove" + ". Error: " + e->Message, 1105);
					}
				}
			}

			void state(String ^username, String ^state)
			{

				bool status = this->checkUser(username);
				try {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntry ^user = local->Children->Find(username);
					user->InvokeSet("AccountDisabled", true);

					if (status == true) {
						if (state == "suspend") {
							user->InvokeSet("AccountDisabled", true);
						}
						else {
							user->InvokeSet("AccountDisabled", false);
						}
						user->CommitChanges();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":state" + ". Error: " + e->Message, 1105);
				}
			}

			bool checkUser(String ^username)
			{
				bool bRet = false;

				try {
					NTAccount ^acct = gcnew NTAccount(username);
					SecurityIdentifier ^id = safe_cast<SecurityIdentifier^>(acct->Translate(SecurityIdentifier::typeid));

					bRet = id->IsAccountSid();
				}
				catch (IdentityNotMappedException ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":checkUser" + ". Error: " + e->Message, 1105);
				}
				return bRet;
			}

		private:
			String ^getDomainApplicationPoolName(String ^domain_name)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain_name);

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
					Logger::errorLog("Error " + this->GetType()->Name + ":getDomainApplicationPoolName" + ". Error: " + e->Message, 1105);
				}
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


		private:
			void InitializeInstanceFields()
			{
				system_user = "FpanelAppPool";
				system_group = "FpanelUsers";
				default_vhosts = "c:\\vhosts";
			}

		public:
			SubUser()
			{
				InitializeInstanceFields();
			}
		};
	}
}