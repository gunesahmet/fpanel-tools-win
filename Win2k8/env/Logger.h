//.h file code:
#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

namespace Environments
{
	public ref class FileLogger
	{
	public:
		String ^error_logs;
		String ^info_logs;
		String ^debug_logs;
		String ^tmp_path;

		void errorLog(String ^message, int code);

		void infoLog(String ^message, int code);

		void debugLog(String ^message, int code);

	private:
		void InitializeInstanceFields();

	public:
		FileLogger()
		{
			InitializeInstanceFields();
		}
	};
}