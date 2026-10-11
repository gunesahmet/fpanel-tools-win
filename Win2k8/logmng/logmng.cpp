#include "logmng.h"

using namespace System;
using namespace System::IO;

namespace Tools
{

	void logmng::Main(array<String^> ^Args)
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
		domain = main_case["domain"];
		clean_err = main_case["clean-err"];
		compress = main_case["comp"];
		day = main_case["day"];

		if (process == "init") {
			init();
		}
		else if (process == "show") {
			showDomain();
		}
		else if (process == "show-panel") {
			showPanel();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void logmng::init()
	{
		// @TODO Yazýlacak
	}

	void logmng::showDomain()
	{
		try {
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Missing arguments", 3802);
				System::Environment::Exit(3802);
			}

			DateTime dateTime = DateTime::UtcNow.Date;

			String ^directory = env->vhosts_dir + domain + "\\stats\\logs";
			if (validate->checkDirectory(directory) == false) {
				logger->errorLog("Missing log directory", 3803);
				System::Environment::Exit(3803);
			}
			String ^last_updated = GetLatestWritenFileFileInDirectory(directory);

			StreamReader ^reader = gcnew StreamReader(last_updated);
			reader->BaseStream->Seek(0, SeekOrigin::End);
			int count = 0;
			while ((count < 20) && (reader->BaseStream->Position > 0)) {
				reader->BaseStream->Position--;
				int c = reader->BaseStream->ReadByte();
				if (reader->BaseStream->Position > 0)
					reader->BaseStream->Position--;
				if (c == Convert::ToInt32('\n')) {
					++count;
				}
			}
			String ^str = reader->ReadToEnd();
			Console::WriteLine(str->Replace("\r", ""));
			reader->Close();
		}
		catch (Exception ^e) {
			logger->debugLog("logmng: " + e->Message, 3804);
		}

	}

	void logmng::showPanel()
	{
		try {
			StreamReader ^reader = gcnew StreamReader(env->panel_log_dir + "\\error.log");
			reader->BaseStream->Seek(0, SeekOrigin::End);
			int count = 0;
			while ((count < 20) && (reader->BaseStream->Position > 0)) {
				reader->BaseStream->Position--;
				int c = reader->BaseStream->ReadByte();
				if (reader->BaseStream->Position > 0)
					reader->BaseStream->Position--;
				if (c == Convert::ToInt32('\n')) {
					++count;
				}
			}
			String ^str = reader->ReadToEnd();
			Console::WriteLine(str->Replace("\r", ""));
			reader->Close();
		}
		catch (Exception ^e) {
			logger->debugLog("logmng: " + e->Message, 3805);
		}
	}

	String ^logmng::GetLatestWritenFileFileInDirectory(String ^directory)
	{
		try {
			array<String^> ^allfiles = System::IO::Directory::GetFiles(directory, "*.*", System::IO::SearchOption::AllDirectories);
			DateTime lastWrite = DateTime::MinValue;
			String ^lastWritenFile = nullptr;

			for each (String ^file in allfiles)
			{
				FileInfo ^info = gcnew FileInfo(file);
				if (info->LastWriteTime > lastWrite) {
					lastWrite = info->LastWriteTime;
					lastWritenFile = file;
				}
			}
			return lastWritenFile->ToString();
		}
		catch (Exception ^e) {
			logger->debugLog("logmng: " + e->Message, 3806);
			return "";
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	logmng ^mainProcess = gcnew logmng();
	mainProcess->Main(Args);
	return 0;
}