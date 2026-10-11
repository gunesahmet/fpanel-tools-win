#pragma once
#include "Logger.h"
#include "FileManager.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Security::AccessControl;

namespace FPanel
{
	namespace Windows
	{
		private ref class Directories
		{
		private:
			enum class SymbolicLink
			{
				File = 0,
				Directory = 1
			};

			[DllImport("kernel32.dll")]
			static bool CreateSymbolicLink(String ^lpSymlinkFileName, String ^lpTargetFileName, SymbolicLink dwFlags);
		private:
			Logger ^logger;
		public:
			void create(String ^domain, String ^path, String ^username, String ^ftpPath)
			{

				if (checkDomainDirectory(path) == false) {
					// Dizinler oluþturuluyor
					try {
						Directory::CreateDirectory(path + "\\cgi-bin");
						Directory::CreateDirectory(path + "\\conf\\prodir");
						Directory::CreateDirectory(path + "\\conf\\ssl");
						Directory::CreateDirectory(path + "\\conf\\subdomains");
						Directory::CreateDirectory(path + "\\error-docs");
						Directory::CreateDirectory(path + "\\vhost" );
						Directory::CreateDirectory(path + "\\vhost\\httpdoc" );
						Directory::CreateDirectory(path + "\\stats\\logs");
						Directory::CreateDirectory(path + "\\stats\\web-stats");
						Directory::CreateDirectory(path + "\\subdomains");
						Directory::CreateDirectory(ftpPath);

						// Creating symbolic links
						CreateSymbolicLink(ftpPath + "\\vhost", path + "\\vhost", SymbolicLink::Directory);
						CreateSymbolicLink(ftpPath + "\\cgi-bin", path + "\\cgi-bin", SymbolicLink::Directory);
						CreateSymbolicLink(ftpPath + "\\error-docs", path + "\\error-docs", SymbolicLink::Directory);
						CreateSymbolicLink(ftpPath + "\\subdomains", path + "\\subdomains", SymbolicLink::Directory);

						// Permissions
						FileManager ^fm = gcnew FileManager();
						fm->ReadAndExecute(path, username, AccessControlType::Allow);

						fm->AddDirectorySecurity(path + "\\cgi-bin", username, FileSystemRights::FullControl, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\error-docs", username, FileSystemRights::FullControl, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\vhost", username, FileSystemRights::FullControl, AccessControlType::Allow);


						fm->AddDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ListDirectory, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ReadData, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ReadAttributes, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ReadExtendedAttributes, AccessControlType::Allow);
						fm->AddDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ReadPermissions, AccessControlType::Allow);

						// Subdomains dizinine sadece okuma izni verilecek altýndaki subdomainlere sadece full izin verilebilir
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::DeleteSubdirectoriesAndFiles, AccessControlType::Allow);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::Delete, AccessControlType::Allow);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ChangePermissions, AccessControlType::Allow);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::TakeOwnership, AccessControlType::Allow);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::FullControl, AccessControlType::Allow);

						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::DeleteSubdirectoriesAndFiles, AccessControlType::Deny);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::Delete, AccessControlType::Deny);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::ChangePermissions, AccessControlType::Deny);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::TakeOwnership, AccessControlType::Deny);
						fm->RemoveDirectorySecurity(path + "\\subdomains", username, FileSystemRights::FullControl, AccessControlType::Deny);
						
						fm->RemoveFull(path, "fpusers");
						fm->RemoveListContent(path, "fpusers");
						fm->Read(path, "fpusers", AccessControlType::Deny);
						fm->Write(path, "fpusers", AccessControlType::Deny);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Directories::create. Error: " + e->Message, 1105);
					}
				}
				else {
					logger->errorLog("Domain directory is already exists", 2601);
				}
			}

			void update(String ^domain, String ^path, String ^username, String ^ftpPath, String ^newUsername, String ^newFtpPath)
			{
				try {
					bool status = this->checkDomainDirectory(path);

					if (status == true) {
						Directory::Move(ftpPath, newFtpPath);
					}
					else {
						logger->errorLog("Domain directory is not exists", 2602);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Directories::update. Error: " + e->Message, 1105);
				}
			}

			void remove(String ^path, String ^ftpPath)
			{
				try {
					bool status = this->checkDomainDirectory(path);
					if (status == true) {
						if (path != "c:\\vhosts\\")
							Directory::Delete(path, true);
						if (ftpPath != "c:\\inetpub\\ftproot\\localuser\\")
							Directory::Delete(ftpPath, true);
					}
					else {
						logger->errorLog("Domain directory is not exists", 2602);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Directories::remove. Error: " + e->Message, 1105);
				}
			}

			bool checkDomainDirectory(String ^path)
			{
				if (!Directory::Exists(path)) {
					return false; // Dizin yok
				}
				return true; // Dizin var
			}

		private:
			void InitializeInstanceFields()
			{
				logger = gcnew Logger();
			}

		public:
			Directories()
			{
				InitializeInstanceFields();
			}
		};
	}
}