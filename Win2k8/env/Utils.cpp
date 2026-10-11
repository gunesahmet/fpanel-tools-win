#pragma managed

#include "Stdafx.h"
#include "Utils.h"


namespace Environments
{
	DateTime FUtils::UnixTimestampToDateTime(double unixTime)
	{
		DateTime unixStart = DateTime(1970, 1, 1, 0, 0, 0, 0, System::DateTimeKind::Utc);
		Int64 unixTimeStampInTicks = safe_cast<Int64>(unixTime * TimeSpan::TicksPerSecond);
		return DateTime(unixStart.Ticks + unixTimeStampInTicks, System::DateTimeKind::Utc);
	}

	bool FUtils::isAdmin()
	{
		bool isAdmin;
		try {
			WindowsIdentity ^user = WindowsIdentity::GetCurrent();
			WindowsPrincipal ^principal = gcnew WindowsPrincipal(user);
			isAdmin = principal->IsInRole(WindowsBuiltInRole::Administrator);
		}
		catch (...) {
			isAdmin = false;
		}

		return isAdmin;
	}

	String ^FUtils::exec(String ^command, String ^parameters)
	{
		String ^output = "";
		try {
			System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();
			pProcess->StartInfo->CreateNoWindow = true;

			pProcess->StartInfo->RedirectStandardOutput = true;
			pProcess->StartInfo->RedirectStandardInput = true;
			pProcess->StartInfo->WindowStyle = ProcessWindowStyle::Hidden;

			pProcess->StartInfo->UseShellExecute = false;

			String ^strCommand = command;
			pProcess->StartInfo->FileName = strCommand;

			if (parameters != nullptr) {
				String ^strCommandParameters = parameters;
				pProcess->StartInfo->Arguments = strCommandParameters;
				pProcess->StartInfo->EnvironmentVariables->Values;
			}

			pProcess->EnableRaisingEvents = false;

			try {
				pProcess->Start();
				while (!pProcess->StandardOutput->EndOfStream) {
					String ^line = pProcess->StandardOutput->ReadLine();
					line = Trim(line);
					output += line + "\n";
					//Console::WriteLine("line: " + line);
				}
				exit_code = pProcess->ExitCode.ToString();
			}
			catch (Exception ^e) {
				FPanel::Logger::errorLog("FUtils::exec exit code Error: " + e->Message, 1801);
			}

			StreamReader ^streamReader = pProcess->StandardOutput;
			pProcess->WaitForExit();
		}
		catch (...) {
			//Console::WriteLine(command + " " + parameters);
			return output;
		}

		return output;
	}

	Int64 FUtils::unixTimeStamp()
	{
		String ^string = DateTime::Now.ToUniversalTime().ToString("MM/dd/yyyy HH:mm:ss");
		array<String^> ^strArray1 = string->Split(' ');
		strArray1[0] = string->Replace(".", "/");
		strArray1[0] = string->Replace("-", "/");
		array<String^> ^strArray2 = strArray1[0]->Split('/');
		short int16_1 = Convert::ToInt16(strArray2[0]);
		short int16_2 = Convert::ToInt16(strArray2[1]);
		short int16_3 = Convert::ToInt16(strArray2[2]);
		array<String^> ^strArray3 = strArray1[0]->Split('/');
		short int16_4 = Convert::ToInt16(strArray3[0]);
		short int16_5 = Convert::ToInt16(strArray3[1]);
		short int16_6 = Convert::ToInt16(strArray3[2]);
		return safe_cast<Int64>((DateTime::UtcNow - DateTime(safe_cast<int>(int16_3), safe_cast<int>(int16_1), safe_cast<int>(int16_2), safe_cast<int>(int16_4), safe_cast<int>(int16_5), safe_cast<int>(int16_6))).TotalSeconds);
	}

