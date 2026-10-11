#include "Stdafx.h"
#include "Logger.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
#pragma managed

namespace Environments
{
	void FileLogger::errorLog(String ^message, int code)
	{
		String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
		try {
			if (Directory::Exists(tmp_path) == false) {
				Directory::CreateDirectory(tmp_path);
			}
		}
		finally
		{
		}
		StreamWriter ^streamWriter = (gcnew FileInfo(error_logs))->AppendText();
		try {
			streamWriter->WriteLine("[{0}] [ERROR][{1}]\t{2}", current_date, code, message);
			streamWriter->Close();
		}
		finally
		{
			delete streamWriter;
		}
	}

	void FileLogger::infoLog(String ^message, int code)
	{
		String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
		try {
			if (Directory::Exists(tmp_path) == false) {
				Directory::CreateDirectory(tmp_path);
			}
		}
		finally
		{
		}
		StreamWriter ^streamWriter = (gcnew FileInfo(info_logs))->AppendText();
		try {
			streamWriter->WriteLine("[{0}] [INFO][{1}]\t{2}", current_date, code, message);
			streamWriter->Close();
		}
		finally
		{
			delete streamWriter;
		}
	}

	void FileLogger::debugLog(String ^message, int code)
	{
		if (File::Exists(Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\core\\bin\\debug")) {
			String ^current_date = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
			try {
				if (Directory::Exists(tmp_path) == false) {
					Directory::CreateDirectory(tmp_path);
				}
			}
			finally
			{
			}
			StreamWriter ^streamWriter = (gcnew FileInfo(debug_logs))->AppendText();
			try {
				if (code != 0)
					streamWriter->WriteLine("[{0}] [DEBUG][{1}]\t{2}", current_date, code, message);
				else
					streamWriter->WriteLine("[{0}] [DEBUG]\t{1}", current_date, message);
				streamWriter->Close();
			}
			finally
			{
				delete streamWriter;
			}
		}
	}

	void FileLogger::InitializeInstanceFields()
	{
		error_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\error.log";
		info_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\success.log";
		debug_logs = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel\\debug.log";
		tmp_path = Path::GetPathRoot(Environment::SystemDirectory) + "\\fpanel\\var\\log\\fpanel";
	}
}