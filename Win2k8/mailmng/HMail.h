#pragma once
#include "../env/Env.h"
#include "../env/Logger.h"

using namespace System;
using namespace hMailServer;
using namespace Environments;
using namespace System::Runtime::InteropServices;

namespace HMail
{
	private ref class Server
	{
	private:
		static Environments::Env ^env;
		static Environments::FileLogger ^logger;
	public:
		void setEnv(Environments::Env ^_env)
		{
			env = _env;
		}

		void setLogger(Environments::FileLogger ^_logger)
		{
			logger = _logger;
		}

		void createDomain(String ^domain_name, String ^quota)
		{
			try {
				if (String::IsNullOrEmpty(domain_name)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}

				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}

				hMailServer::Domain ^domain = hMailApp->Domains->Add();
				domain->Name = domain_name;
				if ((quota == "0") || (quota == "unlimited") || String::IsNullOrEmpty(quota)) {
					domain->MaxSize = 0;
				}
				else {
					domain->MaxSize = Convert::ToInt32(quota) / 1024 / 1024;
				}

				domain->Active = true;
				// set postmaster and save changes
				domain->Postmaster = "postmaster@" + domain_name;
				domain->Save();
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
			logger->infoLog(domain_name + " added to mail server", 1706);
		}

		void updateDomain(String ^domain_name, String ^quota)
		{
			try {
				if (String::IsNullOrEmpty(domain_name)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];

				if ((domain != nullptr)) {
					if ((quota == "0") || (quota == "unlimited") || String::IsNullOrEmpty(quota)) {
						domain->MaxSize = 0;
					}
					else {
						domain->MaxSize = Convert::ToInt32(quota) / 1024 / 1024;
					}
					domain->Active = true;
					domain->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
			}
		}

		void removeDomain(String ^domain_name)
		{
			try {
				if (String::IsNullOrEmpty(domain_name)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];

				if ((domain != nullptr)) {
					domain->Delete();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
			}
		}

		void createMailBox(String ^mail_addr, String ^passwd, String ^quota)
		{
			if (String::IsNullOrEmpty(mail_addr) || String::IsNullOrEmpty(passwd)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}

			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}
			int max_msg_size;
			if ((quota == "0") || (quota == "unlimited") || String::IsNullOrEmpty(quota)) {
				max_msg_size = 0;
			}
			else {
				max_msg_size = Convert::ToInt32(quota);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {
					
					Account ^account = domain->Accounts->Add();
					account->Address = mail_addr;
					account->Password = passwd;
					account->Active = true;
					account->AdminLevel = eAdminLevel::hAdminLevelNormal;
					account->MaxSize = max_msg_size;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
			logger->infoLog(mail_addr + " created", 1711);
		}

		void updateMailBox(String ^mail_addr, String ^quota, String ^new_passwd)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			int max_msg_size;
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}
			if ((quota == "0") || (quota == "unlimited") || String::IsNullOrEmpty(quota)) {
				max_msg_size = 0;
			}
			else {
				max_msg_size = Convert::ToInt32(quota);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {
					
					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					
					if (new_passwd != nullptr)
						account->Password = new_passwd;
					account->MaxSize = max_msg_size;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void removeMailBox(String ^mail_addr)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			//int max_msg_size;
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];

			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {
					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					account->Delete();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void createMailAlias(String ^mail_addr, String ^mail_alias)
		{
			if (String::IsNullOrEmpty(mail_addr) || String::IsNullOrEmpty(mail_alias)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}

			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			array<String^> ^mail_alias_splt = mail_alias->Split('@');
			String ^domainname = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			String ^mail_alias_address = mail_alias_splt[0];

			if (checkDomain(domainname) == false) {
				logger->errorLog(domainname + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domainname];
				if ((domain != nullptr)) {
					Alias ^alias = domain->Aliases->Add();
					alias->Active = true;
					alias->Name = mail_alias;
					alias->Value = mail_addr;
					alias->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void removeMailAlias(String ^mail_addr, String ^mail_alias)
		{
			if (String::IsNullOrEmpty(mail_addr) || String::IsNullOrEmpty(mail_alias)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}

			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			array<String^> ^mail_alias_splt = mail_alias->Split('@');
			String ^domainname = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			String ^mail_alias_address = mail_alias_splt[0];

			if (checkDomain(domainname) == false) {
				logger->errorLog(domainname + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domainname];
				if ((domain != nullptr)) {
					Alias ^alias = domain->Aliases->ItemByName[mail_alias];
					alias->Delete();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void suspendDomain(String ^domain_name)
		{
			if (String::IsNullOrEmpty(domain_name)) {
				logger->errorLog("Missing arguments", 1709);
				System::Environment::Exit(1709);
			}

			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];

				if ((domain != nullptr)) {
					domain->Active = false;
					domain->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
			}
		}

		void unsuspendDomain(String ^domain_name)
		{
			if (String::IsNullOrEmpty(domain_name)) {
				logger->errorLog("Missing arguments", 1709);
				System::Environment::Exit(1709);
			}

			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];

				if ((domain != nullptr)) {
					domain->Active = true;
					domain->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
			}
		}

		void suspendMail(String ^mail_addr)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {

					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					account->Active = false;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void unsuspendMail(String ^mail_addr)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {

					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					account->Active = true;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void createMailForward(String ^mail_addr, String ^mail_forward)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {
					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					account->ForwardEnabled = true;
					account->ForwardAddress = mail_forward;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		void removeMailForward(String ^mail_addr)
		{
			if (String::IsNullOrEmpty(mail_addr)) {
				logger->errorLog("Missing arguments", 1722);
				System::Environment::Exit(1722);
			}
			array<String^> ^mail_addr_splt = mail_addr->Split('@');
			String ^domain_name = mail_addr_splt[1];
			String ^mail_address = mail_addr_splt[0];
			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}

			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
				if ((domain != nullptr)) {
					Account ^account = domain->Accounts->ItemByAddress[mail_addr];
					account->ForwardEnabled = false;
					account->ForwardAddress = nullptr;
					account->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
				System::Environment::Exit(0);
			}
		}

		bool checkDomain(String ^domain_name)
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					return false;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];
			}
			catch (...) {
				return false;
			}
			return true;
		}

		void domainSettings(String ^domain_name, String ^options, String ^mail_addr)
		{
			if (String::IsNullOrEmpty(domain_name)) {
				logger->errorLog("Missing arguments", 1709);
				System::Environment::Exit(1709);
			}

			if (checkDomain(domain_name) == false) {
				logger->errorLog(domain_name + " not found", 1723);
				System::Environment::Exit(1723);
			}
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->debugLog("Mail server auth failed", 1716);
					return;
				}
				hMailServer::Domain ^domain = hMailApp->Domains->ItemByName[domain_name];

				if ((domain != nullptr)) {
					if (options != "reject") {
						domain->Postmaster = mail_addr;
					}
					else {
						domain->Postmaster = nullptr;
					}
					domain->Save();
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1717);
			}
		}
	};

}
