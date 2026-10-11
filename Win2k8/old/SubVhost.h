#pragma once
#include "Handlers.h"
#include "DefaultDocuments.h"
#include "ApplicationPool.h"
#include "../../env/Env.h"

using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::DirectoryServices;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class SubVhost
		{
		private:
			Handlers ^handlers;
			DefaultDocuments ^default_documents;
			ApplicationPool ^application_pool;
			Environments::Env ^env;
			String ^DIRECTORY_SEPERATOR = "\\";

		public:
			void create(String ^subdomain_name, String ^domain_name, String ^ipaddr, String ^asp, String ^php, String ^aspnet, String ^cgi, String ^perl, String ^python, String ^php_version, String ^aspnet_version)
			{
				String ^path = env->vhosts_dir + domain_name + "subdomains" + DIRECTORY_SEPERATOR + subdomain_name;
				String ^siteName = subdomain_name + '.' + domain_name;
				Boolean bWebsite = IsWebsiteExists(siteName);
				if (!bWebsite) {
					try {
						ServerManager ^serverManager = gcnew ServerManager();
						Site ^site = serverManager->Sites->Add(siteName, "http", ipaddr + ":80:" + siteName, path + DIRECTORY_SEPERATOR + "httpdoc");
						site->ApplicationDefaults->EnabledProtocols = "http,https";

						site->TraceFailedRequestsLogging->Enabled = true;
						site->TraceFailedRequestsLogging->Directory = path + DIRECTORY_SEPERATOR + "httpdoc";

						// Log file initializing 
						Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
						ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");
						ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();
						ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", siteName);

						ConfigurationElement ^logFileElement = siteElement->GetChildElement("logFile");
						logFileElement["directory"] = path + DIRECTORY_SEPERATOR + "stats\\logs";


						// Ayarlar kaydediliyor
						serverManager->CommitChanges();
						setAuthentication(siteName);
						delete serverManager;

						String ^app_pool_name = getDomainApplicationPoolName(domain_name);
						application_pool->assignToSite(siteName, app_pool_name);
						this->makeHandlers(siteName, asp, php, aspnet, cgi, perl, python, php_version, aspnet_version, "create");
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create" + ". Error: " + e->Message, 1105);
					}
				}
				else {
					Logger::errorLog("Error " + this->GetType()->Name + ":create" + ". Error: " + subdomain_name + "." + domain_name + " already exists", 1105);
				}
			}

			void update(String ^subdomain_name, String ^domain_name, String ^ipaddr, String ^asp, String ^php, String ^aspnet, String ^cgi, String ^perl, String ^python, String ^php_version, String ^aspnet_version)
			{
				String ^siteName = subdomain_name + '.' + domain_name;

				Boolean bWebsite = IsWebsiteExists(siteName);

				if (bWebsite) {
					if (ipaddr != nullptr) {
						try {
							ServerManager ^serverManager = gcnew ServerManager();
							serverManager->Sites[siteName]->Bindings->Clear();
							serverManager->Sites[siteName]->Bindings->Add(ipaddr + ":80:" + siteName, "http");
							//serverManager.Sites[siteName].Bindings.Add(ipaddr + ":80:" + "www." + siteName, "http");
							serverManager->CommitChanges();
							delete serverManager;
							this->makeHandlers(siteName, asp, php, aspnet, cgi, perl, python, php_version, aspnet_version, "update");
						}
						catch (Exception ^e) {
							Logger::errorLog("Error " + this->GetType()->Name + ":update" + ". Error: " + e->Message, 1105);
						}
					}
				}
			}

			void remove(String ^subdomain_name, String ^domain_name)
			{
				String ^siteName = subdomain_name + '.' + domain_name;
				Boolean bWebsite = IsWebsiteExists(siteName);

				if (bWebsite) {
					try {
						ServerManager ^serverManager = gcnew ServerManager();
						Site ^site = serverManager->Sites[siteName];
						serverManager->Sites->Remove(site);
						serverManager->CommitChanges();
						delete serverManager;
						application_pool->removeApplicationPool(siteName);
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":remove" + ". Error: " + e->Message, 1105);
					}
				}
			}

			bool startStopSite(String ^subdomain_name, String ^domain_name, String ^state)
			{
				String ^siteName = subdomain_name + '.' + domain_name;

				Boolean bWebsite = IsWebsiteExists(siteName);

				if (bWebsite == true) {
					// var server = new ServerManager();
					// var site = serverMgr.Sites.FirstOrDefault(s => s.Name == siteName);
					String ^webSite = "IIS://" + System::Environment::MachineName + "/W3SVC/" + getSiteId(siteName);
					try {
						DirectoryEntry ^w3svc = gcnew DirectoryEntry(webSite);
						if (state == "suspend") {
							w3svc->Invoke("Stop", nullptr);
						}
						else if (state == "unsuspend") {
							w3svc->Invoke("Start", nullptr);
						}
					}
					catch (Exception ^e) {
						Logger::errorLog("Error " + this->GetType()->Name + ":create" + ". Error: " + e->Message, 1105);
						return false;
					}

					return true;
				}
				System::Environment::Exit(1605);
			}

			void makeHandlers(String ^siteName, String ^asp, String ^php, String ^aspnet, String ^cgi, String ^perl, String ^python, String ^php_version, String ^aspnet_version, String ^process)
			{

				Boolean bWebsite = IsWebsiteExists(siteName);

				if (bWebsite) {
					Dictionary<String^, array<String^>^> ^dictionary = gcnew Dictionary<String^, array<String^>^>();
					if (aspnet == "on") {
						handlers->createAspNet(siteName);
					}
					else {
						handlers->clearHandlers(siteName);
						handlers->createHandler(siteName, "StaticFile", "*", "*", "", "StaticFileModule,DefaultDocumentModule,DirectoryListingModule", "", "Either", "Read", "False", "", "4194304");
					}

					if (asp == "on") {
						handlers->createHandler(siteName, "ASPClassic", "*.asp", "GET,HEAD,POST", "", "IsapiModule", "%windir%\\system32\\inetsrv\\asp.dll", "File", "Script", "False", "", "4194304");
					}
					else {
						handlers->removeHandler(siteName, "ASPClassic");
					}

					if (cgi == "on") {
						handlers->createHandler(siteName, "CGI", "*.cgi", "GET,HEAD,POST", "", "CgiModule", "c:\\fpanel\\perl\\bin\\perl.exe %s %s", "Either", "Script", "False", "", "4194304");
					}
					else {
						handlers->removeHandler(siteName, "CGI");
					}

					if (perl == "on") {
						handlers->createHandler(siteName, "Perl", "*.pl", "GET,HEAD,POST", "", "CgiModule", "c:\\fpanel\\perl\\bin\\perl.exe %s %s", "Either", "Script", "False", "", "4194304");
					}
					else {
						handlers->removeHandler(siteName, "Perl");
					}

					if (python == "on") {
						handlers->createHandler(siteName, "Python", "*.py", "GET,HEAD,POST", "", "CgiModule", "c:\\fpanel\\python\\python.exe -u %s %s", "Either", "Script", "False", "", "4194304");
					}
					else {
						handlers->removeHandler(siteName, "Python");
					}

					if (php == "on") {
						if (process == "create") {
							String ^phpversion = this->phpVersion(php_version);
							handlers->createHandler(siteName, "Php", "*.php", "GET,HEAD,POST", "", "FastCgiModule", "c:\\fpanel\\php\\" + phpversion + "\\php-cgi.exe", "Either", "Script", "False", "", "4194304");
							addPhPCGISettings("c:\\fpanel\\php\\" + phpversion + "\\php-cgi.exe", path, siteName);
							lockPHP(siteName);
						}
						else {
							String ^phpversion = this->phpVersion(php_version);
							handlers->removeHandler(siteName, "Php");
							handlers->createHandler(siteName, "Php", "*.php", "GET,HEAD,POST", "", "FastCgiModule", "c:\\fpanel\\php\\" + phpversion + "\\php-cgi.exe", "Either", "Script", "False", "", "4194304");
						}
					}
					else {
						handlers->removeHandler(siteName, "Php");
					}

					default_documents->addDefaultDocuments(siteName, php, aspnet, asp, perl, cgi, python);
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
					serverManager->CommitChanges();
					delete serverManager;
					return flagset;
				}
				catch (...) {
				}
			}

			static void setEnv(Environments::Env ^_env)
			{
				env = _env;
			}

		private:
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
				catch (...) {
				}
			}

			String ^getDomainApplicationPoolName(String ^domain_name)
			{
				ServerManager ^serverManager = gcnew ServerManager();
				try {
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

					ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

					ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain_name);

					ConfigurationElementCollection ^siteCollection = siteElement->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(siteCollection, "application", "path", "/");
					if (applicationElement == nullptr)
						return nullptr;

					String ^poolName = applicationElement["applicationPool"]->ToString();
					serverManager->CommitChanges();
					delete serverManager;
					return poolName;
				}
				catch (...) {
				}
				return {};
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


			void addPhPCGISettings(String ^fullPath, String ^path, String ^siteName)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^fastCgiSection = config->GetSection("system.webServer/fastCgi");

					ConfigurationElementCollection ^fastCgiCollection = fastCgiSection->GetCollection();

					ConfigurationElement ^applicationElement = fastCgiCollection->CreateElement("application");
					applicationElement["fullPath"] = fullPath;
					applicationElement["arguments"] = "-d open_basedir=" + path;

					ConfigurationElementCollection ^environmentVariablesCollection = applicationElement->GetCollection("environmentVariables");

					ConfigurationElement ^environmentVariableElement = environmentVariablesCollection->CreateElement("environmentVariable");
					environmentVariableElement["name"] = "PHPRC";
					environmentVariableElement["value"] = env->panel_domain_configs_dir + siteName;
					environmentVariablesCollection->Add(environmentVariableElement);
					fastCgiCollection->Add(applicationElement);
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Vhost::addPhPCGISettings. Error: " + e->Message, 1105);
				}
			}

			void removePhPCGISettings(String ^fullPath, String ^path)
			{
				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^fastCgiSection = config->GetSection("system.webServer/fastCgi");

					ConfigurationElementCollection ^fastCgiCollection = fastCgiSection->GetCollection();

					ConfigurationElement ^applicationElement = FindElement(fastCgiCollection,
						"application",
						"fullPath", fullPath,
						"arguments", "-d open_basedir=" + path);
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


			static String ^getSiteId(String ^siteName)
			{
				DirectoryEntry ^w3svc = gcnew DirectoryEntry("IIS://localhost/w3svc");
				String ^siteId = "";
				for each (DirectoryEntry ^de in w3svc->Children)
				{
					PropertyValueCollection ^dep = de->Properties["ServerComment"];
					if (de->SchemaClassName == "IIsWebServer" && dep[0]->ToString() == siteName) {
						siteId = de->Name;
					}


				}
				return siteId;
			}


		private:
			void InitializeInstanceFields()
			{
				handlers = gcnew Handlers();
				default_documents = gcnew DefaultDocuments();
				application_pool = gcnew ApplicationPool();
			}

		public:
			SubVhost()
			{
				InitializeInstanceFields();
			}
		};
	}
}