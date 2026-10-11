#pragma once
#include "Handlers.h"
#include "ApplicationPool.h"
#include "DefaultDocuments.h"
#include "Vdir.h"
#include "Logger.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::DirectoryServices;
using namespace System::IO;
using namespace System::Threading;
using namespace Microsoft::Web::Administration;
using namespace Environments;

namespace FPanel
{
	namespace Windows
	{
		private ref class Vhost
		{
		private:
			Handlers ^handlers;
			DefaultDocuments ^defaultDocuments;
			ApplicationPool ^applicationPool;
			Vdir ^vd;
			Environments::Env ^env;

		public:
			void createSite(String ^siteName, String ^appPool, String ^appPoolMode, String ^path, 
							String ^ipaddr, String ^cpu_limit, String ^asp, String ^php, String ^aspnet, 
							String ^cgi, String ^perl, String ^python, String ^php_version, String ^aspnet_version,
							String ^username, String ^password, String ^prefix, String ^aspNetCore
				)
			{
				Boolean bWebsite = IsWebsiteExists(siteName);
				if (!bWebsite) {
					try {
						String ^app_pool_name;
						ServerManager ^serverManager = gcnew ServerManager();
						Site ^site = serverManager->Sites->Add(siteName, "http", ipaddr + ":80:" + siteName, path + "\\vhost\\httpdoc");
						serverManager->Sites[siteName]->Bindings->Add(ipaddr + ":80:" + "www." + siteName, "http");
						site->ApplicationDefaults->EnabledProtocols = "http,https";

						site->TraceFailedRequestsLogging->Enabled = true;
						site->TraceFailedRequestsLogging->Directory = path + "\\vhost\\httpdoc";

						// Log file initializing 
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
						ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
						ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);

						ConfigurationElement ^logFileElement = siteElement->GetChildElement("logFile");
						logFileElement["directory"] = path + "\\stats\\logs";
						// Ayarlar kaydediliyor

						serverManager->CommitChanges();
						delete serverManager;

						setAuthentication(siteName);
						if (aspNetCore == "on") {
							if (appPool != "on") {
								app_pool_name = env->nomanaged_default_apppool;
							}
							else {
								app_pool_name = siteName + "(No Managed Pool)";
								applicationPool->createApplicationPool(app_pool_name, aspnet_version, cpu_limit, username, password, appPoolMode, aspNetCore);
							}
						}
						else {
							if (appPool != "on") {
								if (aspnet_version == "v40") {
									if (appPoolMode == "integrated")
										app_pool_name = env->integrated_default_apppool_v4;
									else
										app_pool_name = env->classic_default_apppool_v4;
								}
								else {
									if (appPoolMode == "integrated")
										app_pool_name = env->integrated_default_apppool_v2;
									else
										app_pool_name = env->classic_default_apppool_v2;
								}
							}
							else {
								app_pool_name = siteName + "(Application Pool)";
								applicationPool->createApplicationPool(app_pool_name, aspnet_version, cpu_limit, username, password, appPoolMode, aspNetCore);
							}
						}

						applicationPool->assignToSite(siteName, app_pool_name);

						setLogSettings(siteName);

						this->removeWebConfig(path + "\\vhost\\httpdoc");

						this->makeHandlers(siteName, asp, php, aspnet, cgi, perl, python, php_version, aspnet_version, "create", prefix, aspNetCore);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Vhost::createSite. Error: " + e->Message, 1105);
					}
					lockTrust(siteName);
				}
				else {
					Logger::errorLog("Error Vhost::createSite. Error " + siteName + " already exists", 1105);
				}
			}

