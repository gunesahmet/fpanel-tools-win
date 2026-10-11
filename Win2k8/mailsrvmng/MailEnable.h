#pragma once
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Utils.h"

using namespace System;
using namespace MailEnable::Administration;
using namespace Environments;
using namespace Microsoft::Win32;

namespace Mail
{
	private ref class Enable
	{
	private:
		Environments::Env ^env;
		Environments::FileLogger ^logger;
		Environments::FUtils ^utils;

	public:
		void setEnv(Environments::Env ^_env)
		{
			env = _env;
			utils = gcnew Environments::FUtils();
		}

		void setLogger(Environments::FileLogger ^_logger)
		{
			logger = _logger;
		}

		void init(String ^spf, String ^spf_best_guess, String ^spf_best_guesses, String ^spf_white_lists_status, 
			String ^spf_white_lists, String ^spf_pass_local, String ^max_msg_size_status, String ^max_msg_size, String ^sub_port_status,
			String ^sub_port, String ^sub_port_auth_mode, String ^smtpd_recipient_limit, String ^smtp_in_connection_limit, String ^smtp_out_connection_limit,
			String ^max_queue_lifetime, String ^dns_rbl_status, String ^dns_rbl_mode, String ^url_rbl_status, String ^url_rbl_mode, String ^me_dns_black_list,
			String ^me_url_black_list
		)
		{
			try {
				Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
				RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP", true);
				if (myKey != nullptr) {

					if (spf != nullptr) {
						if (spf == "on") {
							myKey->SetValue("SPF Action", "1", RegistryValueKind::DWord); 						// Enable SPF (Sender Policy Framework) checking
							myKey->SetValue("SPF Enabled", "1", RegistryValueKind::DWord);  					//

							try {
								if (spf_best_guess == "on") {
									myKey->SetValue("SPF Best Guess Status", "1", RegistryValueKind::DWord); // Apply best-guess policy for domains without SPF records
									myKey->SetValue("SPF Best Guess Policy", spf_best_guesses, RegistryValueKind::String);// This policy will be applied to all domains that do not have a SPF record
								}
								else {
									myKey->SetValue("SPF Best Guess Status", "0", RegistryValueKind::DWord); // Apply best-guess policy for domains without SPF records
								}
							}
							catch (...) {
							}


							try {
								if (spf_white_lists_status == "on") {
									myKey->SetValue("SPF Local Policy Status", "1", RegistryValueKind::DWord); // Enable local SPF whitelist policy
									myKey->SetValue("SPF Local Policy", spf_white_lists, RegistryValueKind::String);	// The whitelist policy is applied after the 'all' keyword is found. Use thi to add your own possible denials to inbound connections
								}
								else {
									myKey->SetValue("SPF Local Policy Status", "0", RegistryValueKind::DWord); // Enable local SPF whitelist policy
								}
							}
							catch (...) {
							}


							try {
								if (spf_pass_local == "on") {
									myKey->SetValue("SPF Pass Local IPs", "1", RegistryValueKind::DWord);	// Dont check connections from local IP addresses
								}
								else {
									myKey->SetValue("SPF Pass Local IPs", "0", RegistryValueKind::DWord);	// Dont check connections from local IP addresses
								}
							}
							catch (...) {
							}


							try {
								myKey->SetValue("SPF Use Trusted Forwarders", 0, RegistryValueKind::DWord);		// 
								myKey->SetValue("SPF Allow Backup MX", 0, RegistryValueKind::DWord);				//
							}
							catch (...) {
							}

						}
						else {
							myKey->SetValue("SPF Action", "0", RegistryValueKind::DWord); 						// Enable SPF (Sender Policy Framework) checking
							myKey->SetValue("SPF Enabled", "0", RegistryValueKind::DWord);  					//
						}
					}

					try {
						if (max_msg_size_status != nullptr) {
							if (max_msg_size_status == "on") {
								myKey->SetValue("Maximum Outbound Message Size", max_msg_size, RegistryValueKind::DWord); // Limit outbound message size (save as Byte)
							}
						}
						else {
							myKey->DeleteValue("Maximum Outbound Message Size");
						}
					}
					catch (...) {
					}


					try {
						if (sub_port_status != nullptr) {
							if (sub_port_status == "on") {
								myKey->SetValue("Alternate Port Enabled", "1", RegistryValueKind::DWord);
								myKey->SetValue("Alternate Port", sub_port, RegistryValueKind::DWord);					// Submission Port -> Listen on alternate port
								myKey->SetValue("Alternate Port Authentication Mode", sub_port_auth_mode, RegistryValueKind::DWord);	// Submission Port -> Authentication Mode -> 
																																		// Always allow authentication 
																																		// Never allow authentication
																																		// Only allow secure authentication (Usin SSL or TLS)
								myKey->SetValue("Alternate Port Relay Mode", "0", RegistryValueKind::DWord);
								myKey->SetValue("Alternate Port SSL Enabled", "0", RegistryValueKind::DWord);			// Submission Port -> Requires SSL
							}
							else {
								myKey->SetValue("Alternate Port Enabled", "0", RegistryValueKind::DWord);
							}
						}
					}
					catch (...) {
					}


					try {
						if (smtpd_recipient_limit != nullptr) {
							myKey->SetValue("Inbound Message Restriction", smtpd_recipient_limit, RegistryValueKind::DWord); // Limit number of recipients per hour to
						}
					}
					catch (...) {
					}


					try {
						if (smtp_in_connection_limit != nullptr) {
							myKey->SetValue("Maximum Recv Threads", smtp_in_connection_limit, RegistryValueKind::DWord); // Maximum number of concurrent connections
						}
					}
					catch (...) {
					}

					try {
						if (smtp_out_connection_limit != nullptr) {
							myKey->SetValue("Maximum Send Threads", smtp_out_connection_limit, RegistryValueKind::DWord); // Maximum number of send threads
						}
					}
					catch (...) {
					}

					try {
						if (max_queue_lifetime != nullptr) {
							myKey->SetValue("Outbound Failed Message Lifetime", max_queue_lifetime, RegistryValueKind::DWord); // Failed message lifetime as hours
						}
					}
					catch (...) {
					}

					try {
						if (dns_rbl_status != nullptr) {
							if (dns_rbl_status == "on") {
								myKey->SetValue("RDNS Handling Mode", dns_rbl_mode, RegistryValueKind::DWord); // Action when detected 
																									  // 0 => Dont accept the mail 
																									  // 1 => Mark the email as spam
							}
						}
					}
					catch (...) {
					}

					try {
						if (url_rbl_status != nullptr) {
							if (url_rbl_status == "on") {
								myKey->SetValue("URI SBL Status", "1", RegistryValueKind::DWord); // Enable URL Blacklisting
								myKey->SetValue("URI SBL Mode", url_rbl_mode, RegistryValueKind::DWord); // Action when detected 
																								// 0 => Dont accept the mail 
																								// 2 => Replace the link
																								// 3 => Mark the email as spam 
							}
							else {
								myKey->SetValue("URI SBL Status", "0", RegistryValueKind::DWord); // Enable URL Blacklisting
							}
						}
					}
					catch (...) {
					}
					myKey->Close();
				}

				if (dns_rbl_status != nullptr) {
					RegistryKey ^myKeyDnsRbl = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist", true);
					if (myKeyDnsRbl != nullptr) {
						if (dns_rbl_status == "on") {
							try {
								try {
									Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
									RegistryKey ^myKeyRbl = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist", true);
									String ^bl = myKeyRbl->GetValue("Processing Order")->ToString();
									myKeyRbl->Close();
									array<String^> ^b_arr = bl->Split(',');
									for (int i = 0; i < b_arr->Length; i++) {
										RegistryKey ^myKeyMeDnsBlackList = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist\\" + b_arr[i], true);
										if (myKeyMeDnsBlackList != nullptr) {
											myKeyMeDnsBlackList->SetValue("Status", "0", RegistryValueKind::DWord);
											myKeyMeDnsBlackList->Close();
										}
									}
								}
								catch (...) {
								}

								myKeyDnsRbl->SetValue("Enabled", "1", RegistryValueKind::DWord); 						// Enable DNS Blacklisting
								if (me_dns_black_list != nullptr) {
									array<String^> ^bdns_arr = me_dns_black_list->Split(',');
									for (int i = 0; i < bdns_arr->Length; i++) {
										RegistryKey ^myKeyMeDnsBlackList = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist\\" + bdns_arr[i], true);
										if (myKeyMeDnsBlackList != nullptr) {
											myKeyMeDnsBlackList->SetValue("Status", "1", RegistryValueKind::DWord); // Selected DNS blacklist
											myKeyMeDnsBlackList->Close();
										}
									}
								}
							}
							catch (...) {
							}
						}
						else {
							myKeyDnsRbl->SetValue("Enabled", "0", RegistryValueKind::DWord); 						// Enable DNS Blacklisting
						}
					}
					myKeyDnsRbl->Close();
				}

				if (url_rbl_status != nullptr) {
					if (url_rbl_status == "on") {
						if (me_url_black_list != nullptr) {
							try {
								Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
								RegistryKey ^myKeyRbl = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist", true);
								String ^bl = myKeyRbl->GetValue("Processing Order")->ToString();
								myKeyRbl->Close();
								array<String^> ^b_arr = bl->Split(',');
								for (int i = 0; i < b_arr->Length; i++) {
									RegistryKey ^myKeyMeUrlBlackList = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist\\" + b_arr[i], true);
									if (myKeyMeUrlBlackList != nullptr) {
										myKeyMeUrlBlackList->SetValue("SURBL Status", "0", RegistryValueKind::DWord);
										myKeyMeUrlBlackList->Close();
									}
								}
							}
							catch (...) {
							}

							try {
								array<String^> ^burl_arr = me_url_black_list->Split(',');
								for (int i = 0; i < burl_arr->Length; i++) {
									RegistryKey ^myKeyMeUrlBlackList = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist\\" + burl_arr[i], true);
									if (myKeyMeUrlBlackList != nullptr) {
										myKeyMeUrlBlackList->SetValue("SURBL Status", "1", RegistryValueKind::DWord); // Selected URL blacklist
										myKeyMeUrlBlackList->Close();
									}
								}
							}
							catch (...) {
							}
						}
					}
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailsrvmng: " + e->Message, 1804);
			}
		}

		void getMailEnableBlackLists()
		{
			try {
				Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
				RegistryKey ^myKeyRbl = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Connectors\\SMTP\\Reverse DNS Blacklist", false);
				String ^bl = myKeyRbl->GetValue("Processing Order")->ToString();
				myKeyRbl->Close();
				Console::WriteLine(bl);
			}
			catch (Exception ^e) {
				logger->debugLog("mailsrvmng: " + e->Message, 1804);
			}
		}

		void spamassassinSettings(String ^spam_status)
		{
			if (spam_status == "on") {
				try {
					Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
					RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Agents\\MTA", true);
					if (myKey != nullptr) {
						myKey->SetValue("Pickup Event Enabled", "1", RegistryValueKind::DWord);
						myKey->Close();
					}
				}
				catch (Exception ^e) {
					logger->debugLog("mailsrvmng: " + e->Message, 1804);
				}


			}
			else {
				try {
					Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
					RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Agents\\MTA", true);
					if (myKey != nullptr) {
						myKey->SetValue("Pickup Event Enabled", "0", RegistryValueKind::DWord);
						myKey->Close();
					}
				}
				catch (Exception ^e) {
					logger->debugLog("mailsrvmng: " + e->Message, 1804);
				}
			}
		}

		void spamassassinInit()
		{
			try {
				Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
				RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Wow6432Node\\Mail Enable\\Mail Enable\\Agents\\MTA", true);
				if (myKey != nullptr) {
					myKey->SetValue("Pickup Event Enabled", "1", RegistryValueKind::DWord);
					myKey->Close();
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailsrvmng: " + e->Message, 1804);
			}
		}

		void retrieveSettings()
		{

		}

		void saveSSL(String ^ssl_name, String ^certificate_path, String ^private_key_path)
		{

		}
	};

}
