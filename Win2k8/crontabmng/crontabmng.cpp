#include "crontabmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{

	void crontabmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);


		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");
		String ^minute = main_case["minute"];
		String ^hour = main_case["hour"];
		String ^day = main_case["day"];
		String ^week = main_case["week"];
		String ^month = main_case["month"];
		String ^job = main_case["job"];
		String ^timestamp = main_case["timestamp"];
		String ^old_timestamp = main_case["old-timestamp"];
		String ^passwd = main_case["passwd"];
		String ^user = main_case["user"];
		String ^new_user = main_case["new-user"];
		String ^domain = main_case["domain"];
		String ^password = main_case["password"];
		String ^start_date = dateTime.ToString("d");
		array<String^> ^arr = utils->split(start_date, gcnew array<Char> {'/'});
		String ^startDateDay = arr[1];
		String ^startDateMonth = arr[0];
		String ^startDateYear = arr[2];
		Int16 startDateDayInt = Convert::ToInt16(startDateDay);
		if (startDateDayInt < 10) {
			startDateDay = "0" + startDateDay;
		}
		Int16 startDateMonthInt = Convert::ToInt16(startDateMonth);
		if (startDateMonthInt < 10) {
			startDateMonth = "0" + startDateMonth;
		}

		start_date = startDateMonth + "/" + startDateDay + "/" + startDateYear;

		if (process == "init") {
			//TODO: Yazýlacak gereklimi kontrol edilecek
		}
		else if (process == "create") {
			createCronJob(minute, hour, day, week, month, timestamp, user, start_date, job, domain, password);
		}
		else if (process == "remove") {
			removeCronJob(user, timestamp);
		}
		else if (process == "remove-perm") {
			removePermCronJob(user);
		}
		else if (process == "test") {
			test();
		}
		else {
			System::Environment::Exit(0);
		}

	}

	void crontabmng::createCronJob(String ^minute, String ^hour, String ^day, String ^week, String ^month, String ^timestamp, String ^username, String ^start_date, String ^job, String ^domain, String ^password)
	{
		job = utils->exec(env->fcrypt, "-d --in=\"" + job + "\"");
		job = utils->Trim(job);

		password = utils->exec(env->fcrypt, "-d --in=\"" + password + "\"");
		password = utils->Trim(password);
		try {
			if (String::IsNullOrEmpty(minute) || String::IsNullOrEmpty(job) || String::IsNullOrEmpty(timestamp) || String::IsNullOrEmpty(username)) {
				logger->errorLog("Crontab - Username, Job or Minute is null", 3701);
				System::Environment::Exit(3701);
			}

			if (username == "root") {
				username = "Administrator";
			}

			if (validate->checkUser(username) == false) {
				logger->errorLog("Username not found", 3701);
				System::Environment::Exit(3701);
			}

			String ^success_message = username + " cronjob added for " + domain;

			if (String::IsNullOrEmpty(hour))
				hour = "*";

			if (String::IsNullOrEmpty(day))
				day = "*";

			if (String::IsNullOrEmpty(week))
				week = "*";

			if (String::IsNullOrEmpty(month))
				month = "*";

			if (!String::IsNullOrEmpty(minute)) {
				minute = minute->Replace("*", "")->Replace("/", "");
				if (Convert::ToInt32(minute) < 10)
					minute = " 0" + minute;
			}
			else {
				minute = "1";
			}


			if (hour != "*") {
				hour = hour->Replace("*", "")->Replace("/", "");
				if (Convert::ToInt32(hour) < 10)
					hour = " 0" + hour;
			}

			if (day != "*")
				day = day->Replace("*", "")->Replace("/", "");

			if (week != "*")
				week = week->Replace("*", "")->Replace("/", "");


			if (month != "*")
				month = month->Replace("*", "")->Replace("/", "");

			timestamp = " /tn " + timestamp + "." + username;
			username = " /it /ru " + username + " /rp " + password + " ";
			String ^startdate = " /sd " + start_date;
			job = " /tr \"" + job + "\"";

			String ^stats;
			if (month == "*") {
				if (week == "*") {
					if (day == "*") {
						if (hour == "*") {
							logger->debugLog("schtasks /create " + timestamp + job + startdate + username + " /sc minute /mo " + minute + " /f", 999);
							stats = utils->exec("schtasks", " /create " + timestamp + job + startdate + username + " /sc minute /mo " + minute + " /f");
						}
						else {
							stats = utils->exec("schtasks", " /create " + timestamp + job + startdate + username + " /sc hourly /mo " + hour + " /st " + "00:" + hour + ":00" + " /f");
						}
					}
					else {
						stats = utils->exec("schtasks", " /create " + timestamp + job + startdate + username + " /sc daily /mo " + day + " /st " + hour + ":" + minute + ":00" + " /f");
					}
				}
				else {
					stats = utils->exec("schtasks", " /create " + timestamp + job + startdate + username + " /sc weekly /mo " + week + " /d " + day + " /st " + hour + ":" + minute + ":00" + " /f");
				}
			}
			else {
				stats = utils->exec("schtasks", " /create " + timestamp + job + startdate + username + " /sc monthly /mo " + month + " /d " + day + " /st " + hour + ":" + minute + ":00" + " /f");
			}
			logger->debugLog("schtasks /create " + timestamp + job + startdate + username + " /sc monthly /mo " + month + " /d " + day + " /st " + hour + ":" + minute + ":00" + " /f", 9999);
			logger->infoLog(success_message, 3701);

		}
		catch (Exception ^e) {
			logger->debugLog("crontabmng:" + e->Message, 3704);
		}

	}

	void crontabmng::removeCronJob(String ^username, String ^timestamp) {
		try {
			if (String::IsNullOrEmpty(username) || String::IsNullOrEmpty(timestamp)) {
				logger->errorLog("Username or timestamp is null", 3701);
				System::Environment::Exit(3701);
			}

			if (username == "root") {
				username = "Administrator";
			}

			utils->exec("schtasks", "/delete /tn " + timestamp + "." + username + " /f");
			logger->infoLog(username + " cronjob removed", 3702);
		}
		catch (Exception ^e) {
			logger->debugLog("crontabmng:" + e->Message, 3705);
		}
	}

	void crontabmng::removePermCronJob(String ^username) {
		try {
			if (String::IsNullOrEmpty(username)) {
				logger->errorLog("Username is null", 3701);
				System::Environment::Exit(3701);
			}

			if (username == "root") {
				username = "Administrator";
			}

			String ^timestamp_list = utils->exec("schtasks", "findstr \"" + username + "\"");
			array<String^> ^cronjobs = timestamp_list->Split(' ');
			for each (String ^crons in cronjobs) {
				String ^cron = crons->Trim();
				if (!String::IsNullOrEmpty(cron))
					utils->exec("schtasks", "/delete /tn " + cron + " /f");
			}
			logger->infoLog(username + "'s all cronjob removed", 3703);
		}
		catch (Exception ^e) {
			logger->debugLog("crontabmng:" + e->Message, 3706);
		}
	}

	void crontabmng::test() {
		System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();
		pProcess->StartInfo->CreateNoWindow = true;

		pProcess->StartInfo->RedirectStandardOutput = true;
		pProcess->StartInfo->RedirectStandardInput = true;
		pProcess->StartInfo->WindowStyle = ProcessWindowStyle::Hidden;

		pProcess->StartInfo->UseShellExecute = false;

		String ^strCommand = "cmd";
		pProcess->StartInfo->FileName = strCommand;

		pProcess->StartInfo->Arguments = "runas /user:kowengo k\" SC \" + command";

		pProcess->StartInfo->EnvironmentVariables->Values;

		pProcess->EnableRaisingEvents = false;
		pProcess->Start();

		String ^output;
		StreamReader ^streamReader = pProcess->StandardOutput;
		try {
			output = streamReader->ReadToEnd();
		}
		catch (...) {
		}
		finally {
			delete streamReader;
		}
		pProcess->WaitForExit();
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args) {
	crontabmng ^bckup = gcnew crontabmng();
	bckup->Main(Args);
	return 0;
}