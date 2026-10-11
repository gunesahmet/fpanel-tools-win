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

		void initSmtp(String ^max_smtp_threads, String ^max_msg_size, String ^smtp_no_tries, String ^smtp_tries_min, String ^smtp_relayer, String ^smtp_relayer_port,
			String ^smtp_relayer_auth, String ^smtp_relayer_user, String ^smtp_relayer_passwd, String ^smtp_relayer_security, String ^smtp_auth_plain,
			String ^allow_empty_sender, String ^allow_invalid_end, String ^dis_invalid_client, String ^max_inv_command, String ^delivery_bind_to_ip,
			String ^max_rec_batch, String ^add_del_head, String ^rule_loop_limit, String ^max_mx_host, String ^hostname,
			String ^max_pop_threads, String ^max_imap_threads, String ^sub_port_status, String ^sub_port, String ^sub_port_auth_mode, String ^sub_ssl_cert
		)
		{
			/* PROTOCOLS */
			/* SMTP */
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}

				//// GENERAL
				hMailApp->Settings->MaxSMTPConnections = Convert::ToInt32(max_smtp_threads); // Connections Max number of simultaneous conn
				hMailApp->Settings->MaxMessageSize = Convert::ToInt32(max_msg_size);

				// Delivery of e-mail
				hMailApp->Settings->SMTPNoOfTries = Convert::ToInt32(smtp_no_tries);
				hMailApp->Settings->SMTPMinutesBetweenTry = Convert::ToInt32(smtp_tries_min);
				hMailApp->Settings->HostName = hostname;

				hMailApp->Settings->SMTPRelayer = smtp_relayer;
				hMailApp->Settings->SMTPRelayerPort = Convert::ToInt32(smtp_relayer_port);
				if (smtp_relayer_auth == "on") {
					hMailApp->Settings->SMTPRelayerRequiresAuthentication = true;
					hMailApp->Settings->SMTPRelayerUsername = smtp_relayer_user;
					hMailApp->Settings->SetSMTPRelayerPassword(smtp_relayer_passwd);

					if(String::Equals(smtp_relayer_security, "starttls_opt"))
						hMailApp->Settings->SMTPRelayerConnectionSecurity = eConnectionSecurity::eCSSTARTTLSOptional;
					else if(String::Equals(smtp_relayer_security, "starttls_req"))
						hMailApp->Settings->SMTPRelayerConnectionSecurity = eConnectionSecurity::eCSSTARTTLSRequired;
					else if(String::Equals(smtp_relayer_security, "ssl"))
						hMailApp->Settings->SMTPRelayerConnectionSecurity = eConnectionSecurity::eCSTLS;
					else
						hMailApp->Settings->SMTPRelayerConnectionSecurity = eConnectionSecurity::eCSNone;

				}
				else {
					hMailApp->Settings->SMTPRelayerRequiresAuthentication = false;
				}

				// RFC COMPLIANCE
				// Auth plain text auth
				if (smtp_auth_plain == "on")
					hMailApp->Settings->AllowSMTPAuthPlain = true;
				else
					hMailApp->Settings->AllowSMTPAuthPlain = false;

				// Allow empty sender address
				if (allow_empty_sender == "on")
					hMailApp->Settings->DenyMailFromNull = false;
				else
					hMailApp->Settings->DenyMailFromNull = true;

				// Allow Incorrectly formatted line endings
				if (allow_invalid_end == "on")
					hMailApp->Settings->AllowIncorrectLineEndings = true;
				else
					hMailApp->Settings->AllowIncorrectLineEndings = false;

				// Disconnect client after to many invalid commands
				if (dis_invalid_client == "on") {
					hMailApp->Settings->DisconnectInvalidClients = true;
					hMailApp->Settings->MaxNumberOfInvalidCommands = Convert::ToInt32(max_inv_command);
				}
				else
					hMailApp->Settings->DisconnectInvalidClients = false;

				// ADVANCED
				hMailApp->Settings->SMTPDeliveryBindToIP = delivery_bind_to_ip;
				hMailApp->Settings->MaxSMTPRecipientsInBatch = Convert::ToInt32(max_rec_batch);
				//hMailApp->Settings->SMTPRelayerUseSSL = true; // ???
				if (add_del_head == "on") {
					hMailApp->Settings->AddDeliveredToHeader = true;
				}
				else
					hMailApp->Settings->AddDeliveredToHeader = false;

				hMailApp->Settings->RuleLoopLimit = Convert::ToInt32(rule_loop_limit);
				hMailApp->Settings->MaxNumberOfMXHosts = Convert::ToInt32(max_mx_host);

				hMailApp->Settings->MaxPOP3Connections = Convert::ToInt32(max_pop_threads);
				hMailApp->Settings->MaxIMAPConnections = Convert::ToInt32(max_imap_threads);
				bool found_sub_port = false;
				int sub_port_id;
				for (int i = 0; i < hMailApp->Settings->TCPIPPorts->Count; i++) {
					if ((hMailApp->Settings->TCPIPPorts[i]->Protocol == eSessionType::eSTSMTP) && (hMailApp->Settings->TCPIPPorts[i]->PortNumber != 25)) {
						sub_port_id = hMailApp->Settings->TCPIPPorts[i]->ID;
						found_sub_port = true;
						break;
					}
				}

				if (sub_port_status == "on") {
					if (found_sub_port) {
						hMailApp->Settings->TCPIPPorts->DeleteByDBID(sub_port_id);

						//if (sub_port_auth_mode != "none") {
						//	if (String::Equals(sub_port_auth_mode, "starttls_opt"))
						//		tcp_ip_port->ConnectionSecurity = eConnectionSecurity::eCSSTARTTLSOptional;
						//	else if (String::Equals(sub_port_auth_mode, "starttls_req"))
						//		tcp_ip_port->ConnectionSecurity = eConnectionSecurity::eCSSTARTTLSRequired;
						//	else if (String::Equals(sub_port_auth_mode, "ssl"))
						//		tcp_ip_port->ConnectionSecurity = eConnectionSecurity::eCSTLS;

						//	tcp_ip_port->UseSSL = false;
						//	// SSL kaydet
						//	//tcp_ip_port->SSLCertificateID = sub_ssl_cert;
						//}
						//else
					}

					TCPIPPort ^tcp_ip_port = hMailApp->Settings->TCPIPPorts->Add();
					tcp_ip_port->Protocol = eSessionType::eSTSMTP;
					tcp_ip_port->Address = "0.0.0.0";
					tcp_ip_port->PortNumber = Convert::ToInt32(sub_port);
					tcp_ip_port->ConnectionSecurity = eConnectionSecurity::eCSNone;
					tcp_ip_port->UseSSL = false;
					tcp_ip_port->Save();
				}
				else {
					hMailApp->Settings->TCPIPPorts->ItemByDBID[sub_port_id]->Delete();
				}
				Environments::FUtils ^utils = gcnew Environments::FUtils();
				utils->exec(env->servicesmng, "--mail --proc=restart");
			}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
		}


		void initAntiSpam(String ^spam_mark_threshold, String ^add_header_spam,
			String ^add_reason_spam, String ^add_msg_subj, String ^add_msg_subj_txt, String ^spam_del_threshold, String ^spam_max_msg_size, 
			String ^use_spf, String ^use_spf_score, String ^check_host_helo, String ^check_host_helo_score, String ^use_mx_checks, String ^use_mx_checks_score,
			String ^verify_dkim, String ^verify_dkim_score
		)
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}


				hMailApp->Settings->AntiSpam->SpamMarkThreshold = Convert::ToInt32(spam_mark_threshold);

				if (add_header_spam == "on")
					hMailApp->Settings->AntiSpam->AddHeaderSpam = true;
				else
					hMailApp->Settings->AntiSpam->AddHeaderSpam = false;


				if (add_reason_spam == "on")
					hMailApp->Settings->AntiSpam->AddHeaderReason = true;
				else
					hMailApp->Settings->AntiSpam->AddHeaderReason = false;


				if (add_msg_subj == "on") {
					hMailApp->Settings->AntiSpam->PrependSubject = true;
					hMailApp->Settings->AntiSpam->PrependSubjectText = add_msg_subj_txt;
				}
				else
					hMailApp->Settings->AntiSpam->PrependSubject = false;

				hMailApp->Settings->AntiSpam->SpamDeleteThreshold = Convert::ToInt32(spam_del_threshold);
				hMailApp->Settings->AntiSpam->MaximumMessageSize = Convert::ToInt32(spam_max_msg_size);

				if (use_spf == "on") {
					hMailApp->Settings->AntiSpam->UseSPF = true;
					hMailApp->Settings->AntiSpam->UseSPFScore = Convert::ToInt32(use_spf_score);
				}
				else
					hMailApp->Settings->AntiSpam->UseSPF = false;


				if (check_host_helo == "on") {
					hMailApp->Settings->AntiSpam->CheckHostInHelo = true;
					hMailApp->Settings->AntiSpam->CheckHostInHeloScore = Convert::ToInt32(check_host_helo_score);
				}
				else
					hMailApp->Settings->AntiSpam->CheckHostInHelo = false;


				if (use_mx_checks == "on") {
					hMailApp->Settings->AntiSpam->UseMXChecks = true;
					hMailApp->Settings->AntiSpam->UseMXChecksScore = Convert::ToInt32(use_mx_checks_score);
				}
				else
					hMailApp->Settings->AntiSpam->UseMXChecks = false;


				if (verify_dkim == "on") {
					hMailApp->Settings->AntiSpam->DKIMVerificationEnabled = true;
					hMailApp->Settings->AntiSpam->DKIMVerificationFailureScore = Convert::ToInt32(verify_dkim_score);
				}
				else
					hMailApp->Settings->AntiSpam->DKIMVerificationEnabled = false;
			}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
		}

		void getHMailBlackLists()
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				String ^lists;
				DNSBlackLists ^dns_black_lists = hMailApp->Settings->AntiSpam->DNSBlackLists;
				int total_dns = dns_black_lists->Count;

				for (int i = 0; i < total_dns; i++) {
					String ^host;
					String ^id;
					String ^status;
					String ^expected_result;
					String ^reject_msg;
					String ^score;
					if (i == 0) {
						host = dns_black_lists->default[i]->DNSHost;
						id = Convert::ToString(dns_black_lists->default[i]->ID);
						status = Convert::ToString(dns_black_lists->default[i]->Active);
						expected_result = dns_black_lists->default[i]->ExpectedResult;
						reject_msg = dns_black_lists->default[i]->RejectMessage;
						score = Convert::ToString(dns_black_lists->default[i]->Score);
					}
					else {
						host = dns_black_lists->ItemByDBID[i]->DNSHost;
						id = Convert::ToString(dns_black_lists->ItemByDBID[i]->ID);
						status = Convert::ToString(dns_black_lists->ItemByDBID[i]->Active);
						expected_result = dns_black_lists->ItemByDBID[i]->ExpectedResult;
						reject_msg = dns_black_lists->ItemByDBID[i]->RejectMessage;
						score = Convert::ToString(dns_black_lists->ItemByDBID[i]->Score);
					}
		

					String ^json = "{" + 
						"\"host\":\""+ host + "\"," +
						"\"id\":\""+ id + "\"," +
						"\"status\":\""+ status + "\"," +
						"\"expected_result\":\""+ expected_result + "\"," +
						"\"reject_msg\":\""+ reject_msg + "\"," +
						"\"score\":\""+ score + "\"" +
						"}";

					if (String::IsNullOrEmpty(lists)) {
						lists = json;
					}
					else {
						lists += "," + json;
					}
				}

				Console::WriteLine("[" + lists + "]");
			}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
		}

		void spamassassinInit()
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				hMailApp->Settings->AntiSpam->SpamAssassinEnabled = true;
			}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
		}

		void spamassassinSettings(String ^spam_status)
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				if (spam_status == "on") {
					hMailApp->Settings->AntiSpam->SpamAssassinEnabled = true;
				}
				else {
					hMailApp->Settings->AntiSpam->SpamAssassinEnabled = false;
				}
			}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}
		}

		void retrieveSettings()
		{
			//try {

				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}

				String ^json;
				int sub_port_id;
				bool sub_port_status = false;
				for (int i = 0; i < hMailApp->Settings->TCPIPPorts->Count; i++) {
					if ((hMailApp->Settings->TCPIPPorts[i]->Protocol == eSessionType::eSTSMTP) && (hMailApp->Settings->TCPIPPorts[i]->PortNumber != 25)) {
						sub_port_id = hMailApp->Settings->TCPIPPorts[i]->ID;
						sub_port_status = true;
						break;
					}
				}

				String ^sub_port;
				sub_port = Convert::ToString(hMailApp->Settings->TCPIPPorts->ItemByDBID[sub_port_id]->PortNumber);

				json += "{";
				
				json += "\"max_smtp_threads\":\"" + Convert::ToString(hMailApp->Settings->MaxSMTPConnections) + "\",";
				json += "\"max_msg_size\":\"" + Convert::ToString(hMailApp->Settings->MaxMessageSize / 1024) + "\",";
				
				json += "\"sub_port_status\":\"" + (sub_port_status ? "on" : "off") + "\",";
				json += "\"sub_port\":\"" + sub_port + "\",";
				
				json += "\"smtp_no_tries\":\"" + Convert::ToString(hMailApp->Settings->SMTPNoOfTries) + "\",";
				json += "\"smtp_tries_min\":\"" + Convert::ToString(hMailApp->Settings->SMTPMinutesBetweenTry) + "\",";
				json += "\"hostname\":\"" + hMailApp->Settings->HostName + "\",";
				
				json += "\"smtp_relayer\":\"" + hMailApp->Settings->SMTPRelayer + "\",";
				json += "\"smtp_relayer_port\":\"" + Convert::ToString(hMailApp->Settings->SMTPRelayerPort) + "\",";
				json += "\"smtp_relayer_auth\":\"" + (hMailApp->Settings->SMTPRelayerRequiresAuthentication ? "on" : "off") + "\",";
				json += "\"smtp_relayer_user\":\"" + hMailApp->Settings->SMTPRelayerUsername + "\",";
				json += "\"smtp_relayer_security\":\"" + hMailApp->Settings->SMTPRelayerConnectionSecurity.ToString() + "\",";
				
				json += "\"smtp_auth_plain\":\"" + (hMailApp->Settings->AllowSMTPAuthPlain ? "on" : "off") + "\",";
				json += "\"allow_empty_sender\":\"" + (hMailApp->Settings->DenyMailFromNull ? "off" : "on") + "\",";
				json += "\"allow_invalid_end\":\"" + (hMailApp->Settings->AllowIncorrectLineEndings ? "on" : "off") + "\",";
				json += "\"dis_invalid_client\":\"" + (hMailApp->Settings->DisconnectInvalidClients ? "on" : "off") + "\",";
				json += "\"max_inv_command\":\"" + hMailApp->Settings->MaxNumberOfInvalidCommands.ToString() + "\",";
				json += "\"delivery_bind_to_ip\":\"" + hMailApp->Settings->SMTPDeliveryBindToIP + "\",";
				json += "\"max_rec_batch\":\"" + hMailApp->Settings->MaxSMTPRecipientsInBatch.ToString() + "\",";
				json += "\"add_del_head\":\"" + (hMailApp->Settings->AddDeliveredToHeader ? "on" : "off") + "\",";
				json += "\"rule_loop_limit\":\"" + hMailApp->Settings->RuleLoopLimit.ToString() + "\",";
				json += "\"max_mx_host\":\"" + hMailApp->Settings->MaxNumberOfMXHosts.ToString() + "\",";
				
				json += "\"max_pop_threads\":\"" + hMailApp->Settings->MaxPOP3Connections.ToString() + "\",";
				json += "\"max_imap_threads\":\"" + hMailApp->Settings->MaxIMAPConnections.ToString() + "\",";

				json += "\"spam_mark_threshold\":\"" + hMailApp->Settings->AntiSpam->SpamMarkThreshold.ToString() + "\",";

				json += "\"add_header_spam\":\"" + (hMailApp->Settings->AntiSpam->AddHeaderSpam ? "on" : "off") + "\",";
				json += "\"add_reason_spam\":\"" + (hMailApp->Settings->AntiSpam->AddHeaderReason ? "on" : "off") + "\",";
				json += "\"add_msg_subj\":\"" + (hMailApp->Settings->AntiSpam->PrependSubject ? "on" : "off") + "\",";
				json += "\"add_msg_subj_txt\":\"" + hMailApp->Settings->AntiSpam->PrependSubjectText + "\",";

				json += "\"spam_del_threshold\":\"" + hMailApp->Settings->AntiSpam->SpamDeleteThreshold.ToString() + "\",";
				json += "\"spam_max_msg_size\":\"" + (hMailApp->Settings->AntiSpam->MaximumMessageSize / 1024).ToString() + "\",";

				json += "\"use_spf\":\"" + (hMailApp->Settings->AntiSpam->UseSPF ? "on" : "off") + "\",";
				json += "\"use_spf_score\":\"" + hMailApp->Settings->AntiSpam->UseSPFScore.ToString() + "\",";

				json += "\"check_host_helo\":\"" + (hMailApp->Settings->AntiSpam->CheckHostInHelo ? "on" : "off") + "\",";
				json += "\"check_host_helo_score\":\"" + hMailApp->Settings->AntiSpam->CheckHostInHeloScore.ToString() + "\",";

				json += "\"use_mx_checks\":\"" + (hMailApp->Settings->AntiSpam->UseMXChecks ? "on" : "off") + "\",";
				json += "\"use_mx_checks_score\":\"" + hMailApp->Settings->AntiSpam->UseMXChecksScore.ToString() + "\",";

				json += "\"verify_dkim\":\"" + (hMailApp->Settings->AntiSpam->DKIMVerificationEnabled ? "on" : "off") + "\",";
				json += "\"verify_dkim_score\":\"" + hMailApp->Settings->AntiSpam->DKIMVerificationFailureScore.ToString() + "\"";
				json += "}";
				Console::WriteLine(json);
				Environments::FUtils ^utils = gcnew Environments::FUtils();
				utils->exec(env->servicesmng, "--mail --proc=stop");
		/*	}
			catch (Exception ^e) {
				logger->errorLog("mailsrvmng: " + e->Message, 1716);
				System::Environment::Exit(1);
			}*/
		}

		void saveSSL(String ^ssl_name, String ^certificate_path, String ^private_key_path)
		{

		}
	};

}
