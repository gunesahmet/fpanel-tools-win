#pragma once
#include "../env/Env.h"
#include "../env/Logger.h"

using namespace System;
using namespace MailEnable::Administration;
using namespace Environments;

namespace Mail
{
	private ref class Enable
	{
	private:
		Environments::Env ^env;
		Environments::FileLogger ^logger;
	public:
		void setEnv(Environments::Env ^_env)
		{
			env = _env;
		}
		
		void setLogger(Environments::FileLogger ^_logger)
		{
			logger = _logger;
		}

		void createDomain(String ^domain, String ^quota)
		{
			try {
				if (String::IsNullOrEmpty(domain)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}

				createPostOffice(domain);
				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();

				String ^sRedirectionHost = "www." + domain;
				int iRedirectionStatus = 1;

				oDomain->AccountName = domain;
				oDomain->DomainName = domain;
				oDomain->Status = 1;
				oDomain->DomainRedirectionHosts = "";
				oDomain->DomainRedirectionStatus = -1;
				if ((oDomain->GetDomain() != 1)) {

					oDomain->Status = 1;
					oDomain->DomainRedirectionHosts = sRedirectionHost;
					oDomain->DomainRedirectionStatus = iRedirectionStatus;
					if ((oDomain->AddDomain() == 1)) {
						MailEnable::Administration::AddressMap ^oAddressMap = gcnew MailEnable::Administration::AddressMap();
						String ^sPostmasterAccount = "Postmaster";
						String ^sAbuseAccount = "Abuse";
						String ^sCatchAllAccount = "CatchAll";
						oAddressMap->Account = domain;
						oAddressMap->DestinationAddress = ("[SF:" + (domain + ("/" + (sPostmasterAccount + "]"))));
						oAddressMap->SourceAddress = ("[SMTP:Postmaster@" + (domain + "]"));
						oAddressMap->Scope = "0";
						if ((oAddressMap->AddAddressMap() != 1)) {
							//  Cant add address map for some reason...
						}

						oAddressMap = nullptr;
						oAddressMap = gcnew MailEnable::Administration::AddressMap();
						oAddressMap->Account = domain;
						oAddressMap->DestinationAddress = ("[SF:" + (domain + ("/" + (sAbuseAccount + "]"))));
						oAddressMap->SourceAddress = ("[SMTP:Abuse@" + (domain + "]"));
						oAddressMap->Scope = "0";
						if ((oAddressMap->AddAddressMap() != 1)) {
							//  Cant add address map for some reason...
						}

						oAddressMap = nullptr;
						oAddressMap = gcnew MailEnable::Administration::AddressMap();
						oAddressMap->Account = domain;
						oAddressMap->DestinationAddress = ("[SF:" + (domain + ("/" + (sCatchAllAccount + "]"))));
						oAddressMap->SourceAddress = ("[SMTP:*@" + (domain + "]"));
						oAddressMap->Scope = "0";
						if ((oAddressMap->AddAddressMap() != 1)) {
							//  Cant add address map for some reason...
						}

						oAddressMap = nullptr;
					}
					else {
						//  Domain add failed!
						logger->errorLog(domain + " can't create", 1710);
						System::Environment::Exit(1710);
					}

				}
				else {
					//  Domain already exists!
					logger->errorLog(domain + " already exists", 1710);
					System::Environment::Exit(1710);
				}
				logger->infoLog(domain + " added to mail server", 1706);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1716);
			}
		}

