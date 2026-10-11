#pragma once
#include "Logger.h"
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class FtpSettings
		{
		public:
			void Set(String ^idleTimeout, String ^transferTimeout, String ^maxInstances, String ^maxConnections, String ^bannerMessage)
			{
				// TimeoutIdle
				try {
					ServerManager ^serverManagerFirst = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManagerFirst->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");
					ConfigurationElement ^ftpServerElement = siteDefaultsElement->GetChildElement("ftpServer");
					ConfigurationElement ^connectionsElement = ftpServerElement->GetChildElement("connections");
					ConfigurationElement ^messagesElement = ftpServerElement->GetChildElement("messages");

					connectionsElement["unauthenticatedTimeout"] = Convert::ToInt32(idleTimeout);
					connectionsElement["controlChannelTimeout"] = Convert::ToInt32(transferTimeout);
					connectionsElement["dataChannelTimeout"] = Convert::ToInt32(maxInstances);
					connectionsElement["maxConnections"] = Convert::ToInt64(maxConnections);
					messagesElement["bannerMessage"] = bannerMessage;

					serverManagerFirst->CommitChanges();
					delete serverManagerFirst;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FtpSettings::Set. Error: " + e->Message, 1105);
				}
			}

			void Get()
			{
				String ^bannerMessage = "";
				String ^maxConnections = "";
				String ^dataChannelTimeout = "";
				String ^transferTimeout = "";
				String ^idleTimeout = "";

				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");
					ConfigurationElement ^ftpServerElement = siteDefaultsElement->GetChildElement("ftpServer");
					ConfigurationElement ^messagesElement = ftpServerElement->GetChildElement("messages");
					ConfigurationElement ^connectionsElement = ftpServerElement->GetChildElement("connections");
					
					bannerMessage = messagesElement->GetAttributeValue("bannerMessage")->ToString();
					maxConnections = connectionsElement->GetAttributeValue("maxConnections")->ToString();
					dataChannelTimeout = connectionsElement->GetAttributeValue("dataChannelTimeout")->ToString();
					transferTimeout = connectionsElement->GetAttributeValue("controlChannelTimeout")->ToString();
					idleTimeout = connectionsElement->GetAttributeValue("unauthenticatedTimeout")->ToString();

					serverManager->CommitChanges();
					String ^jSon = "{\"timeout_idle\":\"" + idleTimeout + "\", \"timeout_no_transfer\":\"" + transferTimeout + "\", \"maxinstances\":\"" + dataChannelTimeout + "\", \"max_clients\":\"" + maxConnections + "\",\"welcome_message\": \"" + bannerMessage + "\"}";
					Console::WriteLine(jSon);
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FtpSettings::Get. Error: " + e->Message, 1105);
				}
			}

			void Sessions()
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default FTP Site");
					if (siteElement == nullptr) {
						Logger::errorLog("Error FtpSettings::Sessions. Error: FTP Site doesnt exists", 1105);
						Console::WriteLine("{}");
						return;
					}
					else {
						ConfigurationElement ^ftpServerElement = siteElement->GetChildElement("ftpServer");
						if (ftpServerElement == nullptr) {
							Console::WriteLine("{}");
							return;
						}
						else {
							ConfigurationElementCollection ^sessionsCollections = ftpServerElement->GetChildElement("sessions")->GetCollection();

							if (sessionsCollections->Count == 0) {
								serverManager->CommitChanges();
								Console::WriteLine("{}");
							}
							else {
								DateTime current_time = DateTime::Now;
								String ^sess_id = "";
								String ^username = "";
								String ^idle_time = "";
								String ^logged_time = "";
								String ^json = nullptr;

								for each (ConfigurationElement ^sessionsCollection in sessionsCollections) {
									sess_id = sessionsCollection->Attributes["sessionId"]->Value->ToString();
									username = sessionsCollection->Attributes["userName"]->Value->ToString()->Replace("\\", "\\\\");
									logged_time = sessionsCollection->Attributes["sessionStartTime"]->Value->ToString()->Replace("Z", "")->Replace("T", " ");
									idle_time = sessionsCollection->Attributes["commandStartTime"]->Value->ToString()->Replace("Z", "")->Replace("T", " ");

									DateTime logged_time_date = DateTime::ParseExact(logged_time, "yyyy-MM-dd HH:mm:ss.fff", System::Globalization::CultureInfo::InvariantCulture);
									DateTime idle_time_date = DateTime::ParseExact(idle_time, "yyyy-MM-dd HH:mm:ss.fff", System::Globalization::CultureInfo::InvariantCulture);

									if (json == nullptr) {
										json = "[\"" + sess_id + "\",\"" + username + "\",\"" + logged_time_date + "\",\"" + idle_time_date + "\"]";
									}
									else {
										json = json + ",[\"" + sess_id + "\",\"" + username + "\",\"" + logged_time_date + "\",\"" + idle_time_date + "\"]";
									}
								}

								if (json == nullptr) {
									serverManager->CommitChanges();
									json = "{}";
									Console::WriteLine(json);
								}
								else {
									serverManager->CommitChanges();
									Console::WriteLine("[" + json + "]");
								}
							}
						}
					}
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FtpSettings::Sessions. Error: " + e->Message, 1105);
				}
			}

			void kill(String ^_sess_id)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default FTP Site");
					if (siteElement == nullptr) {
						Logger::errorLog("Error FtpSettings::Sessions. Error: FTP Site doesnt exists", 1105);
						return;
					}
					else {
						ConfigurationElement ^ftpServerElement = siteElement->GetChildElement("ftpServer");
						ConfigurationElementCollection ^sessionsElement = ftpServerElement->GetChildElement("sessions")->GetCollection();
						for each (ConfigurationElement ^sessionElement in sessionsElement) {
							String ^sess_id = sessionElement->Attributes["sessionId"]->Value->ToString();
							if (sess_id == _sess_id) {
								ConfigurationMethod ^Terminate = sessionElement->Methods["Terminate"];
								Terminate->CreateInstance()->Execute();
							}
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error FtpSettings::kill. Error: " + e->Message, 1105);
				}
			}
			FtpSettings()
			{
				InitializeInstanceFields();
			}
		private:
			static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
			{
				for each (ConfigurationElement ^element in collection)
				{
					if (String::Equals(element->ElementTagName, elementTagName, StringComparison::OrdinalIgnoreCase)) {
						bool matches = true;

						for (int i = 0; i < keyValues->Length; i += 2) {
							Object ^o = element->GetAttributeValue(keyValues[i]);
							String ^value = nullptr;
							if (o != nullptr) {
								value = o->ToString();
							}

							if (!String::Equals(value, keyValues[i + 1], StringComparison::OrdinalIgnoreCase)) {
								matches = false;
								break;
							}
						}
						if (matches) {
							return element;
						}
					}
				}
				return nullptr;
			}
			
			void InitializeInstanceFields()
			{
				String ^file = "C:\\Windows\\System32\\InetSrv\\Microsoft.Web.Administration.dll";
				{
					if (!File::Exists(file)) {
						Console::WriteLine("IIS not installed on this server.");
						System::Environment::Exit(1);
					}
				}
			}

		};
	}
}