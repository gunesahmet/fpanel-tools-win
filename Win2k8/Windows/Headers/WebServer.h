#pragma once
#include "Admin.h"
#include "ApplicationPool.h"
#include "IISSettings.h"
#include "AspNetSettings.h"
#include "IISLogs.h"
#include "Vdir.h"
#include "FileManager.h"
#include "Handlers.h"
#include "Logger.h"
#include "IISFtp.h"
#include "DotNetVersion.h"
#include "Vhost.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::DirectoryServices;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Security::AccessControl;
using namespace FPanel::Windows;

namespace FPanel
{
	namespace Windows
	{
		private ref class WebServer
		{
		private:
			String ^system_user = "FpanelAppPool";
			String ^system_group = "FpanelUsers";
			String ^classic_default_apppool_v4 = "FpanelAppPoolv4Classic";
			String ^integrated_default_apppool_v4 = "FpanelAppPoolv4Integrated";
			String ^classic_default_apppool_v2 = "FpanelAppPoolv2Classic";
			String ^integrated_default_apppool_v2 = "FpanelAppPoolv2Integrated";
			String ^nomanaged_default_apppool = "FpanelAppPoolNoManaged";

			IISLogs ^iis_logs = gcnew IISLogs();
			AspNetSettings ^aspnet_settings = gcnew AspNetSettings();
			IISFtp ^ftp = gcnew IISFtp();
			Admin ^admin = gcnew Admin();
			DotNetVersion ^dotnet_version = gcnew DotNetVersion();
			Vhost ^vhost = gcnew Vhost();
			Vdir ^vdir = gcnew Vdir();
			Handlers ^handlers = gcnew Handlers();
			DefaultDocuments ^default_documents = gcnew DefaultDocuments();
			ApplicationPool ^application_pool = gcnew ApplicationPool();

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

			void initFastCGISettings(String ^fullPath)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^fastCgiSection = config->GetSection("system.webServer/fastCgi");

					ConfigurationElementCollection ^fastCgiCollection = fastCgiSection->GetCollection();

					ConfigurationElement ^applicationElement = fastCgiCollection->CreateElement("application");
					applicationElement["fullPath"] = fullPath;
					fastCgiCollection->Add(applicationElement);

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::initCGISettings. Error: " + e->Message, 1105);
				}
			}

			void isapiCgiRestrictions(String ^path, bool allowed, String ^description)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^isapiCgiRestrictionSection = config->GetSection("system.webServer/security/isapiCgiRestriction");

					ConfigurationElementCollection ^isapiCgiRestrictionCollection = isapiCgiRestrictionSection->GetCollection();

