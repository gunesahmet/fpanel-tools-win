#pragma once
#include "../../env/Env.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::DirectoryServices;

namespace FPanel
{
	namespace Windows
	{
		private ref class Vdir
		{
		private:
			Environments::Env ^env;
			String ^DIRECTORY_SEPERATOR = "\\";
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

		public:
			void createVirtualDirectory(String ^siteName, String ^vDirName, String ^physicalPath, String ^type, String ^prefix)
			{
				if (this->IsWebsiteExists(siteName) == false) {
					System::Environment::Exit(1608);
				}
				if (this->checkVirtualDir(siteName, vDirName, prefix) == false) {
					try {
						String ^domainAppPoolName = getDomainAppPool(siteName);
						ServerManager ^serverManager = gcnew ServerManager();
						if (type == "vdir") {
							Site ^vDirSite = serverManager->Sites[siteName];
							vDirSite->Applications[0]->VirtualDirectories->Add("/" + vDirName, env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + physicalPath);
						}
						else {
							Site ^appSite = serverManager->Sites[siteName];
							Application ^application = appSite->Applications->Add("/" + vDirName, env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + physicalPath);
						}
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
						ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
						ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);
						if (siteElement != nullptr) {
							ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();
							ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/" + vDirName);
							if (applicationElement != nullptr)
								applicationElement["applicationPool"] = domainAppPoolName;
						}
						serverManager->CommitChanges();
						delete serverManager;
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Vdir::createVirtualDirectory. Error: " + e->Message, 1105);
					}
				}
			}

			void updateVirtualDirectory(String ^siteName, String ^vDirName, String ^newVdirName, String ^physicalPath, String ^type, String ^prefix)
			{
				if (this->IsWebsiteExists(siteName) == false) {
					System::Environment::Exit(1608);
				}
				try {
					if (this->checkVirtualDir(siteName, vDirName, prefix) == false) {
						System::Environment::Exit(1609);
					}
					ServerManager ^serverManager = gcnew ServerManager();
					Application ^app = serverManager->Sites[siteName]->Applications["/" + vDirName];
					app->Path = "/" + newVdirName;
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::updateVirtualDirectory. Error: " + e->Message, 1105);
				}
			}

			void removeVirtualDirectory(String ^siteName, String ^vDirName, String ^type)
			{
				if (this->IsWebsiteExists(siteName) == false) {
					System::Environment::Exit(1608);
				}

				try {
					if (type == "vdir") {
						int websiteId = this->GetWebSiteId("localhost", siteName);

						String ^iisPath = String::Format("IIS://{0}/w3svc/{1}", "localhost", websiteId);

						DirectoryEntry ^w3svc = gcnew DirectoryEntry(iisPath);
						w3svc->RefreshCache();

						DirectoryEntry ^folderRoot = w3svc->Children->Find("Root", "IIsWebVirtualDir");
						folderRoot->RefreshCache();
						DirectoryEntry ^virtualDirectory_renamed = nullptr;
						folderRoot->Children->Find(vDirName, "IIsWebVirtualDir");
						folderRoot->Invoke("Delete", gcnew array<String^> {"IIsWebVirtualDir", vDirName});
						folderRoot->Children->Remove(virtualDirectory_renamed);
						delete folderRoot;
						delete w3svc;
					}

					if (type == "app") {
						ServerManager ^serverManager = gcnew ServerManager();
						Application ^app = serverManager->Sites[siteName]->Applications["/" + vDirName];
						serverManager->Sites[siteName]->Applications->Remove(app);
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::removeVirtualDirectory. Error: " + e->Message, 1105);
				}
				
			}

			void virtualDirectoryGeneralSettings(String ^siteName, String ^directoryBrowse, String ^accessPolicy, String ^dontLog,
				String ^enableParentPath, String ^executeInMTA, String ^defaultDocStatus, String ^defaultDocument, String ^defaultDocOrder,
				String ^directorySslFlag, String ^anonymousAuthStatus, String ^aspBufferLimit, String ^maxRequestEntityAllowed,
				String ^appAllowClientDebug, String ^appAllowDebugging)
			{
				this->dirBrowsing(directoryBrowse, siteName);
				this->accessPol(accessPolicy, siteName);
				this->logging(dontLog, siteName);
				this->parentPath(enableParentPath, executeInMTA, siteName);
				this->defaultDocumentStatus(defaultDocStatus, siteName);
				this->aspSetting(aspBufferLimit, maxRequestEntityAllowed, appAllowClientDebug, appAllowDebugging, siteName);
				this->anonymousAuthSettings(anonymousAuthStatus, siteName);
				this->accessSettings(directorySslFlag, siteName);
				this->clearDefaultDocument(siteName);
				this->defaultDocuments(defaultDocument, siteName);
			}

			void dirBrowsing(String ^directoryBrowse, String ^siteName)
			{
				try {
					if (directoryBrowse != nullptr) {
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						bool directoryBrowsing = false;
						if (directoryBrowse == "true") {
							directoryBrowsing = true;
						}

						ConfigurationSection ^directoryBrowseSection = config->GetSection("system.webServer/directoryBrowse", siteName);
						directoryBrowseSection["enabled"] = directoryBrowsing;
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::dirBrowsing. Error: " + e->Message, 1105);
				}
			}

			void accessPol(String ^accessPolicy, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					String ^accessRules = accessPolicy->Replace("#", ", ");
					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName);
					handlersSection["accessPolicy"] = accessRules;
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::accessPol. Error: " + e->Message, 1105);
				}
			}