		void updateDomain(String ^domain, String ^quota)
		{
			try {
				if (String::IsNullOrEmpty(domain)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}
				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				String ^sPostoffice = domain;
				String ^sDomainName = domain;
				oDomain->AccountName = sPostoffice;
				oDomain->DomainName = sDomainName;
				if ((oDomain->GetDomain() == 1)) {
					// Edit parametrelerine göre iþlem yapýlacak
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

		void removeDomain(String ^domain)
		{
			try {
				if (String::IsNullOrEmpty(domain)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}

				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				MailEnable::Administration::Postoffice ^oPostOffice = gcnew MailEnable::Administration::Postoffice();

				String ^sPostoffice = domain;
				String ^sDomainName = domain;
				oDomain->AccountName = sPostoffice;
				oDomain->DomainName = sDomainName;
				oDomain->Status = -1;
				oDomain->DomainRedirectionHosts = "";
				oDomain->DomainRedirectionStatus = -1;

				oPostOffice->Account = domain;
				oPostOffice->Name = domain;
				int stat_remove_dom = oDomain->RemoveDomain();
				oPostOffice->RemovePostoffice();
				if ((stat_remove_dom == 1)) {
					logger->infoLog(domain + " removed from mail server", 1708);
				}
				else {
					//  Domain removal failed
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1718);
			}
		}

		void listDomains()
		{
			try {
				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				String ^sPostoffice = "MailEnable";
				String ^sDomainName;
				oDomain->AccountName = sPostoffice;
				oDomain->DomainName = "";
				oDomain->Status = -1;
				oDomain->DomainRedirectionHosts = "";
				oDomain->DomainRedirectionStatus = -1;
				if ((oDomain->FindFirstDomain() == 1)) {
					for (; (oDomain->FindNextDomain() == 1);) {
						sDomainName = oDomain->DomainName;
						oDomain->AccountName = sPostoffice;
						oDomain->DomainName = "";
						oDomain->Status = -1;
						oDomain->DomainRedirectionHosts = "";
						oDomain->DomainRedirectionStatus = -1;
					}
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1719);
			}
		}

		void createPostOffice(String ^domain)
		{
			try {
				MailEnable::Administration::Postoffice ^postoffice = gcnew MailEnable::Administration::Postoffice();
				postoffice->Account = domain;
				postoffice->Host = domain;
				postoffice->Name = domain;
				postoffice->Status = 1;
				if ((postoffice->GetPostoffice() == 0)) {
					postoffice->AddPostoffice();
				}
				else {
					logger->errorLog(domain + " can't create", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1720);
			}
		}

		void createMailBox(String ^mail_addr, String ^passwd, String ^quota)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr) || String::IsNullOrEmpty(passwd)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}

				Int64 disk_Limit;

				if ((quota == "0") || (quota == "unlimited") || String::IsNullOrEmpty(quota)) {
					disk_Limit = -1;
				}
				else {
					disk_Limit = Convert::ToInt32(quota);
				}

				String ^sPostoffice = domainname;
				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();
				String ^sMailboxName = mail_address;
				String ^sPassword = passwd;
				String ^sRights = "USER";
				oLogin->Account = sPostoffice;
				oLogin->LastAttempt = -1;
				oLogin->LastSuccessfulLogin = -1;
				oLogin->LoginAttempts = -1;
				oLogin->Password = "";
				oLogin->Rights = "";
				oLogin->Status = -1;
				oLogin->UserName = (sMailboxName + ("@" + sPostoffice));
				if ((oLogin->GetLogin() == 0)) {
					oLogin->Account = sPostoffice;
					oLogin->LastAttempt = 0;
					oLogin->LastSuccessfulLogin = 0;
					oLogin->LoginAttempts = 0;
					oLogin->Password = sPassword;
					oLogin->Rights = sRights;
					oLogin->Status = 1;
					//  0  Disabled, 1  Enabled
					oLogin->UserName = (sMailboxName + ("@" + sPostoffice));
					if ((oLogin->AddLogin() != 1)) {
						logger->errorLog(mail_addr + " already exists", 1724);
						System::Environment::Exit(1724);
					}
				}

				oMailbox->Postoffice = sPostoffice;
				oMailbox->MailboxName = sMailboxName;
				oMailbox->Size = disk_Limit;
				oMailbox->Limit = -1; // Saatlik gönderim limiti
				//  -1  Unlimited OR size value (in KB)
				oMailbox->Status = 1;
				if ((oMailbox->AddMailbox() != 1)) {
					//  Failed to add mailbox
					logger->errorLog(mail_addr + " can't create", 1725);
					System::Environment::Exit(1725);

				}

				MailEnable::Administration::AddressMap ^oAddressMap = gcnew MailEnable::Administration::AddressMap();
				oAddressMap->Account = sPostoffice;
				oAddressMap->DestinationAddress = ("[SF:" + (sPostoffice + ("/" + (sMailboxName + "]"))));
				oAddressMap->SourceAddress = ("[SMTP:" + (sMailboxName + "@" + domainname + "]"));
				oAddressMap->Scope = "0";
				if ((oAddressMap->AddAddressMap() != 1)) {
					//  Failed to add Address Map for some reason!
					logger->errorLog(mail_addr + " can't create", 1726);
					System::Environment::Exit(1726);
				}

				logger->infoLog(mail_addr + " created", 1711);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1721);
			}
		}

		void updateMailBox(String ^mail_addr, String ^quota, String ^new_passwd)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}

				Int64 bandwith_limit;

				if ((quota == "0") || (quota == "unlimited") || (String::IsNullOrEmpty(quota))) {
					bandwith_limit = -1;
				}
				else {
					bandwith_limit = Convert::ToInt32(quota) * 1024;
				}

				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();

				oLogin->Account = domainname;
				oLogin->UserName = (mail_address + ("@" + domainname));

				if ((oLogin->GetLogin() == 1)) {
					String ^password;
					if (new_passwd == nullptr)
						password = oLogin->Password;
					else
						password = new_passwd;

					oLogin->UserName = (mail_address + ("@" + domainname));
					if ((oLogin->EditLogin(mail_address, oLogin->Status, password, domainname, oLogin->Description, oLogin->LoginAttempts, oLogin->LastAttempt, oLogin->LastSuccessfulLogin, oLogin->Rights) != 1)) {
						logger->errorLog(mail_addr + " already exists", 1724);
						System::Environment::Exit(1724);
					}
				}

				oMailbox->Postoffice = domainname;
				oMailbox->MailboxName = mail_address;
				Int64 mStatus = 1;
				if (oMailbox->Size >= bandwith_limit) {
					mStatus = 0;
				}
				if ((oMailbox->EditMailbox(domainname, mail_address, oMailbox->RedirectAddress, oMailbox->RedirectStatus, mStatus, bandwith_limit, oMailbox->Size) != 1)) {
					//  Failed to add mailbox
					logger->errorLog(mail_addr + " can't update", 1725);
					System::Environment::Exit(1725);

				}

				//MailEnable.Administration.AddressMap oAddressMap = new MailEnable.Administration.AddressMap();
				//oAddressMap.Account = domainname;
				//oAddressMap.DestinationAddress = ("[SF:" + (sPostoffice + ("/" + (sMailboxName + "]"))));
				//oAddressMap.Scope = "";
				//oAddressMap.SourceAddress = ("[SMTP:" + (sMailboxName + "@" + domainname + "]"));
				//int addressMapAdded = oAddressMap.AddAddressMap();

				logger->infoLog(mail_addr + " updated", 1711);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1722);
			}
		}

		void removeMailBox(String ^mail_addr)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				String ^sPostoffice = domainname;
				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				oMailbox->Postoffice = sPostoffice;
				oMailbox->MailboxName = mail_address;
				if ((oMailbox->RemoveMailbox() == 1)) {
					//  Now remove the corresponding Address Map entry
					MailEnable::Administration::AddressMap ^oAddressMap = gcnew MailEnable::Administration::AddressMap();
					oAddressMap->Account = sPostoffice;
					oAddressMap->DestinationAddress = ("[SF:" + (sPostoffice + ("/" + (mail_address + "]"))));
					if ((oAddressMap->RemoveAddressMap(true) == 0)) {
						//  Failed to remove Address Map for some reason!

					}
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1722);
			}
		}

		void createMailAlias(String ^mail_addr, String ^mail_alias)
		{
			try {
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


				MailEnable::Administration::AddressMap ^oAddressMap = gcnew MailEnable::Administration::AddressMap();
				oAddressMap->Account = domainname;
				oAddressMap->DestinationAddress = ("[SF:" + (domainname + ("/" + (mail_address + "]"))));
				oAddressMap->Scope = "";
				oAddressMap->SourceAddress = ("[SMTP:" + (mail_alias_address + "@" + domainname + "]"));
				int addressMapAdded = oAddressMap->AddAddressMap();
				logger->infoLog(mail_addr + " updated", 1711);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1723);
			}



		}

		void removeMailAlias(String ^mail_addr, String ^mail_alias)
		{
			try {
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

				MailEnable::Administration::AddressMap ^oAddressMap = gcnew MailEnable::Administration::AddressMap();
				oAddressMap->Account = domainname;
				oAddressMap->DestinationAddress = ("[SF:" + (domainname + ("/" + (mail_address + "]"))));
				oAddressMap->Scope = "";
				oAddressMap->SourceAddress = ("[SMTP:" + (mail_alias_address + "@" + domainname + "]"));
				oAddressMap->RemoveAddressMap(true);
				logger->infoLog(mail_addr + " updated", 1711);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1724);
			}
		}

		void suspendDomain(String ^domain)
		{
			try {
				if (String::IsNullOrEmpty(domain)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}

				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				if ((oDomain->Exists(domain) == true)) {

					oDomain->AccountName = domain;
					oDomain->DomainName = domain;
					if (oDomain->EditDomain(domain, 0, oDomain->DomainRedirectionStatus, oDomain->DomainRedirectionHosts, domain) != 1) {
						MailEnable::Administration::Postoffice ^postoffice = gcnew MailEnable::Administration::Postoffice();
						postoffice->Account = domain;
						postoffice->Host = domain;
						postoffice->Name = domain;
						if ((postoffice->GetPostoffice() == 1)) {
							postoffice->EditPostoffice(domain, 0, domain);
						}
						else {
							logger->errorLog(domain + " can't create", 1715);
							System::Environment::Exit(1715);
						}
					}
					else {
						//@TODO HAta ver çýk
					}
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1724);
			}
		}

		void unsuspendDomain(String ^domain)
		{
			try {
				if (String::IsNullOrEmpty(domain)) {
					logger->errorLog("Missing arguments", 1709);
					System::Environment::Exit(1709);
				}


				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				if ((oDomain->Exists(domain) == true)) {
					oDomain->AccountName = domain;
					oDomain->DomainName = domain;
					if (oDomain->EditDomain(domain, 1, oDomain->DomainRedirectionStatus, oDomain->DomainRedirectionHosts, domain) != 1) {
						// @TODO Hata ver çýk
					}

					MailEnable::Administration::Postoffice ^postoffice = gcnew MailEnable::Administration::Postoffice();
					postoffice->Account = domain;
					postoffice->Host = domain;
					postoffice->Name = domain;
					if ((postoffice->GetPostoffice() == 1)) {
						postoffice->EditPostoffice(domain, 1, domain);
					}
					else {
						logger->errorLog(domain + " can't create", 1715);
						System::Environment::Exit(1715);
					}
				}
				else {
					logger->errorLog(domain + " not found", 1715);
					System::Environment::Exit(1715);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1725);
			}



		}

		void suspendMail(String ^mail_addr)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}

				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();

				oLogin->Account = domainname;
				oLogin->UserName = (mail_address + ("@" + domainname));
				if ((oLogin->GetLogin() == 1)) {
					if ((oLogin->EditLogin(oLogin->UserName, 0, oLogin->Password, oLogin->Account, oLogin->Description, oLogin->LoginAttempts, oLogin->LastAttempt, oLogin->LastSuccessfulLogin, oLogin->Rights) != 1)) {
						logger->errorLog(mail_addr + " already exists", 1724);
						System::Environment::Exit(1724);
					}
				}

				oMailbox->Postoffice = domainname;
				oMailbox->MailboxName = mail_address;
				Console::WriteLine(oMailbox->GetMailbox());
				if ((oMailbox->EditMailbox(domainname, mail_address, oMailbox->RedirectAddress, oMailbox->RedirectStatus, 0, oMailbox->Limit, oMailbox->Size) != 1)) {
					logger->errorLog(mail_addr + " can't update", 1725);
					System::Environment::Exit(1725);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1727);
			}



		}

		void unsuspendMail(String ^mail_addr)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}

				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();

				oLogin->Account = domainname;
				oLogin->UserName = (mail_address + ("@" + domainname));
				if ((oLogin->GetLogin() == 1)) {
					if ((oLogin->EditLogin(oLogin->UserName, 1, oLogin->Password, oLogin->Account, oLogin->Description, oLogin->LoginAttempts, oLogin->LastAttempt, oLogin->LastSuccessfulLogin, oLogin->Rights) != 1)) {
						logger->errorLog(mail_addr + " already exists", 1724);
						System::Environment::Exit(1724);
					}
				}

				oMailbox->Postoffice = domainname;
				oMailbox->MailboxName = mail_address;
				if ((oMailbox->EditMailbox(domainname, mail_address, oMailbox->RedirectAddress, oMailbox->RedirectStatus, 1, oMailbox->Limit, oMailbox->Size) != 1)) {
					//  Failed to add mailbox
					logger->errorLog(mail_addr + " can't update", 1725);
					System::Environment::Exit(1725);

				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1728);
			}
		}

		void createMailForward(String ^mail_addr, String ^mail_forward)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}


				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();

				oLogin->Account = domainname;
				oLogin->UserName = (mail_address + ("@" + domainname));
				if ((oLogin->GetLogin() != 1)) {
					//@Todo hata ver çýk				
				}
				Int64 mStatus = 1;

				oMailbox->Postoffice = domainname;
				oMailbox->MailboxName = mail_address;
				if ((oMailbox->EditMailbox(domainname, mail_address, mail_forward, 1, mStatus, oMailbox->Limit, oMailbox->Size) != 1)) {
					//  Failed to add mailbox
					logger->errorLog(mail_addr + " can't update", 1725);
					System::Environment::Exit(1725);

				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1728);
			}
		}

		void removeMailForward(String ^mail_addr)
		{
			try {
				if (String::IsNullOrEmpty(mail_addr)) {
					logger->errorLog("Missing arguments", 1722);
					System::Environment::Exit(1722);
				}
				array<String^> ^mail_addr_splt = mail_addr->Split('@');
				String ^domainname = mail_addr_splt[1];
				String ^mail_address = mail_addr_splt[0];

				if (checkDomain(domainname) == false) {
					logger->errorLog(domainname + " not found", 1723);
					System::Environment::Exit(1723);
				}

				MailEnable::Administration::Mailbox ^oMailbox = gcnew MailEnable::Administration::Mailbox();
				MailEnable::Administration::Login ^oLogin = gcnew MailEnable::Administration::Login();

				oLogin->Account = domainname;
				oLogin->UserName = (mail_address + ("@" + domainname));
				if ((oLogin->GetLogin() == 1)) {
					//@Todo hata ver çýk				
				}

				oMailbox->Postoffice = domainname;
				oMailbox->MailboxName = mail_address;
				if ((oMailbox->EditMailbox(domainname, mail_address, nullptr, 0, 1, oMailbox->Limit, oMailbox->Size) != 1)) {
					logger->errorLog(mail_addr + " can't update", 1725);
					System::Environment::Exit(1725);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1729);
			}
		}

		bool checkDomain(String ^domainname)
		{
			try {
				MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
				return oDomain->Exists(domainname);
			}
			catch (Exception ^e) {
				logger->debugLog("mailmng: " + e->Message, 1731);
				return false;
			}

		}

		void domainSettings(String ^domain, String ^options, String ^mail_addr)
		{
			//try {

			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Missing arguments", 1709);
				System::Environment::Exit(1709);
			}
			String ^sPostoffice = domain;
			String ^sDomainName = domain;
			String ^redirect_mail_account;


			MailEnable::Administration::Domain ^oDomain = gcnew MailEnable::Administration::Domain();
			MailEnable::Administration::Postoffice ^oPostOffice = gcnew MailEnable::Administration::Postoffice();

			oDomain->AccountName = sPostoffice;
			oDomain->DomainName = sDomainName;
			oDomain->Status = -1;
			oDomain->DomainRedirectionHosts = "";
			oDomain->DomainRedirectionStatus = -1;

			oPostOffice->Account = domain;
			oPostOffice->Name = domain;
			/*		oDomain->DomainRedirectionHosts = "";
					oDomain->DomainRedirectionStatus = -1;*/
			if ((oDomain->GetDomain() == 1)) {
				if (options != "reject") {
					redirect_mail_account = mail_addr->Replace("@" + domain, "");
					MailEnable::Administration::AddressMap ^addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->EditAddressMap(sDomainName, "[SMTP:*@" + sDomainName + "]", "[SF:" + sPostoffice + "/" + redirect_mail_account + "]", "0", 1);

					addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->Account = sPostoffice;
					addressMap->DestinationAddress = "[SF:" + sPostoffice + "/" + redirect_mail_account + "]";
					addressMap->SourceAddress = "[SMTP:Postmaster@" + sDomainName + "]";
					addressMap->Scope = "0";

					if ((addressMap->AddAddressMap() != 1)) {
						Console::WriteLine("Failed Postmaster Redirect");
					}

					addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->Account = sPostoffice;
					addressMap->DestinationAddress = "[SF:" + sPostoffice + "/" + redirect_mail_account + "]";
					addressMap->SourceAddress = "[SMTP:Abuse@" + sDomainName + "]";
					addressMap->Scope = "0";
					if ((addressMap->AddAddressMap() != 1)) {
						Console::WriteLine("Failed Postmaster Abuse");
					}
				}
				else {
					MailEnable::Administration::AddressMap ^addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->EditAddressMap(sDomainName, "[SMTP:*@" + sDomainName + "]", "", "0", 1);

					addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->Account = sPostoffice;
					addressMap->DestinationAddress = "";
					addressMap->SourceAddress = "[SMTP:Postmaster@" + sDomainName + "]";
					addressMap->Scope = "0";

					if ((addressMap->AddAddressMap() != 1)) {
						Console::WriteLine("Failed Postmaster Redirect");
					}

					addressMap = gcnew MailEnable::Administration::AddressMap();
					addressMap->Account = sPostoffice;
					addressMap->DestinationAddress = "";
					addressMap->SourceAddress = "[SMTP:Abuse@" + sDomainName + "]";
					addressMap->Scope = "0";
					if ((addressMap->AddAddressMap() != 1)) {
						Console::WriteLine("Failed Postmaster Abuse");
					}
				}
			}
			else {
				logger->errorLog(domain + " not found", 1715);
				System::Environment::Exit(1715);
			}
			//}
			//catch (Exception ^e) {
			//	logger->debugLog("mailmng: " + e->Message, 1717);
			//}
		}
	};
		
}