			bool updateSite(String ^siteName, String ^appPool, String ^appPoolMode, String ^ipaddr, String ^asp, String ^php, 
				String ^aspnet, String ^cgi, String ^perl, String ^python, String ^php_version, String ^aspnet_version, 
				String ^cpu_limit, String ^username, String ^password, String ^prefix, String ^aspNetCore)
			{
				try {
					String ^app_pool_name;
					Boolean bWebsite = IsWebsiteExists(siteName);
					if (bWebsite) {
						if (ipaddr != nullptr) {
							ServerManager ^serverManager = gcnew ServerManager();
							serverManager->Sites[siteName]->Bindings->Clear();
							serverManager->Sites[siteName]->Bindings->Add(ipaddr + ":80:" + siteName, "http");
							serverManager->Sites[siteName]->Bindings->Add(ipaddr + ":80:" + "www." + siteName, "http");
							serverManager->CommitChanges();
							delete serverManager;
						}
						this->makeHandlers(siteName, asp, php, aspnet, cgi, perl, python, php_version, aspnet_version, "update", prefix, aspNetCore);
						if (aspNetCore == "on") {
							if (appPool != "on") {
								app_pool_name = env->nomanaged_default_apppool;
							}
							else {
								app_pool_name = siteName + "(No Managed Pool)";
								applicationPool->createApplicationPool(app_pool_name, aspnet_version, cpu_limit, username, password, appPoolMode, aspNetCore);
							}
							applicationPool->assignToSite(siteName, app_pool_name);
							applicationPool->removeApplicationPool(siteName + "(Application Pool)");
						}
						else {
							if (appPool != "on") {
								if (aspnet_version == "v40") {
									if (appPoolMode == "integrated")
										app_pool_name = "FpanelAppPoolv4Integrated";
									else
										app_pool_name = "FpanelAppPoolv4Classic";
								}
								else {
									if (appPoolMode == "integrated")
										app_pool_name = "FpanelAppPoolv2Integrated";
									else
										app_pool_name = "FpanelAppPoolv2Classic";
								}
								applicationPool->assignToSite(siteName, app_pool_name);
								applicationPool->removeApplicationPool(siteName + "(Application Pool)");
							}
							else {
								if (applicationPool->checkPool(siteName + "(Application Pool)") == true) {
									applicationPool->updateApplicationPool(siteName + "(Application Pool)", appPoolMode, aspnet_version, cpu_limit, username, password, aspNetCore);
								}
								else {
									applicationPool->createApplicationPool(siteName + "(Application Pool)", appPoolMode, aspnet_version, cpu_limit, username, password, aspNetCore);
									applicationPool->assignToSite(siteName, siteName + "(Application Pool)");
								}
							}
						}

						return true;
					}
					return false;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::updateSite. Error: " + e->Message, 1105);
					return false;
				}
			}

