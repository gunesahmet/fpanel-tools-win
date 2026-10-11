#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::Xml;
using namespace System::Diagnostics;

namespace FPanel
{
	namespace Windows
	{
		private ref class IISSettings
		{
		private:
			ServerManager ^serverMgr;

		public:
			void iisWebServerSettingsList()
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");


					ConfigurationSection ^requestSection = config->GetSection("system.ftpServer/security/requestFiltering");

					ConfigurationSection ^urlCompressionSection = config->GetSection("system.webServer/urlCompression");

					ConfigurationSection ^httpCompressionSection = config->GetSection("system.webServer/httpCompression");


					String ^doDynamicCompression = urlCompressionSection->GetAttributeValue("doDynamicCompression")->ToString();
					String ^doStaticCompression = urlCompressionSection->GetAttributeValue("doStaticCompression")->ToString();
					// Eðer minFileSizeForComp 0 olursa disable oluyor
					String ^minFileSizeForComp = httpCompressionSection->GetAttributeValue("minFileSizeForComp")->ToString();
					String ^cacheDirectory = httpCompressionSection->GetAttributeValue("directory")->ToString();
					cacheDirectory = cacheDirectory->Replace("\\", "/");
					cacheDirectory = cacheDirectory->Replace("%", "#");
					String ^maxDiskSpaceUsage = httpCompressionSection->GetAttributeValue("maxDiskSpaceUsage")->ToString();
					String ^doDiskSpaceLimiting = httpCompressionSection->GetAttributeValue("doDiskSpaceLimiting")->ToString();

					String ^jSon = "{\"doDynamicCompression\":\"" + doDynamicCompression + "\", \"doStaticCompression\":\"" + doStaticCompression + "\",\"minFileSizeForComp\": \"" + minFileSizeForComp + "\",\"cacheDirectory\":\"" + cacheDirectory + "\",\"maxDiskSpaceUsage\":\"" + maxDiskSpaceUsage + "\",\"doDiskSpaceLimiting\":\"" + doDiskSpaceLimiting + "\"}";

					Console::WriteLine(jSon);
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISSettings::iisWebServerSettingsList. Error: " + e->Message, 1105);
				}
			}

			void saveIISsetting( String ^dynamicCompression, String ^staticCompression, 
				String ^fileSizeCompres, String ^cacheDirectory, String ^diskSpaceStatus, String ^diskSpaceLimit)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");

					bool sCompress = false;
					if (staticCompression == "true") {
						sCompress = true;
					}

					bool dCompress = false;
					if (dynamicCompression == "true") {
						dCompress = true;
					}
					ConfigurationSection ^urlCompressionSection = config->GetSection("system.webServer/urlCompression");
					urlCompressionSection["doStaticCompression"] = sCompress;
					urlCompressionSection["doDynamicCompression"] = dCompress;

					bool diskLimitStatus = false;
					if (diskSpaceStatus == "true") {
						diskLimitStatus = true;
					}

					ConfigurationSection ^httpCompressionSection = config->GetSection("system.webServer/httpCompression");
					httpCompressionSection["doDiskSpaceLimiting"] = diskLimitStatus;
					httpCompressionSection["maxDiskSpaceUsage"] = Convert::ToInt32(diskSpaceLimit);

					if (fileSizeCompres == nullptr) {
						fileSizeCompres = "0";
					}
					httpCompressionSection["minFileSizeForComp"] = Convert::ToInt32(fileSizeCompres);

					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISSettings::saveIISsetting. Error: " + e->Message, 1105);
				}
			}

			void clearSegments()
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^requestFilteringSection = config->GetSection("system.ftpServer/security/requestFiltering");
					ConfigurationElementCollection ^hiddenSegmentsCollection = requestFilteringSection->GetCollection("hiddenSegments");
					hiddenSegmentsCollection->Clear();
					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISSettings::clearSegments. Error: " + e->Message, 1105);
				}
			}

			void addSegments(String ^hiddenSegments)
			{
				if (String::IsNullOrEmpty(hiddenSegments))
				{
					this->clearSegments();
				}
				else
				{
					this->clearSegments();

					try {
						array<String^> ^hiddenSegmentsArray = hiddenSegments->Split(gcnew array<Char> {','});

						for each (String ^segment in hiddenSegmentsArray) {
							Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
							ConfigurationSection ^requestFilteringSection = config->GetSection("system.ftpServer/security/requestFiltering");
							ConfigurationElementCollection ^hiddenSegmentsCollection = requestFilteringSection->GetCollection("hiddenSegments");
							ConfigurationElement ^addElement = hiddenSegmentsCollection->CreateElement("add");
							addElement["segment"] = segment;
							hiddenSegmentsCollection->Add(addElement);
							serverMgr->CommitChanges();
						}
					}
					catch (Exception ^e) {
						Logger::errorLog("Error IISSettings::addSegments. Error: " + e->Message, 1105);
					}
				}
			}

		private:
			void InitializeInstanceFields()
			{
				serverMgr = gcnew ServerManager();
			}

		public:
			IISSettings()
			{
				InitializeInstanceFields();
			}
		};

	}
}