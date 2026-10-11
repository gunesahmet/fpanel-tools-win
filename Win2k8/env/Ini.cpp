#include "Stdafx.h"
#include "Ini.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;
using namespace System::Text;

namespace Environments
{
	Ini::Ini(String ^IniPath)
	{
		InitializeInstanceFields();
		Path = (gcnew FileInfo((IniPath != nullptr) ? IniPath : EXE + ".ini"))->FullName->ToString();
	}

	String ^Ini::Read(String ^Key, String ^Section)
	{
		StringBuilder ^RetVal = gcnew StringBuilder(255);
		GetPrivateProfileString((Section != nullptr) ? Section : EXE, Key, "", RetVal, 255, Path);
		return RetVal->ToString();
	}

	void Ini::Write(String ^Key, String ^Value, String ^Section)
	{
		WritePrivateProfileString((Section != nullptr) ? Section : EXE, Key, Value, Path);
	}

	void Ini::DeleteKey(String ^Key, String ^Section)
	{
		Write(Key, nullptr, (Section != nullptr) ? Section : EXE);
	}

	void Ini::DeleteSection(String ^Section)
	{
		Write(nullptr, nullptr, (Section != nullptr) ? Section : EXE);
	}

	bool Ini::KeyExists(String ^Key, String ^Section)
	{
		return Read(Key, Section)->Length > 0;
	}

	void Ini::InitializeInstanceFields()
	{
		EXE = Assembly::GetExecutingAssembly()->GetName()->Name;
	}
}