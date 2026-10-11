#pragma once
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Utils.h"
#include "../env/Services.h"

using namespace System;
using namespace Environments;
using namespace Microsoft::Win32;

namespace Mail
{
	private ref class Enable
	{
	private:
		static Environments::Env ^env;
		static Environments::FileLogger ^logger;
		static Environments::FUtils ^utils;
		String ^mail_queue_dir;
		String ^mail_queue_msg_dir;

	public:
		void setEnv(Environments::Env ^_env)
		{
			env = _env;
			utils = gcnew Environments::FUtils();
			mail_queue_dir = env->mail_dir + "Queues\\SMTP\\Outgoing\\";
			mail_queue_msg_dir = mail_queue_dir + "Messages";
		}

		void setLogger(Environments::FileLogger ^_logger)
		{
			logger = _logger;
		}

		void queue(int start, int end)
		{
			try {
				array<String^> ^queue_lists = System::IO::Directory::GetFiles(mail_queue_dir, "*.*", System::IO::SearchOption::AllDirectories);
				array<String^> ^total_queue_lists = System::IO::Directory::GetFiles(mail_queue_dir, "*.MAI", System::IO::SearchOption::AllDirectories);
				int total_mail = total_queue_lists->Length;
				Int64 start_index = 1;
				Int64 end_index = start_index + end;
				String ^json_data = nullptr;

				for each (String ^file in queue_lists) {
					if (start_index == end) {
						break;
					}

					if (start_index < start) {
						start_index = start_index + 1;
						continue;
					}

					String ^mail_id = file;
					FileInfo ^mail_file = gcnew FileInfo(file);
					FileInfo ^mail_msg_file = gcnew FileInfo(mail_queue_msg_dir + file);
					Int64 mail_size = mail_msg_file->Length;
					if (mail_size == 0)
						continue;
					String ^mail_sender = "";
					String ^mail_date = "";
					String ^mail_file_path = mail_file->ToString();
					String ^mail_age = calcAge(mail_file->ToString());
					String ^mail_recepient = "";
					String ^mail_subject = "";

					StreamReader ^sr = File::OpenText(file);
					try {
						array<String^> ^lines = File::ReadAllLines(file);
						for each (String ^line in lines)
						{
							if (Regex::IsMatch(line, "^Sender=", System::Text::RegularExpressions::RegexOptions::IgnoreCase)) {
								//
								mail_sender = line->Replace("Sender=[SMTP:", "");
								Regex ^rgx = gcnew Regex("\\]$");
								mail_sender = rgx->Replace(mail_sender, "");
								sr->Close();
								break;
							}

							if (Regex::IsMatch(line, "^TimeAcquired=", System::Text::RegularExpressions::RegexOptions::IgnoreCase)) {
								mail_date = line->Replace("TimeAcquired=", "");
							}
							if (Regex::IsMatch(line, "^Recipients=", System::Text::RegularExpressions::RegexOptions::IgnoreCase)) {
								mail_recepient = line->Replace("Recipients=", "");
							}
							if (Regex::IsMatch(line, "^Subject:", System::Text::RegularExpressions::RegexOptions::IgnoreCase)) {
								mail_subject = line->Replace("Subject:", "");
							}
						}
						sr->Close();
						delete sr;
					}
					catch (...) {
					}

					start_index = start_index + 1;
					if (String::IsNullOrEmpty(json_data)) {
						json_data = "[\"" + mail_id + "\", \"" + mail_size + "\",\"" + mail_date + "\",\"" + mail_sender + "\",\"" + mail_recepient + "\",\"" + mail_subject + "\",\"" + mail_age + "\"]";
					}
					else {
						json_data = json_data + ",[\"" + mail_id + "\", \"" + mail_size + "\",\"" + mail_date + "\",\"" + mail_sender + "\",\"" + mail_recepient + "\",\"" + mail_subject + "\",\"" + mail_age + "\"]";
					}
				}
				Console::WriteLine(json_data);
			}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1801);
			}
		}

		void clearQueue()
		{
			try {
				Environments::Services ^services = gcnew Environments::Services();
				services->stopService("MESMTPCS", true);
				array<String^> ^queue_lists = System::IO::Directory::GetFiles(mail_queue_dir, "*.*", System::IO::SearchOption::AllDirectories);
				for each (String ^file in queue_lists) {
					FileInfo ^mail_file = gcnew FileInfo(file);
					utils->removeFile(mail_file->ToString());
				}
				for each (auto subDir in (gcnew DirectoryInfo(mail_queue_msg_dir))->GetDirectories()) {
					utils->removeFolder(subDir->ToString(), true);
				}
			}
			catch (Exception ^e) {
				logger->debugLog("mailqueuemng: " + e->Message, 1802);
			}
		}

		void removeMessage(String ^file)
		{
			try {
				FileInfo ^mail_file = gcnew FileInfo(mail_queue_msg_dir + file);
				if (mail_file->Exists) {
					utils->removeFile(mail_file->ToString());
				}
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
