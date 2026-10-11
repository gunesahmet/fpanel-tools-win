#pragma once
#include "Logger.h"
#include "Security.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::DirectoryServices;
using namespace System::Security::Principal;
using namespace System::Globalization;
using namespace LSA;

namespace FPanel
{
	namespace Windows
	{
		private ref class User
		{
		private:
			String ^system_user;
			String ^system_group;

		public:
			bool createUser(String ^username, String ^password, String ^path, String ^domain, String ^subdomain, String ^appPool)
			{
				if (subdomain == nullptr) {
					int combinedFlag = 0;
					// Password Can not change
					int ADS_UF_PASSWD_CANT_CHANGE = 0x000000040;

					// Password Never Expires
					int ADS_UF_DONT_EXPIRE_PASSWD = 0x00010000;
					combinedFlag = ADS_UF_DONT_EXPIRE_PASSWD | ADS_UF_PASSWD_CANT_CHANGE;

					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntries ^entries = local->Children;

					DirectoryEntry ^user_renamed = entries->Add(username, "User");

					user_renamed->Properties["FullName"]->Add(username);
					user_renamed->Properties["HomeDirectory"]->Value = path;
					user_renamed->Invoke("SetPassword", password);
					user_renamed->Invoke("Put", gcnew array<Object^> {"userFlags", combinedFlag});
					user_renamed->CommitChanges();
					if (appPool == "on") {
						createGroup("IIS_" + username);
						addToGroup(username, "IIS_" + username);
					}
				}
				else {
					int combinedFlag = 0;
					// Password Can not change
					int ADS_UF_PASSWD_CANT_CHANGE = 0x000000040;

					// Password Never Expires
					int ADS_UF_DONT_EXPIRE_PASSWD = 0x00010000;
					combinedFlag = ADS_UF_DONT_EXPIRE_PASSWD | ADS_UF_PASSWD_CANT_CHANGE;

					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntries ^entries = local->Children;

					DirectoryEntry ^user_renamed = entries->Add(username, "User");
					user_renamed->Properties["FullName"]->Add(username);
					user_renamed->Properties["HomeDirectory"]->Value = path;
					user_renamed->Invoke("SetPassword", password);
					user_renamed->Invoke("Put", gcnew array<Object^> {"userFlags", combinedFlag});
					user_renamed->CommitChanges();

					if (appPool == "on") {
						createGroup("IIS_" + username);
						addToGroup(username, "IIS_" + username);
					}
				}
				addToGroup(username, "fpusers");
				LSAWrapper::SetRight(username, LocalSecurityAuthorityRights::LogonAsBatchJob);
				return true;
			}

			void updateUser(String ^username, String ^password, String ^new_username, String ^subdomain, String ^app_pool)
			{
				bool status = this->checkUser(username);

				if (status == true) {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");

					DirectoryEntry ^user_renamed = local->Children->Find(username);
					if ((password != nullptr) && (!String::IsNullOrEmpty(password))) {
						user_renamed->Invoke("SetPassword", password);
					}

					if ((new_username != nullptr) && (!String::IsNullOrEmpty(new_username))) {
						user_renamed->Rename(new_username);
					}

					user_renamed->CommitChanges();

					if ((new_username != nullptr) && (!String::IsNullOrEmpty(new_username))) {
						if (app_pool == "on") {
							this->renameGroup("IIS_" + username, "IIS_" + new_username);
							this->addToGroup(new_username, "IIS_" + new_username);
						}
						else {
							if (isGroupExists("IIS_" + username)) {
								this->removeGroup("IIS_" + username);
							}
						}
					}
					else {
						if (app_pool != "on") {
							if (isGroupExists("IIS_" + username)) {
								this->removeGroup("IIS_" + username);
							}
						}
					}
				}
			}

			void removeUser(String ^username)
			{
				bool status = this->checkUser(username);

				if (status == true) {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntries ^users = local->Children;

					DirectoryEntry ^user_renamed = users->Find(username);
					users->Remove(user_renamed);
					try {
						this->removeGroup("IIS_" + username);
					}
					catch (...) {
					}
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":removeUser" + ". Error: User doesnt exists", 1105);
				}
			}

