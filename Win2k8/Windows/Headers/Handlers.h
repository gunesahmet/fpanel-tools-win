#pragma once
#include "Logger.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::Xml;
using namespace System::Text::RegularExpressions;
using namespace System::Threading;
using namespace System::IO;

namespace FPanel
{
	namespace Windows
	{
		private ref class PublicTokenKey
		{
		public:
			String ^key;
			String ^framework;
			String ^netversion;
			String ^frameworkPath;
		};

		private ref class Handlers
		{
		public:
			bool createHandler(String ^siteName, String ^handlerName, String ^path,
				String ^verb, String ^type, String ^modules, String ^scriptProcessor,
				String ^resourceType, String ^requireAccess, String ^allowPathInfo,
				String ^preCondition, String ^responseBufferLimit)
			{
				//this->checkFileStatus();

				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName);

					ConfigurationElementCollection ^handlersCollection = handlersSection->GetCollection();

					ConfigurationElement ^findElement = FindElement(handlersCollection, "add", "name", handlerName);

					if (findElement != nullptr) {
						serverManager->CommitChanges();
						return true;
					}
					else {
						ConfigurationElement ^addElement = handlersCollection->CreateElement("add");

						addElement["name"] = handlerName;

						addElement["path"] = "" + path + "";

						addElement["verb"] = "" + verb + "";

						if (type != nullptr)
							addElement["type"] = "" + type + "";

						addElement["modules"] = "" + modules + "";

						if (type != nullptr)
							addElement["scriptProcessor"] = "" + scriptProcessor + "";

						addElement["resourceType"] = "" + resourceType + "";

						if (type != nullptr)
							addElement["requireAccess"] = "" + requireAccess + "";

						if (type != nullptr)
							addElement["allowPathInfo"] = "" + allowPathInfo + "";

						if (type != nullptr)
							addElement["preCondition"] = "" + preCondition + "";

						if (responseBufferLimit != nullptr) {
							addElement["responseBufferLimit"] = Convert::ToInt32(responseBufferLimit);
						}

						handlersCollection->AddAt(0, addElement);
						serverManager->CommitChanges();
						delete serverManager;
						return true;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Handlers::createHandler. Error: " + e->Message, 1105);
				}
				return false;
			}

			bool checkFileStatus()
			{
				bool state = false;
				while (true) {
					try {
						FileStream ^fs = gcnew FileStream("c:\\windows\\system32\\inetsrv\\config\\applicationhost.config", FileMode::Open);
						state = true;
						delete fs;
						break;
					}
					catch (Exception ^e) {
						Logger::errorLog("Error Handlers::checkFileStatus. Error: " + e->Message, 1105);
					}
					Thread::Sleep(100);
				}
				return state;

			}

			bool clearHandlers(String ^siteName)
			{
				//this->checkFileStatus();

				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();
					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName);
					ConfigurationElementCollection ^handlersCollection = handlersSection->GetCollection();
					handlersCollection->Clear();

					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Handlers::clearHandlers. Error: " + e->Message, 1105);
				}