			bool removeSite(String ^siteName)
			{
				try {
					Boolean bWebsite = IsWebsiteExists(siteName);
					if (bWebsite) {
						String ^path = env->vhosts_dir + siteName;
						ServerManager ^serverManager = gcnew ServerManager();
						Site ^site = serverManager->Sites[siteName];
						serverManager->Sites->Remove(site);
						serverManager->CommitChanges();
						applicationPool->removeApplicationPool(siteName);
						delete serverManager;
						removePhPCGISettings("c:\\fpanel\\php\\56\\php-cgi.exe", siteName);
						removePhPCGISettings("c:\\fpanel\\php\\72\\php-cgi.exe", siteName);
						removePhPCGISettings("c:\\fpanel\\php\\74\\php-cgi.exe", siteName);
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::removeSite. Error: " + e->Message, 1105);
					return false;
				}

				return true;
			}

			bool startStopSite(String ^siteName, String ^state)
			{
				try {
					Boolean bWebsite = IsWebsiteExists(siteName);

					if (bWebsite == true) {
						String ^webSite = "IIS://" + System::Environment::MachineName + "/W3SVC/" + getSiteId(siteName);
						DirectoryEntry ^w3svc = gcnew DirectoryEntry(webSite);
						if (state == "suspend") {
							w3svc->Invoke("Stop", nullptr);
						}
						else if (state == "unsuspend") {
							w3svc->Invoke("Start", nullptr);
						}
						return true;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::startStopSite. Error: " + e->Message, 1105);
				}

				return false;
			}

			void makeHandlers(String ^siteName, String ^asp, String ^php, String ^aspnet, 
				String ^cgi, String ^perl, String ^python, 
				String ^php_version, String ^aspnet_version, String ^process, String ^prefix, String ^aspNetCore)
			{

					Boolean bWebsite = IsWebsiteExists(siteName);
					if (bWebsite) {
					// Hata ara
					Dictionary<String^, array<String^>^> ^dictionary = gcnew Dictionary<String^, array<String^>^>();
					
					if (String::Equals(aspnet, "on")) {
						handlers->createAspNet(siteName);
					}
					else {
						handlers->clearHandlers(siteName);
						handlers->createHandler(siteName,
							"StaticFile",
							"*",
							"*",
							"",
							"StaticFileModule,DefaultDocumentModule,DirectoryListingModule",
							"",
							"Either",
							"Read",
							"False",
							"",
							"4194304");
					}

					//Console::WriteLine("asp: {0}", asp);
					if (String::Equals(asp, "on")) {
						handlers->createHandler(siteName,
							"ASPClassic",
							"*.asp",
							"GET,HEAD,POST",
							"",
							"IsapiModule",
							"%windir%\\system32\\inetsrv\\asp.dll",
							"File",
							"Script",
							"False",
							"",
							"4194304");
					}
					else {
						handlers->removeHandler(siteName, "ASPClassic");
					}

					// String ^siteName, String ^handlerName, String ^path,
					// String ^verb, String ^type, String ^modules, String ^scriptProcessor,
					// String ^resourceType, String ^requireAccess, String ^allowPathInfo,
					// String ^preCondition, String ^responseBufferLimit
					if (String::Equals(aspNetCore, "on")) {
						handlers->createHandler(siteName, "ASPNetCore", "*",
							"GET,HEAD,POST", nullptr, "AspNetCoreModuleV2", nullptr, 
							"Unspecified", nullptr, nullptr,
							nullptr, "4194304");
					}
					else {
						handlers->removeHandler(siteName, "ASPNetCore");
					}

					//Console::WriteLine("cgi: {0}", cgi);
					if (String::Equals(cgi, "on")) {
						handlers->createHandler(siteName,
							"CGI",
							"*.cgi",
							"GET,HEAD,POST",
							"",
							"CgiModule",
							"c:\\fpanel\\perl\\perl\\bin\\perl.exe %s %s",
							"Either",
							"Script",
							"False",
							"",
							"4194304");
					}
					else {
						handlers->removeHandler(siteName, "CGI");
					}

				
					//Console::WriteLine("perl: {0}", perl);
					if (String::Equals(perl, "on")) {
						handlers->createHandler(siteName,
							"Perl",
							"*.pl",
							"GET,HEAD,POST",
							"",
							"CgiModule",
							"c:\\fpanel\\perl\\perl\\bin\\perl.exe %s %s",
							"Either",
							"Script",
							"False",
							"",
							"4194304");
					}
					else {
						handlers->removeHandler(siteName, "Perl");
					}

		
					//Console::WriteLine("python: {0}", python);
					if (String::Equals(python, "on")) {
						String ^pythonPath;
						if (File::Exists(env->panel_dir + "python\\39")) {
							pythonPath = env->panel_dir + "python\\39\\python.exe";
						}
						else {
							pythonPath = env->panel_dir + "python\\27\\python.exe";
						}
						handlers->createHandler(siteName,
							"Python",
							"*.py",
							"GET,HEAD,POST",
							"",
							"CgiModule",
							pythonPath + " -u %s %s",
							"Either",
							"Script",
							"False",
							"",
							"4194304");
					}
					else {
						handlers->removeHandler(siteName, "Python");
					}
		
					//Console::WriteLine("php: {0}", php);
					//Console::WriteLine("php_version: {0}", php_version);
					if (String::Equals(php, "on")) {
						String ^phpversion = this->phpVersion(php_version);
						if (!File::Exists(env->panel_domain_configs_dir + siteName + "\\php\\php.ini")) {
							File::Copy(env->panel_dir + "\\php\\" + phpversion + "\\php.ini", env->panel_domain_configs_dir + siteName + "\\php\\php.ini");
						}

						if (String::Equals(process, "create")) {
							handlers->createHandler(siteName,
								"Php" + "-" + siteName,
								"*.php",
								"GET,HEAD,POST",
								"",
								"FastCgiModule",
								env->panel_dir + "php\\" + phpversion + "\\php-cgi.exe|-c " + env->panel_domain_configs_dir + siteName + "\\php\\php.ini",
								"Either", "Script",
								"False",
								"",
								"4194304");
							addPhPCGISettings(env->panel_dir + "php\\" + phpversion + "\\php-cgi.exe", siteName, prefix);
							lockPHP(siteName);
						}
						else {
							if (File::Exists(env->panel_domain_configs_dir + siteName + "\\php\\php.ini")) {
								File::Delete(env->panel_domain_configs_dir + siteName + "\\php\\php.ini");
							}
							handlers->removeHandler(siteName, "Php" + "-" + siteName);
							handlers->createHandler(siteName,
								"Php" + "-" + siteName,
								"*.php",
								"GET,HEAD,POST",
								"",
								"FastCgiModule",
								env->panel_dir + "php\\" + phpversion + "\\php-cgi.exe|-c " + env->panel_domain_configs_dir + siteName + "\\php\\php.ini",
								"Either",
								"Script",
								"False",
								"",
								"4194304");
							removePhPCGISettings(env->panel_dir + "php\\56\\php-cgi.exe", siteName);
							removePhPCGISettings(env->panel_dir + "php\\72\\php-cgi.exe", siteName);
							removePhPCGISettings(env->panel_dir + "php\\74\\php-cgi.exe", siteName);
							addPhPCGISettings(env->panel_dir + "php\\" + phpversion + "\\php-cgi.exe", siteName, prefix);
							lockPHP(siteName);
						}
					}
					else {
						handlers->removeHandler(siteName, "Php");
						removePhPCGISettings(env->panel_dir + "php\\56\\php-cgi.exe", siteName);
						removePhPCGISettings(env->panel_dir + "php\\72\\php-cgi.exe", siteName);
						removePhPCGISettings(env->panel_dir + "php\\74\\php-cgi.exe", siteName);
					}
					defaultDocuments->addDefaultDocuments(siteName, php, aspnet, asp, perl, cgi, python);
				}
			}

			String ^phpVersion(String ^php_version)
			{

				if (php_version->ToLower() == "php56") {
					String ^phpversion = "56";
					return phpversion;
				}
				else if (php_version->ToLower() == "php72") {
					String ^phpversion = "72";
					return phpversion;
				}
				else if (php_version->ToLower() == "php74") {
					String ^phpversion = "74";
					return phpversion;
				}
				else {
					String ^phpversion = "56";
					return phpversion;
				}
			}

			bool IsWebsiteExists(String ^strWebsitename)
			{
				Boolean flagset = false;
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					SiteCollection ^sitecollection = serverManager->Sites;
					for each (Site ^site in sitecollection)
					{
						if (site->Name == strWebsitename->ToString()) {
							flagset = true;
							break;
						}
						else {
							flagset = false;
						}
					}
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::IsWebsiteExists. Error: " + e->Message, 1105);
				}
				return flagset;

			}

			void setEnv(Environments::Env ^_env)
			{
				env = _env;
			}

		private:
			void removeWebConfig(String ^path)
			{
				if (System::IO::File::Exists(path + "\\vhost\\httpdoc\\web.config")) {
					try {
						System::IO::File::Delete(path + "\\vhost\\httpdoc\\web.config");
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Vhost::removeWebConfig. Error: " + e->Message, 1105);
					}
				}
			}

			static void setAuthentication(String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^anonymousAuthenticationSection = config->GetSection("system.webServer/security/authentication/anonymousAuthentication", siteName);
					anonymousAuthenticationSection["userName"] = "";
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::setAuthentication. Error: " + e->Message, 1105);
				}
			}

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

