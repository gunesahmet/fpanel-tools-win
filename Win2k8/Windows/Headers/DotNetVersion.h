#pragma once
using namespace Microsoft::Win32;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class DotNetVersion
		{
		public:
			String ^getVersion()
			{
				Environments::FileLogger ^logger = gcnew Environments::FileLogger();
				String ^versions = nullptr;
				try {
					Microsoft::Win32::RegistryKey ^localMachine = Registry::LocalMachine;
					RegistryKey ^ndpKey = localMachine->OpenSubKey("Microsoft\\NET Framework Setup\\NDP", true);
					if (ndpKey != nullptr) {
						for each (String ^versionKeyName in ndpKey->GetSubKeyNames())
						{
							if (versionKeyName->StartsWith("v")) {
								RegistryKey ^versionKey = ndpKey->OpenSubKey(versionKeyName);
								String ^name = safe_cast<String^>(versionKey->GetValue("Version", ""));
								String ^sp = versionKey->GetValue("SP", "")->ToString();
								String ^install = versionKey->GetValue("Install", "")->ToString();
								if (install == "1") {
									if (versions == nullptr) {
										versions = versionKeyName;
									}
									else {
										versions = versions + ',' + versionKeyName;
									}
								}
							}
						}
						ndpKey->Close();
						delete ndpKey;
					}
					else {
						Logger::errorLog("Error DotNetVersion::getVersion.", 1105);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error DotNetVersion::getVersion. Error: " + e->Message, 1105);
				}

				return versions;
			}
		};
	}
}