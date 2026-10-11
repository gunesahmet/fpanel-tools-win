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
		private ref class SubDirectory
		{
		private:
			enum class SymbolicLink
			{
				File = 0,
				Directory = 1
			};

			[DllImport("kernel32.dll")]
			static bool CreateSymbolicLink(String ^lpSymlinkFileName, String ^lpTargetFileName, SymbolicLink dwFlags);

		public:
			void create(String ^subPath, String ^username, String ^subusername, String ^subFtpPath, String ^ftpType)
			{
				bool status = this->checkDomain(subPath);
				FileManager ^fm = gcnew FileManager();

				if (status == false) {
					try {
						Directory::CreateDirectory(subPath);
						Directory::CreateDirectory(subPath + "\\cgi-bin");
						Directory::CreateDirectory(subPath + "\\error-docs");
						Directory::CreateDirectory(subPath + "\\httpdoc");
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create::main" + ". Error: " + e->Message, 1105);
					}

					if (ftpType == "native") {
						try {
							Directory::CreateDirectory(subFtpPath);
						}
						catch (Exception ^e) {
							Logger::errorLog("Error " + this->GetType()->Name + ":create::native" + ". Error: " + e->Message, 1105);
						}
						CreateSymbolicLink(subFtpPath + "\\httpdoc", subPath + "\\httpdoc", SymbolicLink::Directory);
						CreateSymbolicLink(subFtpPath + "\\cgi-bin", subPath + "\\cgi-bin", SymbolicLink::Directory);
						CreateSymbolicLink(subFtpPath + "\\error-docs", subPath + "\\error-docs", SymbolicLink::Directory);

						fm->AddDirectorySecurity(subPath + "\\cgi-bin", subusername, FileSystemRights::FullControl, AccessControlType::Allow);
						fm->AddDirectorySecurity(subPath + "\\error-docs", subusername, FileSystemRights::FullControl, AccessControlType::Allow);
						fm->AddDirectorySecurity(subPath + "\\httpdoc", subusername, FileSystemRights::FullControl, AccessControlType::Allow);
					}

					fm->AddDirectorySecurity(subPath + "\\cgi-bin", username, FileSystemRights::FullControl, AccessControlType::Allow);
					fm->AddDirectorySecurity(subPath + "\\error-docs", username, FileSystemRights::FullControl, AccessControlType::Allow);
					fm->AddDirectorySecurity(subPath + "\\httpdoc", username, FileSystemRights::FullControl, AccessControlType::Allow);
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":create", 1105);
				}
			}

			void update(String ^subPath, String ^username, String ^subFtpPath, String ^newUsername, String ^newSubFtpPath)
			{
				bool status = this->checkDomain(subPath);

				if (status == true) {
					try {
						Directory::Move(subFtpPath, newSubFtpPath);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create::update" + ". Error: " + e->Message, 1105);
					}
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":update", 1105);
				}
			}

			void remove(String ^subPath, String ^subFtpPath)
			{
				bool status = this->checkDomain(subPath);

				if (status == true) {
					try {
						if (subPath != "c:\\vhosts\\")
							Directory::Delete(subPath, true);
						if (subFtpPath != "c:\\inetpub\\ftproot\\localuser\\")
							Directory::Delete(subFtpPath, true);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create::remove" + ". Error: " + e->Message, 1105);
					}
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":remove", 1105);
				}
			}

			bool checkDomain(String ^path)
			{
				if (!Directory::Exists(path)) {
					return false; // Dizin yok
				}
				return true; // Dizin var
			}
		};
	}
}