			void stateUser(String ^username, String ^state)
			{
				bool status = this->checkUser(username);

				if (status == true) {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntry ^user_renamed = local->Children->Find(username);
					user_renamed->InvokeSet("AccountDisabled", true);

					if (state == "suspend") {
						user_renamed->InvokeSet("AccountDisabled", true);
					}
					else {
						user_renamed->InvokeSet("AccountDisabled", false);
					}

					user_renamed->CommitChanges();
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":stateUser" + ". Error: User doesnt exists", 1105);
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
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":checkUser" + ". Error: " + e->Message, 1105);
				}
				return bRet;
			}

			bool addToGroup(String ^username, String ^groupname)
			{
				try {
					DirectoryEntry ^userGroup = nullptr;
					String ^groupPath = String::Format(CultureInfo::CurrentUICulture, "WinNT://{0}/{1},group", "localhost", groupname);
					userGroup = gcnew DirectoryEntry(groupPath);
					if ((nullptr == userGroup) || (true == String::IsNullOrEmpty(userGroup->SchemaClassName)) || (0 != String::Compare(userGroup->SchemaClassName, "group", true, CultureInfo::CurrentUICulture)))
						return false;

					String ^userPath = String::Format(CultureInfo::CurrentUICulture, "WinNT://{0},user", username);
					userGroup->Invoke("Add", gcnew array<Object^> {userPath});
					userGroup->CommitChanges();
					return true;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":addToGroup" + ". Error: " + e->Message, 1105);
					return false;
				}
			}

			void renameGroup(String ^groupname, String ^new_groupname)
			{
				try {
					DirectoryEntry ^de = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntry ^groupEntry = nullptr;
					groupEntry = de->Children->Find(groupname, "group");
					if (groupEntry != nullptr) {
						groupEntry->Rename(new_groupname);
						//groupEntry.Properties["samAccountName"].Value = new_groupname;
						groupEntry->CommitChanges();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":renameGroup" + ". Error: " + e->Message, 1105);
				}

				//DirectoryEntry objGroup;  // Group object.
				//string strPath;           // Binding path.
				//strPath = @"WinNT://localhost";
				//string strGroup = groupname;

				//DirectoryEntry objADAM = new DirectoryEntry(strPath);
				//objADAM.RefreshCache();

				//objGroup = objADAM.Children.Find(strGroup, "group");
				//objADAM.Rename(new_groupname);
				//objADAM.CommitChanges();
			}

			void createGroup(String ^groupname)
			{
				try {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntry ^newGroup = local->Children->Add(groupname, "group");
					newGroup->Invoke("Put", gcnew array<Object^> {"Description", "IIS User for domain"});
					newGroup->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":createGroup" + ". Error: " + e->Message, 1105);
				}
			}

			void removeUserFromGroup(String ^username, String ^groupname)
			{
				try {
					DirectoryEntry ^objLocalAdmins = gcnew DirectoryEntry("WinNT://localhost/" + groupname + ",group");
					Object ^oUserPath = "WinNT://localhost/" + username;

					objLocalAdmins->Properties["member"]->Remove(oUserPath);

					objLocalAdmins->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":removeUserFromGroup" + ". Error: " + e->Message, 1105);
				}
			}

			void removeGroup(String ^groupname)
			{
				try {
					DirectoryEntry ^objGroup; // Group object.
					String ^strPath; // Binding path.
					strPath = "WinNT://localhost";
					String ^strGroup = groupname;

					DirectoryEntry ^objADAM = gcnew DirectoryEntry(strPath);
					objADAM->RefreshCache();

					objGroup = objADAM->Children->Find(strGroup, "group");
					objADAM->Children->Remove(objGroup);
					objADAM->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":removeGroup" + ". Error: " + e->Message + " GroupName: " + groupname, 1105);
				}
			}

		private:
			bool checkSystemGroup()
			{
				try {
					DirectoryEntry ^dirEntryLocalMachine = gcnew DirectoryEntry("WinNT://localhost,computer");
					return dirEntryLocalMachine->Children->Find(system_group, "group") != nullptr;
				}
				catch (...) {
					return false;
				}
			}

			bool isGroupExists(String ^group_name) {
			/*	System::DirectoryServices::AccountManagement::PrincipalContext ^local = gcnew System::DirectoryServices::AccountManagement::PrincipalContext(System::DirectoryServices::AccountManagement::ContextType::Machine);
				try {
					System::DirectoryServices::AccountManagement::Principal ^group = System::DirectoryServices::AccountManagement::GroupPrincipal::FindByIdentity(local, group_name);
					bool groupExists = group != nullptr;
					return groupExists;
				}
				finally {
					delete local;
				}*/
				try {
					DirectoryEntry ^dirEntryLocalMachine = gcnew DirectoryEntry("WinNT://localhost,computer");
					return dirEntryLocalMachine->Children->Find(group_name, "group") != nullptr;
				}
				catch (...) {
					return false;
				}
			}

		private:
			void InitializeInstanceFields()
			{
				system_user = "FpanelAppPool";
				system_group = "FpanelUsers";
			}

		public:
			User()
			{
				InitializeInstanceFields();
			}
		};
	}
}