					ConfigurationElement ^applicationElement = isapiCgiRestrictionCollection->CreateElement("add");
					applicationElement["path"] = path;
					applicationElement["allowed"] = allowed;
					applicationElement["description"] = description;
					isapiCgiRestrictionCollection->Add(applicationElement);

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::isapiCgiRestrictions. Error: " + e->Message, 1105);
				}
			}

			void setTrust()
			{
				try {

					String ^runtimeVersion = "4.0";
					String ^framework_path;
					String ^machine_config_path;
					String ^web_config_path;
					Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
					RegistryKey ^myKey = localMachine->OpenSubKey("SOFTWARE\\Microsoft\\ASP.NET", true);
					for each (String ^sub_key in myKey->GetSubKeyNames())
					{
						if (Regex::IsMatch(sub_key, runtimeVersion)) {
							RegistryKey ^sk = myKey->OpenSubKey(sub_key);
							framework_path = sk->GetValue("Path")->ToString();
							sk->Close();
							break;
						}
					}
					myKey->Close();
					web_config_path = framework_path + "config\\web.config";
					machine_config_path = framework_path + "config\\machine.config";

					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config =
						serverManager->GetWebConfiguration(gcnew WebConfigurationMap(machine_config_path, web_config_path), nullptr);

					if (config != nullptr) {
						ConfigurationSection ^trustSection = config->GetSection("system.web/trust");
						if (trustSection != nullptr) {
							trustSection->OverrideMode = OverrideMode::Deny;
							trustSection["level"] = "Medium";
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setTrust. Error: " + e->Message, 1105);
				}
			}

			bool checkVirtualDir(String ^vDirApp)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default Web Site");
					if (siteElement == nullptr)
						return false;


					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						return false;


					ConfigurationElementCollection ^applicationCollection = applicationElement->GetCollection();

					ConfigurationElement ^virtualDirectoryElement = FindElement(applicationCollection, "virtualDirectory", "path", "/" + vDirApp);
					if (virtualDirectoryElement == nullptr)
						return false;


					serverManager->CommitChanges();
					delete serverManager;
					return true;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setDefaultSite second section. Error: " + e->Message, 1105);
					return false;
				}
			}

			bool checkVirtualApp(String ^vDirApp)
			{
				bool status = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default Web Site");
					if (siteElement == nullptr)
						status = false;
					else {
						ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

						ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/" + vDirApp);
						if (applicationElement == nullptr)
							status = false;
						else {
							status = true;
						}
					}

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setDefaultSite second section. Error: " + e->Message, 1105);
				}
				return status;

			}

			void setDefaultSite()
			{
				try {
					ServerManager ^serverManagerFirst = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManagerFirst->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default Web Site");
					if (siteElement == nullptr)
						System::Environment::Exit(1000);


					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						throw gcnew InvalidOperationException("Element not found!");

					applicationElement["applicationPool"] = integrated_default_apppool_v4;


					ConfigurationSection ^anonymousAuthenticationSection = config->GetSection("system.webServer/security/authentication/anonymousAuthentication", "Default Web Site");
					anonymousAuthenticationSection["userName"] = "";

					Microsoft::Web::Administration::Configuration ^config_trust = serverManagerFirst->GetWebConfiguration(nullptr);
					config_trust->SetMetadata("defaultManagedRuntimeVersion", "4.0");
					ConfigurationSection ^trustSection = config_trust->GetSection("system.web/trust");
					trustSection["level"] = "Full";
					trustSection->SetMetadata("lockItem", true);
					serverManagerFirst->CommitChanges();
					delete serverManagerFirst;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setDefaultSite first section. Error: " + e->Message, 1105);
				}

				try {
					ServerManager ^serverManagerSecond = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManagerSecond->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default Web Site");
					if (siteElement == nullptr)
						throw gcnew InvalidOperationException("Element not found!");


					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						throw gcnew InvalidOperationException("Element not found!");


					ConfigurationElementCollection ^applicationCollection = applicationElement->GetCollection();

					ConfigurationElement ^virtualDirectoryElement = FindElement(applicationCollection, "virtualDirectory", "path", "/");
					if (virtualDirectoryElement == nullptr)
						throw gcnew InvalidOperationException("Element not found!");

					virtualDirectoryElement["physicalPath"] = "C:\\vhosts\\default";

					serverManagerSecond->CommitChanges();
					delete serverManagerSecond;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setDefaultSite second section. Error: " + e->Message, 1105);
				}
			}

			void setLogSettings()
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElement ^siteDefaultsElement = sitesSection->GetChildElement("siteDefaults");

					ConfigurationElement ^logFileElement = siteDefaultsElement->GetChildElement("logFile");
					logFileElement["logExtFileFlags"] = "Date, Time, ClientIP, UserName, ServerIP, Method, UriStem, UriQuery, HttpStatus, Win32Status, BytesSent, BytesRecv, TimeTaken, ServerPort, UserAgent, HttpSubStatus";

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::setLogSettings. Error: " + e->Message, 1105);
				}
			}

			void clearApplicationPools()
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^applicationPoolsSection = config->GetSection("system.applicationHost/applicationPools");
					for each (ConfigurationElement ^appPool in applicationPoolsSection->GetCollection())
					{
						if (!String::Equals(appPool->GetAttributeValue("Name")->ToString(), "DefaultAppPool", StringComparison::OrdinalIgnoreCase)) {
							appPool->Delete();
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::clearApplicationPools. Error: " + e->Message, 1105);
				}
			}

			void clearWebSites()
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					SiteCollection ^sites = serverManager->Sites;
					for each (ConfigurationElement ^site in sites->GetCollection())
					{
						if (!String::Equals(site->GetAttributeValue("Name")->ToString(), "Default Web Site", StringComparison::OrdinalIgnoreCase)) {
							site->Delete();
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::clearWebSites. Error: " + e->Message, 1105);
				}
			}

			void clearVirtualDirs()
			{
				// For App modes
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", "Default Web Site");
					if (siteElement == nullptr)
						return;

					for each (ConfigurationElement ^app in siteElement->GetCollection())
					{
						if (!String::Equals(app->GetAttributeValue("path")->ToString(), "/", StringComparison::OrdinalIgnoreCase)) {
							app->Delete();
						}
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::clearVirtualDirs. Error: " + e->Message, 1105);
				}

				// For Directory Mode
				try {
					int websiteId = vdir->GetWebSiteId("localhost", "Default Web Site");

					String ^iisPath = String::Format("IIS://{0}/w3svc/{1}", "localhost", websiteId);

					DirectoryEntry ^w3svc = gcnew DirectoryEntry(iisPath);
					w3svc->RefreshCache();

					DirectoryEntry ^folderRoot = w3svc->Children->Find("Root", "IIsWebVirtualDir");
					folderRoot->RefreshCache();
					DirectoryEntry ^virtualDirectory_renamed = nullptr;
					for each (DirectoryEntry ^vd in folderRoot->Children)
					{
						Console::WriteLine("Path: " + vd->Path);
						Console::WriteLine("Name: " + vd->Name);
						folderRoot->Children->Find(vd->Name, "IIsWebVirtualDir");
						folderRoot->Invoke("Delete", gcnew array<String^> {"IIsWebVirtualDir", vd->Name});
					}

					delete folderRoot;
					delete w3svc;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::clearVirtualDirs. Error: " + e->Message, 1105);
				}
			}

			void InitializeInstanceFields()
			{
				String ^file = "C:\\Windows\\System32\\InetSrv\\Microsoft.Web.Administration.dll";
				{
					if (!File::Exists(file)) {
						Console::WriteLine("IIS not installed on this server.");
						System::Environment::Exit(1);
					}
				}
			}

		public:
			void initIIS()
			{
				//Stopwatch stopwatch = new Stopwatch();
				//stopwatch.Start();
				String ^panel_dir = "c:\\fpanel";
				Console::WriteLine("Firewall Setting");
				this->exec("netsh", "advfirewall set global StatefulFtp enable");

				Console::WriteLine("Directories Creating");
				if (!Directory::Exists("C:\\vhosts\\default")) {
					Directory::CreateDirectory("C:\\vhosts\\default");
				}

				if (!Directory::Exists("C:\\inetpub\\logs\\php-logs")) {
					Directory::CreateDirectory("C:\\inetpub\\logs\\php-logs");
				}

				if (!Directory::Exists("C:\\inetpub\\ftproot\\localuser")) {
					Directory::CreateDirectory("C:\\inetpub\\ftproot\\localuser");
					Directory::CreateDirectory("C:\\inetpub\\ftproot\\logs");
				}

				Console::WriteLine("Creating System Groups");
				admin->createSystemGroups();
				Console::WriteLine("Creating System User");
				admin->createSystemUser();
				String ^system_user_password = admin->getPassword();



				// IIS log init
				Console::WriteLine("Initializing IIS Logs");
				iis_logs->initLogs();

				Console::WriteLine("Clearing Web Sites");
				clearWebSites();
				Console::WriteLine("Clearing Application Pools");
				clearApplicationPools();
				Console::WriteLine("Clearing Virtual Directories");
				clearVirtualDirs();

				Console::WriteLine("Creating IIS Application Pools");
				bool status_app_pool = application_pool->createApplicationPool(integrated_default_apppool_v4, "v40", "0", system_user, system_user_password, "Integrated", "off");
				status_app_pool = application_pool->createApplicationPool(classic_default_apppool_v4, "v40", "0", system_user, system_user_password, "Classic", "off");

				status_app_pool = application_pool->createApplicationPool(integrated_default_apppool_v2, "v20", "0", system_user, system_user_password, "Integrated", "off");
				status_app_pool = application_pool->createApplicationPool(classic_default_apppool_v2, "v20", "0", system_user, system_user_password, "Classic", "off");

				status_app_pool = application_pool->createApplicationPool(nomanaged_default_apppool, nullptr, "0", system_user, system_user_password, nullptr, "on");


				Console::WriteLine("Clearing FTP Site");
				ftp->createDefaultFTPSite();

				Console::WriteLine("Configuring FastCGI Settings");
				// Dizinleri kontrol et!!!
				// PHPler
				this->initFastCGISettings(panel_dir + "\\php\\56\\php-cgi.exe");
				this->initFastCGISettings(panel_dir + "\\php\\72\\php-cgi.exe");
				this->initFastCGISettings(panel_dir + "\\php\\74\\php-cgi.exe");
				// Perl
				this->initFastCGISettings(panel_dir + "\\perl\\perl\\bin\\perl.exe");
				// Python
				this->initFastCGISettings(panel_dir + "\\python\\27\\python.exe");
				this->initFastCGISettings(panel_dir + "\\python\\39\\python.exe");

				Console::WriteLine("Configuring Isapi CGI Restrictions");
				// IsapiCgiRestrictions
				this->isapiCgiRestrictions("c:\\fpanel\\perl\\perl\\bin\\perl.exe \"%s\" %s", true, "Perl");
				this->isapiCgiRestrictions("c:\\fpanel\\python\\27\\python.exe \"%s\" %s", true, "Python 2.7");
				this->isapiCgiRestrictions("c:\\fpanel\\python\\39\\python.exe \"%s\" %s", true, "Python 3.9");


				Console::WriteLine("Creating Handlers");
				handlers->createHandler("Default Web Site", "CGI", "*.cgi", "GET,HEAD,POST", "", "CgiModule", panel_dir + "\\perl\\perl\\bin\\perl.exe %s %s", "Either", "Script", "False", "", "4194304");
				handlers->createHandler("Default Web Site", "Perl", "*.pl", "GET,HEAD,POST", "", "CgiModule", panel_dir + "\\perl\\perl\\bin\\perl.exe %s %s", "Either", "Script", "False", "", "4194304");
				handlers->createHandler("Default Web Site", "Python", "*.py", "GET,HEAD,POST", "", "CgiModule", panel_dir + "\\python\\27\\python.exe -u %s %s", "Either", "Script", "False", "", "4194304");
				//handlers->createHandler("Default Web Site", "Python 3.9", "*.py", "GET,HEAD,POST", "", "CgiModule", panel_dir + "\\python\\39\\python.exe -u %s %s", "Either", "Script", "False", "", "4194304");
				handlers->createHandler("Default Web Site", "Php72", "*.php", "GET,HEAD,POST", "", "FastCgiModule", panel_dir + "\\php\\72\\php-cgi.exe", "Either", "Script", "False", "", "4194304");
				//handlers->createHandler("Default Web Site", "Php72", "*.php", "GET,HEAD,POST", "", "FastCgiModule", panel_dir + "\\php\\72\\php-cgi.exe", "Either", "Script", "False", "", "4194304");
				//handlers->createHandler("Default Web Site", "Php74", "*.php", "GET,HEAD,POST", "", "FastCgiModule", panel_dir + "\\php\\74\\php-cgi.exe", "Either", "Script", "False", "", "4194304");

				Console::WriteLine("Configuring Default Web Site");
				this->setDefaultSite();

				Console::WriteLine("Creating Virtual Dirs");
				this->createVirtualDirectory("dbmyadmin", "c:\\vhosts\\dbmyadmin", "vdir");
				this->createVirtualDirectory("webmail", "c:\\vhosts\\webmail_rc", "app");
				this->createVirtualDirectory("mylittleadmin", "c:\\vhosts\\mylittleadmin", "app");

				Console::WriteLine("Configuring Virtual Dirs");
				virtualDirectorySettings("Default Web Site", "dbmyadmin");
				virtualDirectorySettings("Default Web Site", "webmail");
				virtualDirectorySettings("Default Web Site", "mylittleadmin");

				Console::WriteLine("Adding Default Documents");
				default_documents->addDefaultDocuments("Default Web Site", "on", "on", "on", "on", "on", "on");

				Console::WriteLine("ACL Setting");
				FileManager ^file_manager = gcnew FileManager();
				file_manager->AddDirectorySecurity("c:\\vhosts\\webmail_rc", system_user, FileSystemRights::FullControl, AccessControlType::Allow);
				file_manager->ReadAndExecute("c:\\vhosts\\", "IIS_IUSRS", AccessControlType::Allow);
				file_manager->Write("c:\\vhosts\\", "IIS_IUSRS", AccessControlType::Allow);

				Console::WriteLine("Setting Logs");
				setLogSettings();
				//enableAdvancedLogging();
				//stopwatch.Stop();
				//Console.WriteLine("Time elapsed: {0}", stopwatch.Elapsed);
				Console::WriteLine("Configuring Trust Settings");
				setTrust();
			}

			void exec(String ^command, String ^parameters)
			{
				try {
					System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();
					pProcess->StartInfo->CreateNoWindow = true;
					pProcess->StartInfo->RedirectStandardOutput = false;
					pProcess->StartInfo->RedirectStandardError = false;
					pProcess->StartInfo->UseShellExecute = false;

					String ^strCommand = command;
					String ^strCommandParameters = parameters;
					pProcess->StartInfo->FileName = strCommand;
					pProcess->StartInfo->Arguments = strCommandParameters;

					pProcess->EnableRaisingEvents = false;
					pProcess->Start();
					pProcess->WaitForExit();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::exec. Error: " + e->Message, 1105);
				}
			}

			void addMimeType(String ^fileExtension, String ^mimeType)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^staticContentSection = config->GetSection("system.webServer/staticContent");

					ConfigurationElementCollection ^staticContentCollection = staticContentSection->GetCollection();

					ConfigurationElement ^mimeMapElement = staticContentCollection->CreateElement("mimeMap");

					mimeMapElement["fileExtension"] = fileExtension;
					mimeMapElement["mimeType"] = mimeType;
					staticContentCollection->Add(mimeMapElement);

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::addMimeType. Error: " + e->Message, 1105);
				}
			}

			void removeMimeType(String ^fileExtension, String ^mimeType)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^staticContentSection = config->GetSection("system.webServer/staticContent");

					ConfigurationElementCollection ^staticContentCollection = staticContentSection->GetCollection();

					ConfigurationElement ^mimeMapElement = FindElement(staticContentCollection, "mimeMap", "fileExtension", fileExtension);
					if (mimeMapElement == nullptr)
						System::Environment::Exit(7102);

					staticContentCollection->Remove(mimeMapElement);

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::removeMimeType. Error: " + e->Message, 1105);
				}
			}

			bool createVirtualDirectory(String ^vDirName, String ^physicalPath, String ^type)
			{

				try {
					ServerManager ^serverManager = gcnew ServerManager();
					if (type == "vdir") {
						if (checkVirtualDir(vDirName) == true) {
							return false;
						}
						Site ^vDirSite = serverManager->Sites["Default Web Site"];
						vDirSite->Applications[0]->VirtualDirectories->Add("/" + vDirName, physicalPath);
					}
					else {
						if (checkVirtualApp(vDirName) == true) {
							return false;
						}
						Site ^appSite = serverManager->Sites["Default Web Site"];
						Application ^application = appSite->Applications->Add("/" + vDirName, physicalPath);
					}
					serverManager->CommitChanges();
					delete serverManager;

					return true;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::createVirtualDirectory. Error: " + e->Message, 1105);
				}
				return false;
			}

			static void virtualDirectorySettings(String ^siteName, String ^vDirName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^anonymousAuthenticationSection = config->GetSection("system.webServer/security/authentication/anonymousAuthentication", siteName + "/" + vDirName);
					anonymousAuthenticationSection["userName"] = "";

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error WebServer::virtualDirectorySettings. Error: " + e->Message, 1105);
				}
			}

			WebServer()
			{
				InitializeInstanceFields();
			}
		};
	}
}