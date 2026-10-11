#pragma once
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		public ref class IISLogs
		{
		private:

		public:
			void initLogs()
			{
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");

					ConfigurationElement ^logFileElement = siteDefaultsElement->GetChildElement("logFile");
					logFileElement["localTimeRollover"] = true;

					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error IISLogs::initLogs. Error: " + e->Message, 1105);
				}
			}

		private:
			void InitializeInstanceFields()
			{
			}

		public:
			IISLogs()
			{
				InitializeInstanceFields();
			}
		};
	}
}