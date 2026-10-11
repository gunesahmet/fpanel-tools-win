#pragma once
#include "SystemInfo.h"
#include "Logger.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::DirectoryServices;
using namespace System::Globalization;
using namespace System::Security::Principal;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		public ref class Admin
		{
		private:
			String ^system_user;
			String ^system_group;
			String ^system_group_description;
			Environments::FileLogger ^loggers;

		public:
			void sysInfo()
			{
				SystemInfo ^sysinfo = gcnew SystemInfo();
				sysinfo->sysInfo();
				System::Environment::Exit(0);
				loggers = gcnew Environments::FileLogger();
			}

			String ^exec(String ^command, String ^parameters)
			{
				System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();
				pProcess->StartInfo->FileName = command;
				pProcess->StartInfo->Arguments = parameters;
				pProcess->StartInfo->UseShellExecute = false;
				pProcess->StartInfo->RedirectStandardOutput = true;
				pProcess->Start();
				String ^strOutput = pProcess->StandardOutput->ReadToEnd();
				pProcess->WaitForExit();
				return strOutput;
			}

			void createSystemUser()
			{
				try {
					if (this->checkUser(system_user) == false) {
						int combinedFlag = 0;
						// Password Can not change
						int ADS_UF_PASSWD_CANT_CHANGE = 0x000000040;

						// Password Never Expires
						int ADS_UF_DONT_EXPIRE_PASSWD = 0x00010000;
						combinedFlag = ADS_UF_DONT_EXPIRE_PASSWD | ADS_UF_PASSWD_CANT_CHANGE;

						DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
						DirectoryEntries ^entries = local->Children;

						DirectoryEntry ^user = entries->Add(system_user, "User");
						user->Properties["FullName"]->Add(system_user);
						user->Invoke("SetPassword", getPassword());
						user->Invoke("Put", gcnew array<Object^> {"userFlags", combinedFlag});
						user->CommitChanges();

						addToGroup(system_user, system_group);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Admin::createSystemUser. Error: " + e->Message, 1105);
				}
			}

			void createSystemGroups()
			{
				if (checkSystemGroup() == false) {
					try {
						DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
						DirectoryEntry ^newGroup = local->Children->Add(system_group, "group");
						newGroup->Invoke("Put", gcnew array<Object^> {"Description", system_group_description});
						newGroup->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Admin::createSystemGroups. Error: " + e->Message, 1105);
					}
				}
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
					Logger::errorLog("Error Admin::addToGroup. Error: " + e->Message, 1105);
				}
				return false;
			}

			String ^getPassword()
			{
				String ^text = System::IO::File::ReadAllText("C:\\fpanel\\etc\\panel.conf");
				array<String^> ^split = text->Split(gcnew array<Char> {' '});
				return "Za" + StringTool::Truncate(split[2], 5) + "9";
			}

		private:
			bool checkUser(String ^username)
			{
				bool bRet = false;

				try {
					NTAccount ^acct = gcnew NTAccount(username);
					SecurityIdentifier ^id = safe_cast<SecurityIdentifier^>(acct->Translate(SecurityIdentifier::typeid));
					bRet = id->IsAccountSid();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Admin::checkUser. Error: " + e->Message, 1105);
				}
				return bRet;
			}

			bool checkSystemGroup()
			{
				try {
					DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
					DirectoryEntry ^grp;
					grp = local->Children->Find(system_group, "group");
					if (grp != nullptr) {
						return true;
					}
					else {
						return false;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Admin::checkSystemGroup -> " + system_group + " Error: " +e->Message, 1105);
					return false;
				}
			}

		private:
			void InitializeInstanceFields()
			{
				system_user = "FpanelAppPool";
				system_group = "FpanelUsers";
				system_group_description = "FPanel Local User Group";
			}

		public:
			Admin()
			{
				InitializeInstanceFields();
			}
		};

	}
}