			void logging(String ^dontLog, String ^siteName)
			{
				try {
					if (dontLog != nullptr) {
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

						bool logging = false;
						if (dontLog == "true") {
							logging = true;
						}
						ConfigurationSection ^httpLoggingSection = config->GetSection("system.webServer/httpLogging", siteName);
						httpLoggingSection["dontLog"] = logging;
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::logging. Error: " + e->Message, 1105);
				}
			}

			void parentPath(String ^enableParentPath, String ^executeInMTA, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^aspSection = config->GetSection("system.webServer/asp", siteName);

					if (enableParentPath != "") {
						bool parentPath = false;
						if (enableParentPath == "true") {
							parentPath = true;
						}
						aspSection["enableParentPaths"] = parentPath;
					}

					if (executeInMTA != "") {
						bool exec = false;

						if (executeInMTA == "true") {
							exec = true;
						}

						ConfigurationElement ^comPlusElement = aspSection->GetChildElement("comPlus");
						comPlusElement["executeInMta"] = exec;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::parentPath. Error: " + e->Message, 1105);
				}
			}

			void aspSetting(String ^aspBufferLimit, String ^maxRequestEntityAllowed, String ^appAllowClientDebug, String ^appAllowDebugging, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^aspSection = config->GetSection("system.webServer/asp", siteName);
					ConfigurationElement ^limitsElement = aspSection->GetChildElement("limits");

					if (aspBufferLimit != "") {
						limitsElement["bufferingLimit"] = Convert::ToInt32(aspBufferLimit);
					}

					if (maxRequestEntityAllowed != "") {
						limitsElement["maxRequestEntityAllowed"] = Convert::ToInt32(maxRequestEntityAllowed);
					}

					if (appAllowClientDebug != "") {
						bool clientDebug = false;
						if (appAllowClientDebug == "true") {
							clientDebug = true;
						}
						aspSection["appAllowClientDebug"] = clientDebug;
					}

					if (appAllowDebugging != "") {
						bool debugging = false;
						if (appAllowDebugging == "true") {
							debugging = true;
						}
						aspSection["appAllowDebugging"] = debugging;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::aspSetting. Error: " + e->Message, 1105);
				}
			}

			void accessSettings(String ^dirSSLFlag, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					if (dirSSLFlag != "") {
						ConfigurationSection ^accessSection = config->GetSection("system.webServer/security/access", siteName);
						accessSection["sslFlags"] = dirSSLFlag;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::accessSettings. Error: " + e->Message, 1105);
				}
			}

			void anonymousAuthSettings(String ^anonymousAuthentication, String ^siteName)
			{
				try {
					if (anonymousAuthentication != nullptr) {
						bool authState = false;
						if (anonymousAuthentication == "true") {
							authState = true;
						}
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^anonymousAuthenticationSection = config->GetSection("system.webServer/security/authentication/anonymousAuthentication", siteName);
						anonymousAuthenticationSection["enabled"] = authState;
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::anonymousAuthSettings. Error: " + e->Message, 1105);
				}
			}

			void defaultDocuments(String ^defaultDocuments, String ^siteName)
			{
				if (defaultDocuments != "") {
					array<String^> ^docArray = defaultDocuments->Split(',');
					for (int i = 0; i < docArray->Length; i++) {
						String ^document = docArray[i];
						int docOrder = i;
						this->addDefaultDocument(document, i, siteName);
					}
				}
			}

			void defaultDocumentStatus(String ^defaultDocStatus, String ^siteName)
			{
				try {
					if (defaultDocStatus != nullptr) {
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);

						bool defaultDocumentStatus = false;

						if (defaultDocStatus == "true") {
							defaultDocumentStatus = true;
						}

						defaultDocumentSection["enabled"] = defaultDocumentStatus;
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::defaultDocumentStatus. Error: " + e->Message, 1105);
				}
			}

			void clearDefaultDocument(String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);
					ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");
					filesCollection->Clear();
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::clearDefaultDocument. Error: " + e->Message, 1105);
				}
			}