			static String ^getSiteId(String ^siteName)
			{
				String ^siteId = "";
				try {
					DirectoryEntry ^w3svc = gcnew DirectoryEntry("IIS://localhost/w3svc");
					for each (DirectoryEntry ^de in w3svc->Children)
					{
						PropertyValueCollection ^dep = de->Properties["ServerComment"];
						if (de->SchemaClassName == "IIsWebServer" && dep[0]->ToString() == siteName) {
							siteId = de->Name;
						}
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::getSiteId. Error: " + e->Message, 1105);
				}
				return siteId;
			}

			void setLogSettings(String ^domain)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain);
					if (siteElement == nullptr)
						throw gcnew InvalidOperationException("Element not found!");


					ConfigurationElement ^logFileElement = siteElement->GetChildElement("logFile");
					logFileElement["logExtFileFlags"] = "Date, Time, ClientIP, UserName, ServerIP, Method, UriStem, UriQuery, HttpStatus, Win32Status, BytesSent, BytesRecv, TimeTaken, ServerPort, UserAgent, HttpSubStatus";

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::setLogSettings. Error: " + e->Message, 1105);
				}
			}

			bool lockTrust(String ^siteName)
			{
				String ^framework_path = handlers->findNetVersion("v4");
				String ^web_config_path = framework_path + "config\\web.config";
				String ^machine_config_path = framework_path + "config\\machine.config";

				if (!File::Exists(web_config_path)) {
					Console::WriteLine(web_config_path + " doesnt exists");
					return false;
				}
				if (!File::Exists(machine_config_path)) {
					Console::WriteLine(machine_config_path + " doesnt exists");
					return false;
				}
	
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config =
					serverManager->GetWebConfiguration(gcnew WebConfigurationMap(machine_config_path, web_config_path), nullptr);

					if (config != nullptr) {
						ConfigurationSection ^trustSection = config->GetSection("system.web/trust", siteName);
						if (trustSection != nullptr) {
							//Console::WriteLine(trustSection->OverrideMode.ToString());
							//Console::WriteLine(trustSection["level"]);
							trustSection->OverrideMode = OverrideMode::Deny;
							trustSection["level"] = "Medium";
						}
						serverManager->CommitChanges();
					}
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::lockTrust::setIIS. Error: " + e->Message, 1105);
					return false;
				}
				return true;
			}

			void addPhPCGISettings(String ^phpPath, String ^siteName, String ^prefix)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^fastCgiSection = config->GetSection("system.webServer/fastCgi");

					ConfigurationElementCollection ^fastCgiCollection = fastCgiSection->GetCollection();

					ConfigurationElement ^applicationElement = fastCgiCollection->CreateElement("application");
					applicationElement["fullPath"] = phpPath;
					applicationElement["arguments"] = "-c " + env->panel_domain_configs_dir + siteName + "\\php\\php.ini";
					
					ConfigurationElementCollection ^environmentVariablesCollection = applicationElement->GetCollection("environmentVariables");

					ConfigurationElement ^environmentVariableElement = environmentVariablesCollection->CreateElement("environmentVariable");
					environmentVariableElement["name"] = "PHPRC";
					environmentVariableElement["value"] = env->vhosts_dir + prefix + "\\vhost";
					environmentVariablesCollection->Add(environmentVariableElement);
					fastCgiCollection->Add(applicationElement);
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::addPhPCGISettings. Error: " + e->Message, 1105);
				}
			}

			void removePhPCGISettings(String ^phpPath, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^fastCgiSection = config->GetSection("system.webServer/fastCgi");

					ConfigurationElementCollection ^fastCgiCollection = fastCgiSection->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(fastCgiCollection, 
						"application", 
						"fullPath", phpPath,
						"arguments", "-c " + env->panel_domain_configs_dir + siteName + "\\php\\php.ini");
					if (applicationElement != nullptr)
						fastCgiCollection->Remove(applicationElement);

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::removePhPCGISettings. Error: " + e->Message, 1105);
				}
			}
			
			void lockPHP(String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName);

					ConfigurationElementCollection ^handlersCollection = handlersSection->GetCollection();

					ConfigurationElement ^addElement = FindElement(handlersCollection, "add", "name", "Php");
					if (addElement != nullptr) {
						addElement->SetMetadata("lockItem", true);
					}

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::lockPHP. Error: " + e->Message, 1105);
				}
			}
			
			void InitializeInstanceFields()
			{
				handlers = gcnew Handlers();
				defaultDocuments = gcnew DefaultDocuments();
				applicationPool = gcnew ApplicationPool();
				vd = gcnew Vdir();
			}

		public:
			Vhost()
			{
				InitializeInstanceFields();
			}
		};
	}
}