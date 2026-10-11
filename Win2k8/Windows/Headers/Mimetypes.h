#pragma once
#include "Logger.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;

namespace FPanel
{
	namespace Windows
	{
		private ref class MimeTypes
		{
		private:
			ServerManager ^serverMgr;

		public:
			void addMimeType(String ^siteName, String ^fileExtension, String ^fileExtensionOctet)
			{
				if (this->checkMimeType(siteName, fileExtension) == true)
					System::Environment::Exit(1);

				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^staticContentSection = config->GetSection("system.webServer/staticContent", siteName);
					ConfigurationElementCollection ^staticContentCollection = staticContentSection->GetCollection();
					ConfigurationElement ^mimeMapElement = staticContentCollection->CreateElement("mimeMap");
					mimeMapElement["fileExtension"] = fileExtension;
					mimeMapElement["mimeType"] = fileExtensionOctet;
					staticContentCollection->Add(mimeMapElement);
					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":addMimeType" + ". Error: " + e->Message, 1105);
				}
			}

			void removeMimeType(String ^siteName, String ^fileExtension)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^staticContentSection = config->GetSection("system.webServer/staticContent", siteName);
					ConfigurationElementCollection ^staticContentCollection = staticContentSection->GetCollection();
					ConfigurationElement ^mimeMapElement = FindElement(staticContentCollection, "mimeMap", "fileExtension", fileExtension);
					if (mimeMapElement != nullptr)
						staticContentCollection->Remove(mimeMapElement);
					serverMgr->CommitChanges();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":removeMimeType" + ". Error: " + e->Message, 1105);
				}
			}

		private:
			bool checkMimeType(String ^siteName, String ^fileExtension)
			{
				bool result = false;
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^staticContentSection = config->GetSection("system.webServer/staticContent", siteName);
					ConfigurationElementCollection ^staticContentCollection = staticContentSection->GetCollection();
					ConfigurationElement ^mimeMapElement = FindElement(staticContentCollection, "mimeMap", "fileExtension", fileExtension);
					if (mimeMapElement != nullptr)
						result = true;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":checkMimeType" + ". Error: " + e->Message, 1105);
				}
				return result;

			}

			static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
			{
				for each (ConfigurationElement ^element in collection)
				{
					if (String::Equals(element->ElementTagName, elementTagName, StringComparison::OrdinalIgnoreCase))
					{
						bool matches = true;

						for (int i = 0; i < keyValues->Length; i += 2)
						{
							Object ^o = element->GetAttributeValue(keyValues[i]);
							String ^value = nullptr;
							if (o != nullptr)
							{
								value = o->ToString();
							}

							if (!String::Equals(value, keyValues[i + 1], StringComparison::OrdinalIgnoreCase))
							{
								matches = false;
								break;
							}
						}
						if (matches)
						{
							return element;
						}
					}
				}
				return nullptr;
			}

		private:
			void InitializeInstanceFields()
			{
				serverMgr = gcnew ServerManager();
			}

		public:
			MimeTypes()
			{
				InitializeInstanceFields();
			}
		};
	}
}