			void addDefaultDocument(String ^documentName, int orders, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);
					ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");
					ConfigurationElement ^addElement = filesCollection->CreateElement("add");
					addElement["value"] = documentName;
					filesCollection->AddAt(Convert::ToInt32(orders), addElement);
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::addDefaultDocument. Error: " + e->Message, 1105);
				}
			}

			void virtualDirectorySettings(String ^siteName, String ^vDirName, String ^accessPolicy, String ^anonymousAuthentication,
				String ^aspBufferLimit, String ^maxRequestEntityAllowed, String ^appAllowClientDebug, String ^appAllowDebugging)
			{
				this->accessPolVdir(accessPolicy, siteName, vDirName);
				// this.anonymousAuthSettingsVdir(anonymousAuthentication, siteName, vDirName);
				this->aspSettingVdir(aspBufferLimit, maxRequestEntityAllowed, appAllowClientDebug, appAllowDebugging, siteName, vDirName);
			}

			void accessPolVdir(String ^accessPolicy, String ^siteName, String ^vDirName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					String ^accessRules = accessPolicy->Replace("#", ", ");
					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName + "/" + vDirName);
					handlersSection["accessPolicy"] = accessRules;
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::accessPolVdir. Error: " + e->Message, 1105);
				}
			}

			void anonymousAuthSettingsVdir(String ^anonymousAuthentication, String ^siteName, String ^vDirName)
			{
				try {
					if (anonymousAuthentication != "") {
						ServerManager ^serverManager = gcnew ServerManager();
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						bool authState = false;
						if (anonymousAuthentication == "true") {
							authState = true;
						}
						ConfigurationSection ^anonymousAuthenticationSection = config->GetSection("system.webServer/security/authentication/anonymousAuthentication", siteName + "/" + vDirName);
						anonymousAuthenticationSection["enabled"] = authState;
						serverManager->CommitChanges();
						delete serverManager;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::anonymousAuthSettingsVdir. Error: " + e->Message, 1105);
				}
			}

			void aspSettingVdir(String ^aspBufferLimit, String ^maxRequestEntityAllowed, String ^appAllowClientDebug, String ^appAllowDebugging, String ^siteName, String ^vDirName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^aspSection = config->GetSection("system.webServer/asp", siteName + "/" + vDirName);
					ConfigurationElement ^limitsElement = aspSection->GetChildElement("limits");

					if (aspBufferLimit != "") {
						limitsElement["bufferingLimit"] = Convert::ToInt32(aspBufferLimit);
					}

					if (maxRequestEntityAllowed != "") {
						limitsElement["maxRequestEntityAllowed"] = Convert::ToInt32(maxRequestEntityAllowed);
					}

					if (appAllowClientDebug != "") {
						bool clientDebug = false;
						if (appAllowClientDebug == "true") {
							clientDebug = true;
						}
						aspSection["appAllowClientDebug"] = clientDebug;
					}

					if (appAllowDebugging != "") {
						bool debugging = false;
						if (appAllowDebugging == "true") {
							debugging = true;
						}
						aspSection["appAllowDebugging"] = debugging;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::aspSettingVdir. Error: " + e->Message, 1105);
				}
			}

			bool checkVirtualDir(String ^siteName, String ^vDirApp, String ^prefix)
			{
				bool result = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Site ^defaultSite = serverManager->Sites[siteName];

					for each (Application ^app in defaultSite->Applications) {
						if (app->VirtualDirectories->Count > 0) {
							for each (VirtualDirectory ^vdir in app->VirtualDirectories) {
								if (prefix + "/" + vDirApp == vdir->ToString()) {
									result = true;
								}
							}
						}
					}
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::checkVirtualDir. Error: " + e->Message, 1105);
				}
		
				return result;
			}

			bool IsWebsiteExists(String ^siteName)
			{
				bool flagset = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					SiteCollection ^sitecollection = serverManager->Sites;
					for each (Site ^site in sitecollection) {
						if (site->Name == siteName->ToString()) {
							flagset = true;
							break;
						}
						else {
							flagset = false;
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vdir::IsWebsiteExists. Error: " + e->Message, 1105);
				}
				return flagset;
			}

			int GetWebSiteId(String ^serverName, String ^websiteName)
			{
				int result = -1;
				DirectoryEntry ^w3svc = gcnew DirectoryEntry(String::Format("IIS://{0}/w3svc", serverName));
				try {
					w3svc->RefreshCache();
					for each (DirectoryEntry ^site in w3svc->Children) {
						try {
							site->RefreshCache();
							if (site->Properties["ServerComment"] != nullptr) {
								if (site->Properties["ServerComment"]->Value != nullptr) {
									if (site->Properties["ServerComment"]->Value->ToString()->Equals(websiteName, StringComparison::OrdinalIgnoreCase)) {
										result = int::Parse(site->Name);
									}
								}
							}
						}
						finally {
							delete site;
						}
					}
				}
				finally{
					delete w3svc;
				}
				return result;
			}

			void setEnv(Environments::Env ^_env)
			{
				env = _env;
			}

			String ^getDomainAppPool(String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);

					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						return nullptr;

					String ^poolName = applicationElement["applicationPool"]->ToString();
					serverManager->CommitChanges();
					delete serverManager;
					return poolName;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error ApplicationPool::getApplicationPool. Error: " + e->Message, 1105);
					return env->integrated_default_apppool_v4;
				}
			}
		};
	}
}