	bool FUtils::moveFile(String ^from, String ^there)
	{
		try {
			if (File::Exists(there))
				File::Delete(there);
			File::Move(from, there);
		}
		catch (...) {

		}

		if (File::Exists(there)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool FUtils::writeFile(String ^file, String ^data)
	{
		bool is_exists = File::Exists(file);
		if (!is_exists) {
			TextWriter ^tw = gcnew StreamWriter(file);
			try {
				tw->Write(data);
				tw->Close();
			}
			catch (Exception ^e) {
				Console::WriteLine("Error Create file: " + e->Message);
				return false;
			}
			finally
			{
				delete tw;
			}
		}
		else if (is_exists) {
			TextWriter ^tw = gcnew StreamWriter(file);
			try {
				tw->WriteLine(data);
				tw->Close();
			}
			catch (Exception ^e) {
				Console::WriteLine("Error ReCreate file: " + e->Message);
				return false;
			}
			finally
			{
				delete tw;
			}
		}
		return true;
	}

	bool FUtils::copyFolder(String ^source, String ^target, bool sub_dir)
	{
		try {
			DirectoryInfo ^dir = gcnew DirectoryInfo(source);

			if (!dir->Exists) {
				return false;
			}

			array<DirectoryInfo^> ^dirs = dir->GetDirectories();

			if (!Directory::Exists(target)) {
				Directory::CreateDirectory(target);
			}

			array<FileInfo^> ^files = dir->GetFiles();

			for each (FileInfo ^file in files)
			{
				String ^temppath = Path::Combine(target, file->Name);
				file->CopyTo(temppath, true);
			}
			array<String^> ^subdirectoryEntries = Directory::GetDirectories(source);

			if (sub_dir) {
				for each (DirectoryInfo ^subdir in dirs)
				{
					String ^temppath = Path::Combine(target, subdir->Name);
					copyFolder(subdir->FullName, temppath, sub_dir);
				}
			}
			return true;
		}
		catch (...) {
			return false;
		}

	}

	bool FUtils::copyFile(String ^from, String ^there)
	{
		//try {
			if (!File::Exists(from)) {
				return false;
			}
			File::Copy(from, there, true);
			if (File::Exists(there)) {
				return true;
			}
			else {
				return false;
			}
	/*	}
		catch (...) {
			return false;
		}*/
	}

	bool FUtils::appendFile(String ^file, String ^str)
	{
		try {
			StreamWriter ^stream = (gcnew FileInfo(file))->AppendText();
			try {
				stream->WriteLine(str);
			}
			finally
			{
				delete stream;
			}
		}
		catch (...) {
			return false;
		}

		return true;
	}

	bool FUtils::removeFile(String ^file)
	{
		try {
			if (File::Exists(file)) {
				GC::Collect();
				GC::WaitForPendingFinalizers();
				FileInfo ^f = gcnew FileInfo(file);
				f->Delete();
			}
		}
		catch (...) {
			return false;
		}
		return true;
	}

	bool FUtils::removeFolder(String ^folder, bool sub)
	{
		try {
			if (Directory::Exists(folder)) {
				Directory::Delete(folder, true);
			}
			return true;
		}
		catch (...) {
			Console::WriteLine("Directory {0} cant't delete", folder);
			return false;
		}

		return true;
	}

	bool FUtils::createFolder(String ^folder)
	{
		try {
			DirectoryInfo ^dir = gcnew DirectoryInfo(folder);
			if (!dir->Exists) {
				Directory::CreateDirectory(folder);
			}
		}
		catch (...) {
			return false;
		}

		return true;
	}

	bool FUtils::removeLineFromFile(String ^file, String ^str)
	{
		try {
			Regex ^searchRegex = gcnew Regex(str);

			String ^text_for_files = "";

			StreamReader ^sr = File::OpenText(file);
			try {
				array<String^> ^lines = File::ReadAllLines(file);

				for each (String ^line in lines)
				{
					if (searchRegex->IsMatch(line)) {
						continue;
					}
					else {
						if (line != nullptr)
							text_for_files += line + "\n";
					}
				}
				sr->Close();
				StreamWriter ^sw = gcnew StreamWriter(file);
				try {
					sw->Write(text_for_files);
					sw->Close();
				}
				finally
				{
					delete sw;
				}
			}
			finally
			{
				delete sr;
			}
		}
		catch (...) {
			return false;
		}

		return true;
	}

	void FUtils::replaceLine(String ^file, String ^old_str, String ^new_str)
	{
		try {
			Regex ^searchRegex = gcnew Regex(old_str);
			String ^text_for_files = "";

			StreamReader ^sr = File::OpenText(file);
			try {
				array<String^> ^lines = File::ReadAllLines(file);
				for each (String ^line in lines)
				{
					if (searchRegex->IsMatch(line)) {
						//text_for_files += line->Replace(old_str, new_str);
						text_for_files += new_str + "\n";
					}
					else {
						if (line != nullptr)
							text_for_files += line + "\n";
					}
				}
				sr->Close();
			}
			finally
			{
				delete sr;
			}

			StreamWriter ^sw = gcnew StreamWriter(file);
			try {
				sw->Write(text_for_files);
				sw->Close();
			}
			finally
			{
				delete sw;
			}
		}
		catch (...) {
		}
	}

	void FUtils::replaceAll(String ^%str, String ^old_str, String ^new_str)
	{
		str = str->Replace(old_str, new_str);
		Regex ^searchRegex = gcnew Regex(old_str);
		if (searchRegex->IsMatch(str)) {
			str = str->Replace(old_str, new_str);
		}
	}

	bool FUtils::iniReplace(String ^%str, String ^old_str, String ^new_str)
	{
		String ^nstr;
		bool status = false;
		array<String^> ^lines = str->Replace("\r", "")->Split('\n');
		for each (String ^line in lines) {
			Regex ^searchRegex = gcnew Regex(old_str);
			if (searchRegex->IsMatch(line)) {
				if (new_str == nullptr) {
					nstr += ";" + line + "\n";
				}
				else{
					nstr += new_str + "\n";
				}
				status = true;
			}
			else {
				nstr += line + "\n";
			}
		}
		str = nstr;
		return status;
	}

	String ^FUtils::Base64Encode(String ^str)
	{
		try {
			array<System::Byte> ^plainTextBytes = System::Text::Encoding::UTF8->GetBytes(str);
			return System::Convert::ToBase64String(plainTextBytes);
		}
		catch (...) {
			return "";
		}
	}

	String ^FUtils::Base64Decode(String ^str)
	{
		array<System::Byte> ^base64EncodedBytes = System::Convert::FromBase64String(str);
		return System::Text::Encoding::UTF8->GetString(base64EncodedBytes);
	}

	String ^FUtils::defaultIpAddress()
	{
		array<Char> ^chArray = gcnew array<Char>(3) { ' ', '\t', '\n' };
		String ^pattern = "0.0.0.0.*.0.0.0.0";
		String ^s = exec("route", "PRINT 0.0.0.0 -4");
		String ^str1 = "";
		StringReader ^stringReader = gcnew StringReader(s);
		try {
			int num1 = 0;
			String ^str2 = String::Empty;
			String ^input;
			do {
				input = stringReader->ReadLine();
				if (input != nullptr && Regex::IsMatch(input, pattern) && num1 == 0) {
					array<String^> ^strArray = input->Split(' ');
					int num2 = 0;
					for each (String ^str3 in strArray) {
						if (str3->Trim(chArray) != "") {
							if (num2 == 3) {
								str1 = str3;
							}
							++num2;
						}
					}
					++num1;
				}
			} while (input != nullptr);
		}
		finally {
			delete stringReader;
		}
		return str1;
	}

	String ^FUtils::Trim(String ^str)
	{
		array<Char> ^chArray = gcnew array<Char>(4) { ' ', '\t', '\n', '\r' };
		return str->Trim(chArray);
	}

	void FUtils::FindInstalls(RegistryKey ^regKey, List<String^> ^keys, Dictionary<String^, String^> ^%installed)
	{
		for each (String ^key in keys) {
			//Console::WriteLine(key);
			RegistryKey ^rk = regKey->OpenSubKey(key, true);
			if (rk == nullptr) {
				continue;
			}
			//Console::WriteLine(key);
			for each (String ^rkName in rk->GetSubKeyNames()) {
				RegistryKey ^sk = rk->OpenSubKey(rkName, true);
				//Console::WriteLine(rkName);
				if (sk != nullptr) {
					for each(String ^valueName in sk->GetValueNames()) {
						if (valueName->Contains("DisplayName")) {
							if (sk->GetValue("DisplayName")->ToString() != "") {
								//Console::WriteLine(Convert::ToString(sk->GetValue("DisplayName")));
								installed[Convert::ToString(sk->GetValue("DisplayName"))] = Convert::ToString(sk->GetValue("DisplayVersion"));
							}
						}
					}
					sk->Close();
					delete sk;
				}
			}
			rk->Close();
			delete rk;
		}
	}

	array<String^> ^FUtils::split(String ^str, array<Char> ^separators)
	{
		array<String^> ^subs = str->Split(separators, StringSplitOptions::RemoveEmptyEntries);
		return subs;
	}
}