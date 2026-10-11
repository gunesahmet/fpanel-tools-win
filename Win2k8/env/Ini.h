using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;
using namespace System::Text;

namespace Environments
{
	public ref class Ini
	{
	private:
		String ^Path;
		String ^EXE;

		[DllImport("kernel32", CharSet = CharSet::Unicode)]
		static Int64 WritePrivateProfileString(String ^Section, String ^Key, String ^Value, String ^FilePath);

		[DllImport("kernel32", CharSet = CharSet::Unicode)]
		static int GetPrivateProfileString(String ^Section, String ^Key, String ^Default, StringBuilder ^RetVal, int Size, String ^FilePath);

	public:
		Ini(String ^IniPath);

		String ^Read(String ^Key, String ^Section);

		void Write(String ^Key, String ^Value, String ^Section);

		void DeleteKey(String ^Key, String ^Section);

		void DeleteSection(String ^Section);

		bool KeyExists(String ^Key, String ^Section);

	private:
		void InitializeInstanceFields();
	};
}