				return true;
			}

			bool createAspNet(String ^siteName)
			{
				String ^frameworkVersion2_32 = this->findNetVersion("v2")->Replace("Framework64", "Framework");
				String ^frameworkVersion2_64 = this->findNetVersion("v2");
				
				String ^frameworkVersion4_32 = this->findNetVersion("v4")->Replace("Framework64","Framework");
				String ^frameworkVersion4_64 = this->findNetVersion("v4");

				Dictionary<String^, array<String^>^> ^dictionary = gcnew Dictionary<String^, array<String^>^>();
				// Name, Path, Verb, Type, Modules, scriptProcessor, ResourceType, requireAccess, allowPathInfo, PreCondition, responseBufferLimit, EntryPath
				// For x64 4.0
				if (frameworkVersion4_64 != nullptr) {
					dictionary->Add("AXD-ISAPI-4.0_64bit", gcnew array<String^> {"AXD-ISAPI-4.0_64bit", "*.axd", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("PageHandlerFactory-ISAPI-4.0_64bit", gcnew array<String^> {"PageHandlerFactory-ISAPI-4.0_64bit", "*.aspx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("SimpleHandlerFactory-ISAPI-4.0_64bit", gcnew array<String^> {"SimpleHandlerFactory-ISAPI-4.0_64bit", "*.ashx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("WebServiceHandlerFactory-ISAPI-4.0_64bit", gcnew array<String^> {"WebServiceHandlerFactory-ISAPI-4.0_64bit", "*.asmx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-ISAPI-4.0_64bit", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-ISAPI-4.0_64bit", "*.rem", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-ISAPI-4.0_64bit", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-ISAPI-4.0_64bit", "*.soap", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("svc-ISAPI-4.0_64bit", gcnew array<String^> {"svc-ISAPI-4.0_64bit", "*.svc", "*", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("rules-ISAPI-4.0_64bit", gcnew array<String^> {"rules-ISAPI-4.0_64bit", "*.rules", "*", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xoml-ISAPI-4.0_64bit", gcnew array<String^> {"xoml-ISAPI-4.0_64bit", "*.xoml", "*", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xamlx-ISAPI-4.0_64bit", gcnew array<String^> {"xamlx-ISAPI-4.0_64bit", "*.xamlx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("aspq-ISAPI-4.0_64bit", gcnew array<String^> {"aspq-ISAPI-4.0_64bit", "*.aspq", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtm-ISAPI-4.0_64bit", gcnew array<String^> {"cshtm-ISAPI-4.0_64bit", "*.cshtm", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtml-ISAPI-4.0_64bit", gcnew array<String^> {"cshtml-ISAPI-4.0_64bit", "*.cshtml", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtm-ISAPI-4.0_64bit", gcnew array<String^> {"vbhtm-ISAPI-4.0_64bit", "*.vbhtm", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtml-ISAPI-4.0_64bit", gcnew array<String^> {"vbhtml-ISAPI-4.0_64bit", "*.vbhtml", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("ExtensionlessUrlHandler-ISAPI-4.0_64bit", gcnew array<String^> {"ExtensionlessUrlHandler-ISAPI-4.0_64bit", "*.", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});

					// With publicTokenKey
					dictionary->Add("WebServiceHandlerFactory-Integrated-4.0", gcnew array<String^> {"WebServiceHandlerFactory-Integrated-4.0", "*.asmx", "GET,HEAD,POST,DEBUG", "System.Web.Script.Services.ScriptHandlerFactory, System.Web.Extensions, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.asmx", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-Integrated-4.0", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-Integrated-4.0", "*.rem", "GET,HEAD,POST,DEBUG", "System.Runtime.Remoting.Channels.Http.HttpRemotingHandlerFactory, System.Runtime.Remoting, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.rem", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-Integrated-4.0", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-Integrated-4.0", "*.soap", "GET,HEAD,POST,DEBUG", "System.Runtime.Remoting.Channels.Http.HttpRemotingHandlerFactory, System.Runtime.Remoting, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.soap", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("svc-Integrated-4.0", gcnew array<String^> {"svc-Integrated-4.0", "*.svc", "*", "System.ServiceModel.Activation.ServiceHttpHandlerFactory, System.ServiceModel.Activation, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.svc", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("integratedMode,runtimeVersionv4.0", gcnew array<String^> {"integratedMode,runtimeVersionv4.0", "*.rules", "*", "System.ServiceModel.Activation.ServiceHttpHandlerFactory, System.ServiceModel.Activation, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.svc", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xoml-Integrated-4.0", gcnew array<String^> {"xoml-Integrated-4.0", "*.xoml", "*", "System.ServiceModel.Activation.ServiceHttpHandlerFactory, System.ServiceModel.Activation, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.xoml", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xamlx-Integrated-4.0", gcnew array<String^> {"xamlx-Integrated-4.0", "*.xamlx", "GET,HEAD,POST,DEBUG", "System.Xaml.Hosting.XamlHttpHandlerFactory, System.Xaml.Hosting, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.xamlx", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("ScriptResourceIntegrated-4.0", gcnew array<String^> {"ScriptResourceIntegrated-4.0", "ScriptResource.axd", "GET,HEAD", "System.Web.Handlers.ScriptResourceHandler, System.Web.Extensions, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("ScriptResource.axd", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("ScriptHandlerFactoryAppServices-Integrated-4.0", gcnew array<String^> {"ScriptHandlerFactoryAppServices-Integrated-4.0", "*_AppService.axd", "*", "System.Web.Script.Services.ScriptHandlerFactory, System.Web.Extensions, Version=4.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*_AppService.axd", "Framework64", frameworkVersion4_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});

				}

				if (frameworkVersion4_32 != nullptr) {
					// For x86 4.0
					dictionary->Add("AXD-ISAPI-4.0_32bit", gcnew array<String^> {"AXD-ISAPI-4.0_32bit", "*.axd", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("PageHandlerFactory-ISAPI-4.0_32bit", gcnew array<String^> {"PageHandlerFactory-ISAPI-4.0_32bit", "*.aspx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("SimpleHandlerFactory-ISAPI-4.0_32bit", gcnew array<String^> {"AXDSimpleHandlerFactory-ISAPI-4.0_32bit", "*.ashx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("WebServiceHandlerFactory-ISAPI-4.0_32bit", gcnew array<String^> {"WebServiceHandlerFactory-ISAPI-4.0_32bit", "*.asmx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-ISAPI-4.0_32bit", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-ISAPI-4.0_32bit", "*.rem", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-ISAPI-4.0_32bit", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-ISAPI-4.0_32bit", "*.soap", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("svc-ISAPI-4.0_32bit", gcnew array<String^> {"svc-ISAPI-4.0_32bit", "*.svc", "*", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("rules-ISAPI-4.0_32bit", gcnew array<String^> {"rules-ISAPI-4.0_32bit", "*.rules", "*", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xoml-ISAPI-4.0_32bit", gcnew array<String^> {"xoml-ISAPI-4.0_32bit", "*.xoml", "*", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("xamlx-ISAPI-4.0_32bit", gcnew array<String^> {"xamlx-ISAPI-4.0_32bit", "*.xamlx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("aspq-ISAPI-4.0_32bit", gcnew array<String^> {"aspq-ISAPI-4.0_32bit", "*.aspq", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtm-ISAPI-4.0_32bit", gcnew array<String^> {"cshtm-ISAPI-4.0_32bit", "*.cshtm", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtml-ISAPI-4.0_32bit", gcnew array<String^> {"cshtml-ISAPI-4.0_32bit", "*.cshtml", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtm-ISAPI-4.0_32bit", gcnew array<String^> {"vbhtm-ISAPI-4.0_32bit", "*.vbhtm", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtml-ISAPI-4.0_32bit", gcnew array<String^> {"vbhtml-ISAPI-4.0_32bit", "*.vbhtml", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("ExtensionlessUrlHandler-ISAPI-4.0_32bit", gcnew array<String^> {"ExtensionlessUrlHandler-ISAPI-4.0_32bit", "*.", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion4_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv4.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
				}

				if ((frameworkVersion4_64 != nullptr) || (frameworkVersion4_32 != nullptr)) {
					// For None 4.0
					dictionary->Add("TraceHandler-Integrated-4.0", gcnew array<String^> {"TraceHandler-Integrated-4.0", "trace.axd", "GET,HEAD,POST,DEBUG", "System.Web.Handlers.TraceHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("WebAdminHandler-Integrated-4.0", gcnew array<String^> {"WebAdminHandler-Integrated-4.0", "WebAdmin.axd", "GET,DEBUG", "System.Web.Handlers.WebAdminHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("AssemblyResourceLoader-Integrated-4.0", gcnew array<String^> {"AssemblyResourceLoader-Integrated-4.0", "WebResource.axd", "GET,DEBUG", "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("PageHandlerFactory-Integrated-4.0", gcnew array<String^> {"PageHandlerFactory-Integrated-4.0", "*.aspx", "GET,HEAD,POST,DEBUG", "System.Web.UI.PageHandlerFactory", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("SimpleHandlerFactory-Integrated-4.0", gcnew array<String^> {"SimpleHandlerFactory-Integrated-4.0", "*.ashx", "GET,HEAD,POST,DEBUG", "System.Web.UI.SimpleHandlerFactory", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("aspq-Integrated-4.0", gcnew array<String^> {"aspq-Integrated-4.0", "*.aspq", "GET,HEAD,POST,DEBUG", "System.Web.HttpForbiddenHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtm-Integrated-4.0", gcnew array<String^> {"cshtm-Integrated-4.0", "*.cshtm", "GET,HEAD,POST,DEBUG", "System.Web.HttpForbiddenHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("cshtml-Integrated-4.0", gcnew array<String^> {"cshtml-Integrated-4.0", "*.xshtml", "GET,HEAD,POST,DEBUG", "System.Web.HttpForbiddenHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtm-Integrated-4.0", gcnew array<String^> {"vbhtm-Integrated-4.0", "*.vbhtm", "GET,HEAD,POST,DEBUG", "System.Web.HttpForbiddenHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("vbhtml-Integrated-4.0", gcnew array<String^> {"vbhtml-Integrated-4.0", "*.vbhtml", "GET,HEAD,POST,DEBUG", "System.Web.HttpForbiddenHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("ExtensionlessUrlHandler-Integrated-4.0", gcnew array<String^> {"ExtensionlessUrlHandler-Integrated-4.0", "*.", "GET,HEAD,POST,DEBUG", "System.Web.Handlers.TransferRequestHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv4.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
				}

				if (frameworkVersion2_32 != nullptr) {
					// For x86 2.0
					dictionary->Add("AXD-ISAPI-2.0", gcnew array<String^> {"AXD-ISAPI-2.0", "*.axd", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("PageHandlerFactory-ISAPI-2.0", gcnew array<String^> {"PageHandlerFactory-ISAPI-2.0", "*.aspx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("SimpleHandlerFactory-ISAPI-2.0", gcnew array<String^> {"SimpleHandlerFactory-ISAPI-2.0", "*.ashx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("WebServiceHandlerFactory-ISAPI-2.0", gcnew array<String^> {"WebServiceHandlerFactory-ISAPI-2.0", "*.asmx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-ISAPI-2.0", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-ISAPI-2.0", "*.rem", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-ISAPI-2.0", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-ISAPI-2.0", "*.soap", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_32 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness32", "0", "MACHINE/WEBROOT/APPHOST"});
				}

				if (frameworkVersion2_64 != nullptr) {
					// For x64 2.0
					dictionary->Add("AXD-ISAPI-2.0-64", gcnew array<String^> {"AXD-ISAPI-2.0-64", "*.axd", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("PageHandlerFactory-ISAPI-2.0-64", gcnew array<String^> {"PageHandlerFactory-ISAPI-2.0-64", "*.aspx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("SimpleHandlerFactory-ISAPI-2.0-64", gcnew array<String^> {"SimpleHandlerFactory-ISAPI-2.0-64", "*.ashx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("WebServiceHandlerFactory-ISAPI-2.0-64", gcnew array<String^> {"WebServiceHandlerFactory-ISAPI-2.0-64", "*.asmx", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-ISAPI-2.0-64", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-ISAPI-2.0-64", "*.rem", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-ISAPI-2.0-64", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-ISAPI-2.0-64", "*.soap", "GET,HEAD,POST,DEBUG", "", "IsapiModule", frameworkVersion2_64 + "\\aspnet_isapi.dll", "Unspecified", "Script", "False", "classicMode,runtimeVersionv2.0,bitness64", "0", "MACHINE/WEBROOT/APPHOST"});

					dictionary->Add("WebServiceHandlerFactory-Integrated", gcnew array<String^> {"WebServiceHandlerFactory-Integrated", "*.asmx", "GET,HEAD,POST,DEBUG", "System.Web.Services.Protocols.WebServiceHandlerFactory, System.Web.Services, Version=2.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.asmx", "Framework64", frameworkVersion2_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv2.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-rem-Integrated", gcnew array<String^> {"HttpRemotingHandlerFactory-rem-Integrated", "*.rem", "GET,HEAD,POST,DEBUG", "System.Runtime.Remoting.Channels.Http.HttpRemotingHandlerFactory, System.Runtime.Remoting, Version=2.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.rem", "Framework64", frameworkVersion2_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv2.0", "4194304", "MACHINE/WEBROOT/APPHOST"});
					dictionary->Add("HttpRemotingHandlerFactory-soap-Integrated", gcnew array<String^> {"HttpRemotingHandlerFactory-soap-Integrated", "*.soap", "GET,HEAD,POST,DEBUG", "System.Runtime.Remoting.Channels.Http.HttpRemotingHandlerFactory, System.Runtime.Remoting, Version=2.0.0.0, Culture=neutral, PublicKeyToken=" + this->findTokenKey("*.soap", "Framework64", frameworkVersion2_64) + "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode,runtimeVersionv2.0", "4194304", "MACHINE/WEBROOT/APPHOST"});

				}


				dictionary->Add("SecurityCertificate", gcnew array<String^> {"SecurityCertificate", "*.cer", "GET,HEAD,POST", "", "IsapiModule", "%windir%\\system32\\inetsrv\\asp.dll", "File", "Script", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("ISAPI-dll", gcnew array<String^> {"ISAPI-dll", "*.dll", "*", "", "IsapiModule", "", "File", "Execute", "True", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("TraceHandler-Integrated", gcnew array<String^> {"TraceHandler-Integrated", "trace.axd", "GET,HEAD,POST,DEBUG", "System.Web.Handlers.TraceHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("WebAdminHandler-Integrated", gcnew array<String^> {"WebAdminHandler-Integrated", "WebAdmin.axd", "GET,DEBUG", "System.Web.Handlers.WebAdminHandler", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("AssemblyResourceLoader-Integrated", gcnew array<String^> {"AssemblyResourceLoader-Integrated", "WebResource.axd", "GET,DEBUG", "", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("PageHandlerFactory-Integrated", gcnew array<String^> {"PageHandlerFactory-Integrated", "*.aspx", "GET,HEAD,POST,DEBUG", "System.Web.UI.PageHandlerFactory", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("SimpleHandlerFactory-Integrated", gcnew array<String^> {"SimpleHandlerFactory-Integrated", "*.ashx", "GET,HEAD,POST,DEBUG", "System.Web.UI.SimpleHandlerFactory", "ManagedPipelineHandler", "", "Unspecified", "Script", "False", "integratedMode", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("CGI-exe", gcnew array<String^> {"CGI-exe", "*.exe", "*", "", "CgiModule", "", "File", "Execute", "True", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("SSINC-stm", gcnew array<String^> {"SSINC-stm", "*.stm", "GET,POST", "", "ServerSideIncludeModule", "", "File", "Script", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("SSINC-shtm", gcnew array<String^> {"SSINC-shtm", "*.shtm", "GET,POST", "", "ServerSideIncludeModule", "", "File", "Script", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("SSINC-shtml", gcnew array<String^> {"SSINC-shtml", "*.shtml", "GET,POST", "", "ServerSideIncludeModule", "", "File", "Script", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("TRACEVerbHandler", gcnew array<String^> {"TRACEVerbHandler", "*", "TRACE", "", "ProtocolSupportModule", "", "Unspecified", "None", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});
				dictionary->Add("OPTIONSVerbHandler", gcnew array<String^> {"OPTIONSVerbHandler", "*", "OPTIONS", "", "ProtocolSupportModule", "", "Unspecified", "None", "False", "", "4194304", "MACHINE/WEBROOT/APPHOST"});

				// None

				for each (KeyValuePair<String^, array<String^>^> ^handler in dictionary) {
					createHandler(siteName, handler->Value[0], handler->Value[1], handler->Value[2],
						handler->Value[3], handler->Value[4], handler->Value[5], handler->Value[6],
						handler->Value[7], handler->Value[8], handler->Value[9], handler->Value[10]);
				}
				return true;
			}

			bool removeHandler(String ^siteName, String ^handlerName)
			{
				//this->checkFileStatus();
				bool status = false;

				try {
					ServerManager ^serverManager = gcnew ServerManager();
					Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

					ConfigurationSection ^handlersSection = config->GetSection("system.webServer/handlers", siteName);

					ConfigurationElementCollection ^handlersCollection = handlersSection->GetCollection();

					ConfigurationElement ^addElement = FindElement(handlersCollection, "add", "name", handlerName);

					if (addElement == nullptr) {
						status = false;
					}
					else {
						handlersCollection->Remove(addElement);
						serverManager->CommitChanges();
						status = true;
					}
					serverManager->CommitChanges();
					delete serverManager;
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Handlers::removeHandler. Error: " + e->Message, 1105);
				}
				return status;
			}

			String ^findTokenKey(String ^name, String ^bit, String ^version)
			{
				XmlDocument ^xml = gcnew XmlDocument();
				PublicTokenKey ^key = gcnew PublicTokenKey();

				key->framework = version + "\\Config\\web.config";
				xml->Load(key->framework);
				XmlNodeList ^nodes = xml->SelectNodes("/configuration/system.web/httpHandlers/add[@path='" + name + "']");
				for each (XmlNode ^node in nodes) {
					String ^token = node->Attributes["type"]->Value->Trim();
					Dictionary<String^, String^> ^results = gcnew Dictionary<String^, String^>();
					for each (String ^kvp in token->Split(',')) {
						if (kvp->Trim()->StartsWith("PublicKeyToken")) {
							results->Add(kvp->Split('=')[0], kvp->Split('=')[1]);
							// Console.WriteLine(kvp);

						}
					}
					key->key = results[" PublicKeyToken"];

				}
				return key->key->Trim();
			}

			String ^findNetVersion(String ^runtimeVersion)
			{
				String ^key;
				String ^subKey;
				if (runtimeVersion->Equals("v2")) {
					key = "SOFTWARE\\Microsoft\\ASP.NET";
					subKey = "Path";
				}
				else {
					key = "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client";
					subKey = "InstallPath";
				}
				String ^framework_path;
				try {
					//Console::WriteLine("key {0}", key);
					if (runtimeVersion->Equals("v2")) {
						Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
						RegistryKey ^myKey = localMachine->OpenSubKey(key, false);
						if (myKey != nullptr) {
							for each (String ^sub_key in myKey->GetSubKeyNames()) {
								//Console::WriteLine("key {0}, SubKey {1}", key, sub_key);
								RegistryKey ^sk = myKey->OpenSubKey(sub_key, false);
								framework_path = sk->GetValue(subKey)->ToString();
								sk->Close();
								break;
							}
							myKey->Close();
						}
						else {
							Console::WriteLine("Key not found");
						}
					}
					else {
						Microsoft::Win32::RegistryKey ^localMachine = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
						RegistryKey ^myKey = localMachine->OpenSubKey(key, false);
						framework_path = myKey->GetValue(subKey)->ToString();
						myKey->Close();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Handlers::findNetVersion. Error: " + e->Message, 1105);
				}

				return framework_path;
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
		};
	}
}