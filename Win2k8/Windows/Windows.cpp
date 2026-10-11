#include "stdafx.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::DirectoryServices;
using namespace System::Security::Principal;
using namespace System::Globalization;

int main(array<System::String ^> ^args)
{
	String ^domain = "domain.com";
	String ^username = "Test";
	String ^password = "ENTER_SECRET_KEY";

	int combinedFlag = 0;
	// Password Can not change
	int ADS_UF_PASSWD_CANT_CHANGE = 0x000000040;

	// Password Never Expires
	int ADS_UF_DONT_EXPIRE_PASSWD = 0x00010000;
	combinedFlag = ADS_UF_DONT_EXPIRE_PASSWD | ADS_UF_PASSWD_CANT_CHANGE;

	DirectoryEntry ^local = gcnew DirectoryEntry("WinNT://localhost");
	DirectoryEntries ^entries = local->Children;

	DirectoryEntry ^user = entries->Add(username, "User");
	user->Properties["FullName"]->Add(username);
	user->Properties["HomeDirectory"]->Value = "c:\\vhosts\\" + domain;
	user->Invoke("SetPassword", password);
	user->Invoke("Put", gcnew array<Object^> {"userFlags", combinedFlag});
	user->CommitChanges();
	return 0;
}
