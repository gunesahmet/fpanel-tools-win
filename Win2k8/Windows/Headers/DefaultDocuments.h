#pragma once
using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class DefaultDocuments
		{
		private:

		public:
			void addDefaultDocuments(String ^siteName, String ^php, String ^aspnet, String ^asp, String ^perl, String ^cgi, String ^python)
			{
				if (php == "on") {
					this->addDefaultDocument(siteName, "index.php");
				}
				else {
					this->removeDefaultDocument(siteName, "index.php");
				}

				if (asp == "on") {
					this->addDefaultDocument(siteName, "default.asp");
					this->addDefaultDocument(siteName, "index.asp");
				}
				else {
					this->removeDefaultDocument(siteName, "default.asp");
					this->removeDefaultDocument(siteName, "index.asp");
				}


				if (aspnet == "on") {
					this->addDefaultDocument(siteName, "default.aspx");
					this->addDefaultDocument(siteName, "index.aspx");
				}
				else {
					this->removeDefaultDocument(siteName, "default.aspx");
					this->removeDefaultDocument(siteName, "index.aspx");
				}


				if (perl == "on") {
					this->addDefaultDocument(siteName, "index.pl");
				}
				else {
					this->removeDefaultDocument(siteName, "index.pl");
				}

				if (python == "on") {
					this->addDefaultDocument(siteName, "index.py");
				}
				else {
					this->removeDefaultDocument(siteName, "index.py");
				}

				if (cgi == "on") {
					this->addDefaultDocument(siteName, "index.cgi");
				}
				else {
					this->removeDefaultDocument(siteName, "index.cgi");
				}
			}

			void addDefaultDocument(String ^siteName, String ^element)
			{
				//this->checkFileStatus();
				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();

					ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);

					ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");

					ConfigurationElement ^addElement = FindElement(filesCollection, "add", "value", element);
					if (addElement == nullptr) {
						ConfigurationElement ^addElement9 = filesCollection->CreateElement("add");
						addElement9["value"] = element;
						filesCollection->AddAt(0, addElement9);
					}

					serverMgr->CommitChanges();
					delete serverMgr;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error DefaultDocuments::addDefaultDocument. Error: " + e->Message, 1105);
				}
			}

			void removeDefaultDocument(String ^siteName, String ^document)
			{
				//this->checkFileStatus();

				try {
					ServerManager ^serverMgr = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();

					ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);

					ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");

					ConfigurationElement ^addElement = FindElement(filesCollection, "add", "value", document);

					if (addElement != nullptr)
						filesCollection->Remove(addElement);

					serverMgr->CommitChanges();
					delete serverMgr;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error DefaultDocuments::removeDefaultDocument. Error: " + e->Message, 1105);
				}
			}

			bool checkFileStatus()
			{
				FileInfo ^file = gcnew FileInfo("C:\\Windows\\system32\\inetsrv\\config\\applicationHost.config");
				while (true) {

					if (this->IsFileLocked(file) == false) {
						return true;
					}
				}
			}

		protected:
			virtual bool IsFileLocked(FileInfo ^file)
			{
				FileStream ^stream = nullptr;

				try {
					stream = file->Open(FileMode::Open, FileAccess::ReadWrite, FileShare::None);
				}
				catch (...) {
					return true;
				}
				finally
				{
					if (stream != nullptr)
						stream->Close();
				}

					//file is not locked
				return false;
			}

		private:
			static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
			{
				for each (ConfigurationElement ^element in collection)
				{
					if (String::Equals(element->ElementTagName, elementTagName, StringComparison::OrdinalIgnoreCase)) {
						bool matches = true;

						for (int i = 0; i < keyValues->Length; i += 2) {
							Object ^o = element->GetAttributeValue(keyValues[i]);
							String ^value = nullptr;
							if (o != nullptr) {
								value = o->ToString();
							}

							if (!String::Equals(value, keyValues[i + 1], StringComparison::OrdinalIgnoreCase)) {
								matches = false;
								break;
							}
						}
						if (matches) {
							return element;
						}
					}
				}
				return nullptr;
			}
		private:
			void InitializeInstanceFields()
			{}

		public:
			DefaultDocuments()
			{
				InitializeInstanceFields();
			}
		};
	}
}