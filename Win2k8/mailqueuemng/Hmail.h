#pragma once
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Utils.h"

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
		static Environments::FUtils ^utils;

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

		void queue(int start, int end)
		{
			String ^output;
			//try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				array<String^> ^lines = hMailApp->Status->UndeliveredMessages->Split('\n');
				for (int i = 0; i < lines->Length; ++i) {
					array<String^> ^message_array = lines[i]->Split('\t');
					if (message_array->Length <= 1) {
						continue;
					}
					String ^file = message_array[5];
					FileInfo ^mail_msg_file = gcnew FileInfo(file);
					Int64 mail_size = mail_msg_file->Length;
					String ^mail_subject;

					StreamReader ^sr = File::OpenText(file);
					try {
						array<String^> ^lines = File::ReadAllLines(file);
						for each (String ^line in lines) {
							if (Regex::IsMatch(line, "^Subject:", System::Text::RegularExpressions::RegexOptions::IgnoreCase)) {
								mail_subject = line->Replace("Subject:", "");
							}
						}
						sr->Close();
						delete sr;
					}
					catch (...) {
					}


					String ^msg_json = "{";
					msg_json += "\"queue_id\":\"" + message_array[0] + "\","; // ID
					msg_json += "\"arrival_time\":\"" + message_array[1]  + "\","; // Created Date
					msg_json += "\"queue_name\":\"" + message_array[2] + "\","; // From
					msg_json += "\"sender\":\"" + message_array[2]  + "\","; // From
					msg_json += "\"subject\":\"" + mail_subject + "\","; // subject
					msg_json += "\"message_size\":\"" + Convert::ToString(mail_size)  + "\","; // message_size
					msg_json += "\"recipients\":\"" + message_array[3]  + "\""; // To ???

					msg_json += "}";

					// 5 => FileName

					if (String::IsNullOrEmpty(output)) {
						output = msg_json;
					}
					else {
						output += "," + msg_json;
					}
				}
	/*		}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1802);
			}*/
			Console::WriteLine("[" + output + "]");
		}

		void clearQueue()
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				array<String^> ^lines = hMailApp->Status->UndeliveredMessages->Split('\n');
				hMailApp->GlobalObjects->DeliveryQueue->Clear();
			}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1802);
			}
		}

		void removeMessage(String ^mail_id)
		{
			try {
				hMailServer::Application ^hMailApp = gcnew hMailServer::ApplicationClass();
				if (hMailApp != nullptr)
					hMailApp->Authenticate("Administrator", nullptr);
				else {
					logger->errorLog("Mail server auth failed", 1716);
					return;
				}
				hMailApp->GlobalObjects->DeliveryQueue->Remove(Convert::ToInt32(mail_id));
			}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1803);
			}
		}

		String ^calcAge(String ^file)
		{
			try {
				String ^age = "";

				FileInfo ^fi = gcnew FileInfo(file);
				System::DateTime created_date = fi->CreationTime;

				DateTime current_date = DateTime::Now;
				TimeSpan span = current_date.Subtract(created_date);
				age = span.Days + " d " + span.Hours + " h " + span.Minutes + " m " + span.Seconds + " s";
				return age;
			}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1804);
				return "";
			}
		}

	};

}
