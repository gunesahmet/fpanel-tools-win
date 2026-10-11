#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

namespace FPanel
{
	public ref class Logger
	{
	private:
		static String ^error_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\error.log";
		static String ^info_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\success.log";
		static String ^debug_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\debug.log";
		static String ^tmp_path = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\tmp";

	public:
		static void errorLog(String ^message, int code)
		{
			String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
			try {
				if (Directory::Exists(tmp_path) == false) {
					Directory::CreateDirectory(tmp_path);
				}
			}
			catch (...) {

			}
			StreamWriter ^streamWriter = (gcnew FileInfo(error_logs))->AppendText();
			try {
				streamWriter->WriteLine("[{0}] [ERROR][{1}]\t{2}", current_date, code, message);
				streamWriter->Close();
			}
			finally {
				delete streamWriter;
			}
		}

		static void infoLog(String ^message, int code)
		{
			String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
			try {
				if (Directory::Exists(tmp_path) == false) {
					Directory::CreateDirectory(tmp_path);
				}
			}
			catch (...) {
			}
			StreamWriter ^streamWriter = (gcnew FileInfo(info_logs))->AppendText();
			try {
				streamWriter->WriteLine("[{0}] [INFO][{1}]\t{2}", current_date, code, message);
				streamWriter->Close();
			}
			finally {
				delete streamWriter;
			}
		}

		static void debugLog(String ^message)
		{
			if (File::Exists(Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\core\\bin\\debug"))
			{
				String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
				try {
					if (Directory::Exists(tmp_path) == false) {
						Directory::CreateDirectory(tmp_path);
					}
				}
				catch (...) {
				}
				StreamWriter ^streamWriter = (gcnew FileInfo(debug_logs))->AppendText();
				try {
					streamWriter->WriteLine("[{0}] [DEBUG]\t{1}", current_date, message);
					streamWriter->Close();
				}
				finally {
					delete streamWriter;
				}
			}
		}
	};
}