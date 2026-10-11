#include "Stdafx.h"
#include "Utils.h"
#include "Validations.h"
#include <vector>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Security::Principal;
using namespace System::Text;
using namespace System::Text::RegularExpressions;
using namespace Microsoft::Web::Administration;

namespace Environments
{
	bool Validations::validateDomain(String ^domain)
	{
		try {
			String ^ValidHostnameRegex = "^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}$";
			return Regex::IsMatch(domain, ValidHostnameRegex);
		}
		finally
		{
		}
		return false;
	}

	bool Validations::validateIpAddress(String ^ipaddress)
	{
		try {
			String ^ValidHostnameRegex = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
			return Regex::IsMatch(ipaddress, ValidHostnameRegex);
		}
		finally
		{
		}
		return false;
	}

	bool Validations::validateFileName(String ^file_name)
	{
		//string ValidHostnameRegex = @"(?i)^(?!^(PRN|AUX|CLOCK\$|NUL|CON|COM\d|LPT\d|\..*)(\..+)?$)[^\\\./:\*\?\"<>\|][^\\/:\*\?\"<>\|]{0,254}$";
		//return Regex.IsMatch(file_name, ValidHostnameRegex);
		return false;
	}

	bool Validations::checkUser(String ^username)
	{
		bool bRet = false;
		try {
			NTAccount ^acct = gcnew NTAccount(username);
			SecurityIdentifier ^id = safe_cast<SecurityIdentifier^>(acct->Translate(SecurityIdentifier::typeid));
			bRet = id->IsAccountSid();
		}
		catch (...) {
			/* Invalid user account */
		}
		return bRet;
	}

	bool Validations::checkDirectory(String ^directory)
	{
		bool result = false;

		if (Directory::Exists(directory)) {
			result = true;
		}

		return result;
	}

	bool Validations::checkDirectoryStartWith(String ^path, String ^search_str)
	{
		array<String^> ^dirs = Directory::GetDirectories(path, search_str, SearchOption::TopDirectoryOnly);
		if (dirs->Length > 0) {
			return true;
		}
		return false;
	}

	bool Validations::checkFile(String ^file)
	{
		bool result = false;

		if (File::Exists(file)) {
			result = true;
		}

		return result;
	}

	bool Validations::fileIsContains(String ^file, String ^rgx)
	{
		bool result = false;
		Regex ^searchRegex = gcnew Regex(rgx);
		StreamReader ^sr = File::OpenText(file);

		try {
			array<String^> ^lines = File::ReadAllLines(file);
			for each (String ^line in lines)
			{
				if (searchRegex->IsMatch(line)) {
					sr->Close();
					result = true;
				}
			}
		}
		finally{
			delete sr;
		}

		return result;
	}

	bool Validations::isDomainExistsIis(String ^domain)
	{
		bool result = false;
		try {
			ServerManager ^serverManager = gcnew ServerManager();
			SiteCollection ^sitecollection = serverManager->Sites;
			for each (Site ^site in sitecollection) {
				if (site->Name == domain) {
					result = true;
					break;
				}
			}
			delete serverManager;
		}
		catch (...) {
		}
		return result;

	}
}