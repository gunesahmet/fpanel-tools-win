#include "servicesmng.h"

using namespace System;
using namespace System::IO;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void servicesmng::Main(array<String^> ^Args) {
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;

		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		logger->debugLog(str, 999);

		Arguments ^main_case = gcnew Arguments(Args);

		String ^command = Args[0]->Replace("--", "");

		String ^service = main_case["service"];
		//createVariables();

		if (command == "status") {
			showStatus();
		}
		else {
			initService(service, command);
			System::Environment::Exit(0);
		}
	}

	void servicesmng::createVariables() {
		web = gcnew array<String^> {"W3SVC"};

		ftp = gcnew array<String^> {"FTPSVC"};

		database = gcnew array<String^> {"MySQLServer", "MSSQL$SQLEXPRESS", "PostgreSQL"};

		dns = gcnew array<String^> {"bind"};

		// Mail
		smtp = gcnew array<String^> {"MESMSCS", "MESMTPCS", "HMSTMP"};
		imap = gcnew array<String^> {"MEIMAPS", "HMIMAP"};
		pop = gcnew array<String^> {"MEPOPCS", "MEPOPS", "HMPOP"};

		spam = gcnew array<String^> {"SpamAssassin"};

		// Cron
		cron = gcnew array<String^> {"Schedule"};

		// Remote Desktop
		ssh = gcnew array<String^> {"TermService"};


		antivirus = gcnew array<String^> {"antivirus"};
	}

	void servicesmng::initService(String ^service, String ^command) {
		if (service == "BIND") {
			service = "Bind DNS Service";
		}
		if (service == "HMSMTP" || service == "HMIMAP") {
			service = "hmailserver";
		}
		try {
			if (command == "stop") {
				serv->stopService(service, false);
			}
			else if (command == "start") {
				serv->startService(service, false);
			}
			else if (command == "restart") {
				serv->stopService(service, true);
				serv->startService(service, false);
			}

		}
		catch (Exception ^e) {
			logger->debugLog("servicesmng:initService " + e->Message, 1802);
		}
	}

	void servicesmng::showStatus() {
		try {
			String ^iis_status = "off";

			String ^mysql_status = "off";
			String ^postgresql_status = "off";
			String ^mssql_status = "off";

			String ^bind_status = "off";
			String ^msdnsl_status = "off";

			String ^iis_ftp_status = "off";

			String ^mailenable_smtp_status = "off";
			String ^hmail_smtp_status = "off";

			String ^mailenable_imap_status = "off";
			String ^hmail_imap_status = "off";

			String ^task_scheduled_status = "off";

			String ^rdp_status = "off";

			String ^spam_status = "off";

			String ^clamav_status = "off";
			String ^amavis_status = "off";

			if (serv->serviceStatus("W3SVC") == true)
				iis_status = "on";

			if (serv->serviceStatus("MySQLServer") == true)
				mysql_status = "on";

			if (serv->serviceStatus("PostgreSQL") == true)
				postgresql_status = "on";

			if (serv->serviceStatus("MSSQL$SQLEXPRESS") == true)
				mssql_status = "on";

			if (serv->serviceStatus("FTPSVC") == true)
				iis_ftp_status = "on";

			if (serv->serviceStatus("MESMTPCS") == true)
				mailenable_smtp_status = "on";

			if (serv->serviceStatus("MEIMAPS") == true)
				mailenable_imap_status = "on";

			if (serv->serviceStatus("Schedule") == true)
				task_scheduled_status = "on";

			if (serv->serviceStatus("TermService") == true)
				rdp_status = "on";

			if (serv->serviceStatus("FPanel SpamAssassin Service") == true)
				spam_status = "on";

			if (serv->serviceStatus("ClamAv") == true)
				clamav_status = "on";

			if (serv->serviceStatus("Amavis") == true)
				amavis_status = "on";


			if (serv->serviceStatus("hMailServer") == true) {

				TcpClient ^tcpClient = gcnew TcpClient();
				try {
					try {
						tcpClient->Connect("127.0.0.1", 25);
						hmail_smtp_status = "on";
					}
					catch (...) {
						try {
							tcpClient->Connect("127.0.0.1", 587);
							hmail_smtp_status = "on";
						}
						catch (...) {
						}
					}
				}
				finally {
					delete tcpClient;
				}
				tcpClient = gcnew TcpClient();
				try {
					try {
						tcpClient->Connect("127.0.0.1", 143);
						hmail_imap_status = "on";
					}
					catch (...) {
					}
				}
				finally{
					delete tcpClient;
				}
			}

			if (serv->serviceStatus("Bind DNS Service") == true)
				bind_status = "on";

			String ^web_procs = "\"WEB\":{\"iis\":\"" + iis_status + "\"}";
			String ^database_procs = "\"DATABASE\":{\"MySQLServer\":\"" + mysql_status + "\",\"MSSQL$SQLEXPRESS\":\"" + mssql_status + "\",\"PostgreSQL\":\"" + postgresql_status + "\"}";
			String ^dns_procs = "\"DNS\":{\"BIND\":\"" + bind_status + "\",\"MicrosoftDNS\":\"" + msdnsl_status + "\"}";
			String ^ftp_procs = "\"FTP\":{\"FTPSVC\":\"" + iis_ftp_status + "\"}";
			String ^smtp_procs = "\"SMTP\":{\"MESMTPCS\":\"" + mailenable_smtp_status + "\",\"HMSMTP\":\"" + hmail_smtp_status + "\"}";
			String ^imap_procs = "\"IMAP\":{\"MEIMAPS\":\"" + mailenable_imap_status + "\",\"HMIMAP\":\"" + hmail_imap_status + "\"}";
			String ^cron_procs = "\"CRON\":{\"schedule\":\"" + task_scheduled_status + "\"}";
			String ^ssh_procs = "\"SSH\":{\"rdp\":\"" + rdp_status + "\"}";
			String ^spam_procs = "\"SPAM\":{\"SpamAssassin\":\"" + spam_status + "\"}";
			String ^antivirus_procs = "\"ANTIVIRUS\":{\"ClamAV\":\"" + clamav_status + "\",\"Amavis\":\"" + amavis_status + "\"}";

			String ^json = "{" + web_procs + "," + database_procs + "," + dns_procs + "," + ftp_procs + "," + smtp_procs + "," + imap_procs + "," + cron_procs + "," + ssh_procs + "," + spam_procs + "," + antivirus_procs + "}";
			Console::WriteLine(json);
		}
		catch (Exception ^e) {
			logger->debugLog("servicesmng: " + e->Message, 1803);
		}


	}
}

using namespace Tools;
int main(array<System::String ^> ^Args) {
	servicesmng ^mainProcess = gcnew servicesmng();
	mainProcess->Main(Args);
	return 0;
}