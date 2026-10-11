#pragma once
#include "Logger.h"
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class DefaultVDir
		{
		private:
		public:
			void createVirtualDirectory(String ^siteName, String ^vDirName, String ^physicalPath, String ^type)
			{
				if (this->checkVirtualDir(siteName, vDirName) == false) {
					try {
						ServerManager ^serverMgr = gcnew ServerManager();
						if (type == "vdir") {
							Site ^vDirSite = serverMgr->Sites[siteName];
							vDirSite->Applications[0]->VirtualDirectories->Add("/" + vDirName, "C:\\vhosts\\" + siteName + "\\" + physicalPath);
						}
						else {
							Site ^appSite = serverMgr->Sites[siteName];
							Application ^application = appSite->Applications->Add("/" + vDirName, "C:\\vhosts\\" + siteName + "\\" + physicalPath);
						}
						serverMgr->CommitChanges();
					}
					catch (Exception ^e) {
						Logger::errorLog("Error DefaultVDir::createVirtualDirectory. Error: " + e->Message, 1105);
					}
				}
			}

			bool checkVirtualDir(String ^siteName, String ^vDirApp)
			{
				bool result = false;
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Site ^defaultSite = serverMgr->Sites[siteName];
					for each (Application ^app in defaultSite->Applications)
					{
						if (app->VirtualDirectories->Count > 0) {
							for each (VirtualDirectory ^vdir in app->VirtualDirectories)
							{
								if (vDirApp == vdir->ToString()) {
									result = true;
								}
							}
						}
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error DefaultVDir::checkVirtualDir. Error: " + e->Message, 1105);
				}
				return result;
			}

		private:
			void InitializeInstanceFields()
			{}

		public:
			DefaultVDir()
			{
				InitializeInstanceFields();
			}
		};
	}
}