#pragma once
#include "Logger.h"
#include "../../env/Utils.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Security::AccessControl;
using namespace System::Security::Principal;
using namespace System::Text;
using namespace Environments;

namespace FPanel
{
	namespace Windows
	{
		private ref class FileManager
		{
		public:
			void getPermissions(String ^file)
			{
				List<String^> ^fileACL = gcnew List<String^>();
				try {
					if (isDir(file) == false) {
						FileSecurity ^fileSecurity = gcnew FileSecurity(file, AccessControlSections::Access);
						AuthorizationRuleCollection ^arc = fileSecurity->GetAccessRules(true, true, NTAccount::typeid);
						for each (FileSystemAccessRule ^rule in arc) {
							if (String::Compare(rule->IdentityReference->ToString(), String("BUILTIN\\Users").ToLower()) == 0) {
								continue;
							}
							if (String::Compare(rule->IdentityReference->ToString()->ToLower(), String("BUILTIN\\IIS_IUSRS").ToLower()) == 0) {
								continue;
							}
							if (rule->IdentityReference->ToString()->ToLower()->Contains("fpusers")) {
								continue;
							}
							if (rule->IdentityReference->ToString() != "CREATOR OWNER") {
								if (rule->AccessControlType.ToString() == "268435456") {
									fileACL->Add(rule->IdentityReference->ToString() + ":" + rule->AccessControlType.ToString() + " ");
								}
								else {
									fileACL->Add(rule->IdentityReference->ToString() + ":" + rule->AccessControlType.ToString() + " " + rule->FileSystemRights.ToString());
								}
							}
							//fileACL->Add(rule->IdentityReference->ToString() + ":" + rule->AccessControlType.ToString() + " " + rule->FileSystemRights.ToString());
						}
					}
					else {
						DirectorySecurity ^directorySecurity = gcnew DirectorySecurity(file, AccessControlSections::Access);
						AuthorizationRuleCollection ^arc = directorySecurity->GetAccessRules(true, true, NTAccount::typeid);
						for each (FileSystemAccessRule ^rule in arc) {
							if (String::Compare(rule->IdentityReference->ToString(), String("BUILTIN\\Users").ToLower()) == 0) {
								continue;
							}
							if (String::Compare(rule->IdentityReference->ToString()->ToLower(), String("BUILTIN\\IIS_IUSRS").ToLower()) == 0) {
								continue;
							}
							if (rule->IdentityReference->ToString()->ToLower()->Contains("fpusers")) {
								continue;
							}
							if (rule->IdentityReference->ToString() != "CREATOR OWNER") {
								if (rule->AccessControlType.ToString() == "268435456") {
									fileACL->Add(rule->IdentityReference->ToString() + ":" + rule->AccessControlType.ToString() + " ");
								}
								else {
									fileACL->Add(rule->IdentityReference->ToString() + ":" + rule->AccessControlType.ToString() + " " + rule->FileSystemRights.ToString());
								}
							}
						}
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FileManager::getPermissions. Error: " + e->Message, 1105);
				}

				for each (String ^item in fileACL) {
					Console::WriteLine(item);
				}
			}

			void setPermissions(String ^file, String ^user, Dictionary<String^, String^> ^permissions)
			{
				ClearDirectorySecurity(file, user);
				RemoveFull(file, user);
				RemoveModify(file, user);
				RemoveWrite(file, user);
				RemoveRead(file, user);
				RemoveListContent(file, user);
				for each (KeyValuePair<String^, String^> ^item in permissions) {
					this->setPermission(user, file, item->Key, item->Value);
				}
			}

			void setPermission(String ^username, String ^file, String ^permission, String ^type)
			{
				if (permission == "FullControl") {
					if (type->Equals("deny")) {
						AddDirectorySecurity(file, username, FileSystemRights::FullControl, AccessControlType::Deny);
					}
					else if (type->Equals("allow")) {
						AddDirectorySecurity(file, username, FileSystemRights::FullControl, AccessControlType::Allow);
					}
				}
				else {
					if (permission == "Modify") {
						if (type->Equals("deny")) {
							Modify(file, username, AccessControlType::Deny);
						}
						else if (type->Equals("allow")) {
							Modify(file, username, AccessControlType::Allow);
						}
					}
					else {
						if (permission == "ReadAndExecute") {
							if (type->Equals("deny")) {
								ReadAndExecute(file, username, AccessControlType::Deny);
							}
							else if (type->Equals("allow")) {
								ReadAndExecute(file, username, AccessControlType::Allow);
							}
						}
						else {
							if (permission == "Read") {
								if (type->Equals("deny")) {
									Read(file, username, AccessControlType::Deny);
								}
								else if (type->Equals("allow")) {
									Read(file, username, AccessControlType::Allow);
								}
								else {
									//RemoveRead(file, username);
								}
							}

							if (permission == "ListFolderContents") {
								if (type->Equals("deny")) {
									ListContents(file, username, AccessControlType::Deny);
								}
								else if (type->Equals("allow")) {
									ListContents(file, username, AccessControlType::Allow);
								}
								else {
									//RemoveListContent(file, username);
								}
							}
						}


						if (permission->Equals("Write")) {
							if (type->Equals("deny")) {
								Write(file, username, AccessControlType::Deny);
							}
							else if (type->Equals("allow")) {
								Write(file, username, AccessControlType::Allow);
							}
							else {
								//RemoveWrite(file, username);
							}
						}
					}
				}
			}

			void AddDirectorySecurity(String ^file, String ^Account, FileSystemRights Rights, AccessControlType ControlType)
			{
				try {
					if (isDir(file) == true) {
						DirectoryInfo ^dInfo = gcnew DirectoryInfo(file);
						DirectorySecurity ^dSecurity = dInfo->GetAccessControl();
						dSecurity->AddAccessRule(gcnew FileSystemAccessRule(Account, Rights, InheritanceFlags::ContainerInherit | InheritanceFlags::ObjectInherit, PropagationFlags::None, ControlType));
						dInfo->SetAccessControl(dSecurity);
						Directory::SetAccessControl(file, dSecurity);
					}
					else {
						FileInfo ^dInfo = gcnew FileInfo(file);
						FileSecurity ^fSecurity = dInfo->GetAccessControl();
						fSecurity->AddAccessRule(gcnew FileSystemAccessRule(Account, Rights, ControlType));
						dInfo->SetAccessControl(fSecurity);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FileManager::AddDirectorySecurity. Error: " + e->Message, 1105);
				}
			}

			void RemoveDirectorySecurity(String ^file, String ^Account, FileSystemRights Rights, AccessControlType ControlType)
			{
				try {
					if (isDir(file) == true) {
						DirectoryInfo ^dInfo = gcnew DirectoryInfo(file);
						DirectorySecurity ^dSecurity = dInfo->GetAccessControl();

						if (Rights == FileSystemRights::FullControl) {
							dSecurity->RemoveAccessRule(gcnew FileSystemAccessRule(Account, FileSystemRights::FullControl, InheritanceFlags::ContainerInherit | InheritanceFlags::ObjectInherit, PropagationFlags::None, ControlType));
						}
						else {
							dSecurity->RemoveAccessRule(gcnew FileSystemAccessRule(Account, Rights, ControlType));
						}
						dInfo->SetAccessControl(dSecurity);
					}
					else {
						FileInfo ^fInfo = gcnew FileInfo(file);
						FileSecurity ^fSecurity = fInfo->GetAccessControl();
						if (Rights == FileSystemRights::FullControl) {
							fSecurity->RemoveAccessRule(gcnew FileSystemAccessRule(Account, FileSystemRights::FullControl, InheritanceFlags::ContainerInherit | InheritanceFlags::ObjectInherit, PropagationFlags::None, ControlType));
						}
						else {
							fSecurity->RemoveAccessRule(gcnew FileSystemAccessRule(Account, Rights, ControlType));
						}
						fInfo->SetAccessControl(fSecurity);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FileManager::RemoveDirectorySecurity. Error: " + e->Message, 1105);
				}
			}

			void ClearDirectorySecurity(String ^file, String ^username)
			{
				//try {
					String ^hostname = System::Net::Dns::GetHostName();
					String ^host_username = hostname + "\\" + username;

					if (isDir(file) == true) {
						DirectoryInfo ^dInfo = gcnew DirectoryInfo(file);
						DirectorySecurity ^dSecurity = dInfo->GetAccessControl();
						dSecurity->SetAccessRuleProtection(true, false);
						dInfo->SetAccessControl(dSecurity);
					}
					else {
						FileInfo ^fInfo = gcnew FileInfo(file);
						FileSecurity ^fSecurity = fInfo->GetAccessControl();
						fSecurity->SetAccessRuleProtection(true, false);
						fInfo->SetAccessControl(fSecurity);
					}

					// User List "SYSTEM", hostname + "\\FpanelAppPool", host_username, hostname + "\\fpusers", hostname + "\\IIS_IUSRS", hostname + "\\Administrators"
					//            FULL     READ+EXEC+WRITE               FULL           READ+WRITE => DENY            READ+EXEC+WRITE         FULL         

					// hostname + "\\Administrators"
					AddDirectorySecurity(file, "Administrators", FileSystemRights::FullControl, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::CreateFiles, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::WriteData, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::CreateDirectories, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::AppendData, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::WriteAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "Administrators", FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);

					// For SYSTEM
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::FullControl, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::CreateFiles, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::WriteData, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::CreateDirectories, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::AppendData, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::WriteAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "SYSTEM", FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);

					//hostname + "\\FpanelAppPool"
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ExecuteFile, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::Traverse, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ListDirectory, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ReadData, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ReadAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ReadExtendedAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::ReadPermissions, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::CreateFiles, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::WriteData, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::CreateDirectories, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::AppendData, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::WriteAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "FpanelAppPool", FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);
					// hostname + "\\IIS_IUSRS"
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ExecuteFile, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::Traverse, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ListDirectory, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ReadData, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ReadAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ReadExtendedAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::ReadPermissions, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::CreateFiles, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::WriteData, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::CreateDirectories, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::AppendData, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::WriteAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, "IIS_IUSRS", FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);
					// hostname + "\\fpusers"
		/*			ListContents(file, "fpusers", AccessControlType::Allow);
					ReadAndExecute(file, "fpusers", AccessControlType::Allow);
					Write(file, "fpusers", AccessControlType::Allow);*/

					// username
					AddDirectorySecurity(file, username, FileSystemRights::FullControl, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::CreateFiles, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::WriteData, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::CreateDirectories, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::AppendData, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::WriteAttributes, AccessControlType::Allow);
					AddDirectorySecurity(file, username, FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);

			/*	}
				catch (Exception ^e) {
					Logger::errorLog("Error FileManager::ClearDirectorySecurity. Error: " + e->Message, 1105);
				}*/
			}

			void Modify(String ^file, String ^username, AccessControlType ControlType)
			{
				AddDirectorySecurity(file, username, FileSystemRights::Modify, ControlType);
			}

			void ReadAndExecute(String ^file, String ^username, AccessControlType ControlType)
			{
				AddDirectorySecurity(file, username, FileSystemRights::ExecuteFile, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::Traverse, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ListDirectory, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadData, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadPermissions, ControlType);
			}

			void Read(String ^file, String ^username, AccessControlType ControlType)
			{
				AddDirectorySecurity(file, username, FileSystemRights::ListDirectory, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadData, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadPermissions, ControlType);
			}

			void ListContents(String ^file, String ^username, AccessControlType ControlType)
			{
				AddDirectorySecurity(file, username, FileSystemRights::ListDirectory, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadData, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ReadPermissions, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::Traverse, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::ExecuteFile, ControlType);

				// Traverse ve execute List folderdan gelebilir kaldirilmamasi gerekiyor

				//RemoveDirectorySecurity(file, username, FileSystemRights.DeleteSubdirectoriesAndFiles, AccessControlType.Allow);
				//RemoveDirectorySecurity(file, username, FileSystemRights.Delete, AccessControlType.Allow);
				//RemoveDirectorySecurity(file, username, FileSystemRights.ChangePermissions, AccessControlType.Allow);
				//RemoveDirectorySecurity(file, username, FileSystemRights.TakeOwnership, AccessControlType.Allow);
				//RemoveDirectorySecurity(file, username, FileSystemRights.FullControl, AccessControlType.Allow);

				//RemoveDirectorySecurity(file, username, FileSystemRights.DeleteSubdirectoriesAndFiles, AccessControlType.Deny);
				//RemoveDirectorySecurity(file, username, FileSystemRights.Delete, AccessControlType.Deny);
				//RemoveDirectorySecurity(file, username, FileSystemRights.ChangePermissions, AccessControlType.Deny);
				//RemoveDirectorySecurity(file, username, FileSystemRights.TakeOwnership, AccessControlType.Deny);
				//RemoveDirectorySecurity(file, username, FileSystemRights.FullControl, AccessControlType.Deny);
			}

			void Write(String ^file, String ^username, AccessControlType ControlType)
			{
				AddDirectorySecurity(file, username, FileSystemRights::CreateFiles, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::WriteData, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::CreateDirectories, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::AppendData, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::WriteAttributes, ControlType);
				AddDirectorySecurity(file, username, FileSystemRights::WriteExtendedAttributes, ControlType);
			}

			void RemoveRead(String ^file, String ^username)
			{
				RemoveDirectorySecurity(file, username, FileSystemRights::ListDirectory, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadData, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadAttributes, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadPermissions, AccessControlType::Deny);

				RemoveDirectorySecurity(file, username, FileSystemRights::ListDirectory, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadData, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadAttributes, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadPermissions, AccessControlType::Allow);
			}

			void RemoveListContent(String ^file, String ^username)
			{
				RemoveDirectorySecurity(file, username, FileSystemRights::ListDirectory, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadData, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadAttributes, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadPermissions, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::Traverse, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::ExecuteFile, AccessControlType::Deny);

				RemoveDirectorySecurity(file, username, FileSystemRights::ListDirectory, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadData, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadAttributes, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadExtendedAttributes, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ReadPermissions, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::Traverse, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::ExecuteFile, AccessControlType::Allow);
			}

			void RemoveWrite(String ^file, String ^username)
			{
				RemoveDirectorySecurity(file, username, FileSystemRights::CreateFiles, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteData, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::CreateDirectories, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::AppendData, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteAttributes, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteExtendedAttributes, AccessControlType::Deny);

				RemoveDirectorySecurity(file, username, FileSystemRights::CreateFiles, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteData, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::CreateDirectories, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::AppendData, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteAttributes, AccessControlType::Allow);
				RemoveDirectorySecurity(file, username, FileSystemRights::WriteExtendedAttributes, AccessControlType::Allow);
			}

			void RemoveFull(String ^file, String ^username)
			{
				RemoveDirectorySecurity(file, username, FileSystemRights::FullControl, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::FullControl, AccessControlType::Allow);
			}

			void RemoveModify(String ^file, String ^username)
			{
				RemoveDirectorySecurity(file, username, FileSystemRights::Modify, AccessControlType::Deny);
				RemoveDirectorySecurity(file, username, FileSystemRights::Modify, AccessControlType::Allow);
			}

			static bool isDir(String ^path)
			{
				FileAttributes attr = File::GetAttributes(path);
				if ((attr & FileAttributes::Directory) == FileAttributes::Directory)
					return true; // Directory
				else
					return false; // File
			}

			static FileSystemRights getRight(String ^f_right)
			{
				FileSystemRights fright = FileSystemRights::ReadAndExecute;
				if (f_right == "FullControl")
					fright = FileSystemRights::FullControl;
				if (f_right == "Modify")
					fright = FileSystemRights::Modify;
				else if (f_right == "ReadAndExecute")
					fright = FileSystemRights::ReadAndExecute;
				else if (f_right == "Read")
					fright = FileSystemRights::Read;
				else if (f_right == "Write")
					fright = FileSystemRights::Write;
				else if (f_right == "Traverse")
					fright = FileSystemRights::Traverse;
				else if (f_right == "ListDirectory")
					fright = FileSystemRights::ListDirectory;
				else if (f_right == "ReadData")
					fright = FileSystemRights::ReadData;
				else if (f_right == "ExecuteFile")
					fright = FileSystemRights::ExecuteFile;
				else if (f_right == "ReadAttributes")
					fright = FileSystemRights::ReadAttributes;
				else if (f_right == "ReadExtendedAttributes")
					fright = FileSystemRights::ReadExtendedAttributes;
				else if (f_right == "CreateFiles")
					fright = FileSystemRights::CreateFiles;
				else if (f_right == "WriteData")
					fright = FileSystemRights::WriteData;
				else if (f_right == "CreateDirectories")
					fright = FileSystemRights::CreateDirectories;
				else if (f_right == "AppendData")
					fright = FileSystemRights::AppendData;
				else if (f_right == "WriteAttributes")
					fright = FileSystemRights::WriteAttributes;
				else if (f_right == "WriteExtendedAttributes")
					fright = FileSystemRights::WriteExtendedAttributes;
				else if (f_right == "DeleteSubdirectoriesAndFiles")
					fright = FileSystemRights::DeleteSubdirectoriesAndFiles;
				else if (f_right == "Delete")
					fright = FileSystemRights::Delete;
				else if (f_right == "ReadPermissions")
					fright = FileSystemRights::ReadPermissions;
				else if (f_right == "ChangePermissions")
					fright = FileSystemRights::ChangePermissions;
				else if (f_right == "TakeOwnership")
					fright = FileSystemRights::TakeOwnership;

				return fright;
			}
		};
	}
}