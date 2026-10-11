#pragma once
#include "WebServer.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace Microsoft::Web::Administration;
using namespace System::Xml;

namespace FPanel
{
	namespace Windows
	{
		private ref class AspNet
		{

		private:
			WebServer ^procs;
			ServerManager ^serverMgr;


		public:
			void compilation(String ^dotnetVersion, String ^defaultLanguage, String ^debugValue)
			{

				if (defaultLanguage != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/compilation /defaultLanguage:\"" + defaultLanguage + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}
				if (debugValue != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/compilation /debug:\"" + debugValue + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}

			}

			void globalization(String ^dotnetVersion, String ^culture, String ^fileEncoding, String ^requestEncoding, String ^responseEncoding, String ^uiCulture)
			{
				/*
				string frameworkConfigx64 = frameworkConfig2x64;
				string frameworkConfigx32 = frameworkConfig2x32;

				if (dotnetVersion == "2")
				{
					frameworkConfigx64 = frameworkConfig2x64;
					frameworkConfigx32 = frameworkConfig2x32;
				}
				else
				{
					frameworkConfigx64 = frameworkConfig4x64;
					frameworkConfigx32 = frameworkConfig4x32;
				}

				editXML(frameworkConfigx64, "configuration/system.web/compilation", "debug", debugValue);
				*/
				String ^cultureSet = "/culture:" + culture;
				String ^fileencodeSet = "/fileEncoding:" + fileEncoding;
				String ^requestEncodeSet = "/requestEncoding:" + requestEncoding;
				String ^responseEncodeSet = "/responseEncoding:" + responseEncoding;
				String ^settings = nullptr;

				if (culture != nullptr) {
					settings = settings + cultureSet;
				}
				if (fileEncoding != nullptr) {
					settings = settings + " " + fileencodeSet;
				}
				if (requestEncoding != nullptr) {
					settings = settings + " " + requestEncodeSet;
				}
				if (responseEncoding != nullptr) {
					settings = settings + " " + responseEncodeSet;
				}


				if (settings != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/globalization " + settings + " /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}

			}

			void customErrorSetting(String ^dotnetVersion, String ^customErrorMode)
			{
				if (customErrorMode != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/customErrors /mode:\"" + customErrorMode + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}
			}

			void dotNetTrustLevel(String ^dotnetVersion, String ^trustLevel)
			{
				if (trustLevel != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/trust /level:\"" + trustLevel + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}
			}

			void sessionSettings(String ^dotnetVersion, String ^sessionAuth, String ^sessionTimeout)
			{
				if (sessionAuth != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/authentication /mode:\"" + sessionAuth + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}
				if (sessionTimeout != nullptr) {
					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:system.web/sessionState /timeout:\"" + sessionTimeout + "\" /clr:\"" + dotnetVersion + "\" /commit:webroot");
				}
			}

			void addconnectionString(String ^dotnetVersion, String ^connectionString, String ^connectionStringName, String ^connectionStringProvider)
			{
				bool status = this->checkAppCmdOutput("connectionStrings ", dotnetVersion, "appcmd/CONFIG/connectionStrings/add", "name", connectionStringName);

				if (status == true) {
					System::Environment::Exit(7102);
				}


				if ((connectionString != nullptr) && (connectionStringName != nullptr)) {
					String ^connectionStringProp = "[connectionString='" + connectionString + "',name='" + connectionStringName + "',providerName='" + connectionStringProvider + "']";

					procs->exec("C:\\windows\\system32\\inetsrv\\appcmd.exe", "set config /section:connectionStrings /+\"" + connectionStringProp + "\" /clr:" + dotnetVersion + " /commit:webroot");
				}
			}

			void addCustomErrors(String ^dotnetVersion, String ^statusCode, String ^statusUrl)
			{

				bool status = this->checkAppCmdOutput("system.web\\customErrors", dotnetVersion, "appcmd/CONFIG/system.web-customErrors/error", "statusCode", statusCode);

				if (status == true) {
					System::Environment::Exit(7102);
				}


				if ((statusCode != nullptr) && (statusUrl != nullptr)) {
					String ^statusStrings = "\"[redirect='" + statusUrl + "',statusCode='" + statusCode + "']\"";
					procs->exec("c:\\windows\\system32\\inetsrv\\appcmd.exe", "/section:system.web/customErrors /+" + statusStrings + " /commit:webroot /clr:" + dotnetVersion);
				}


			}

			void removeConnectionString(String ^dotnetVersion, String ^connectionString, String ^connectionStringName, String ^connectionStringProvider)
			{
				bool status = this->checkAppCmdOutput("connectionStrings ", dotnetVersion, "appcmd/CONFIG/connectionStrings/add", "name", connectionStringName);

				if (status == false) {
					System::Environment::Exit(7102);
				}

				if (connectionStringName != nullptr) {
					String ^connectionStr = "\"[name='" + connectionStringName + "']\"";
					procs->exec("c:\\windows\\system32\\inetsrv\\appcmd.exe", "config  -section:connectionStrings /-" + connectionStr + " /commit:webroot  /clr:" + dotnetVersion);
				}

			}

			void removeCustomErrors(String ^dotnetVersion, String ^statusCode, String ^statusUrl)
			{
				bool status = this->checkAppCmdOutput("system.web\\customErrors", dotnetVersion, "appcmd/CONFIG/system.web-customErrors/error", "statusCode", statusCode);

				if (status == false) {
					System::Environment::Exit(7102);
				}

				if ((statusCode != nullptr) && (statusUrl != nullptr)) {
					String ^statusCodeString = "\"[statusCode='" + statusCode + "']\"";
					procs->exec("c:\\windows\\system32\\inetsrv\\appcmd.exe", "config  -section:system.web/customErrors /-" + statusCodeString + " /commit:webroot  /clr:" + dotnetVersion);
				}
			}

			bool checkAppCmdOutput(String ^iisSection, String ^dotnetVersion, String ^xmlNode, String ^xmlAttr, String ^xmlAttrValue)
			{
				bool result = false;
				String ^appcmdListConfig = this->exec("c:\\windows\\system32\\inetsrv\\appcmd.exe", "list config /xml -section:" + iisSection + " /clr:" + dotnetVersion);

				XmlDocument ^xmlDoc = gcnew XmlDocument();

				xmlDoc->Load(appcmdListConfig);

				XmlNode ^node = xmlDoc->SelectSingleNode(xmlNode);

				for each (XmlAttribute ^attrib in node)
				{
					if (attrib->Attributes[xmlAttr]->Value == xmlAttrValue) {
						result = true;
					}
				}

				return result;
			}

			static void editXML(String ^xmlFile, String ^xmlNode, String ^xmlAttribute, String ^attrValue)
			{
				XmlDocument ^xmlDoc = gcnew XmlDocument();

				xmlDoc->Load(xmlFile);

				XmlNode ^node = xmlDoc->SelectSingleNode(xmlNode);
				if (node->Attributes[xmlAttribute] != nullptr) {
					node->Attributes[xmlAttribute]->Value = attrValue;
				}
				else {
					XmlAttribute ^attrDebug = xmlDoc->CreateAttribute(xmlAttribute);
					attrDebug->Value = attrValue;
					node->Attributes->SetNamedItem(attrDebug);
				}

				xmlDoc->Save(xmlFile);
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

		public:
			String ^exec(String ^command, String ^parameters)
			{
				System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();

				//strCommand is path and file name of command to run
				pProcess->StartInfo->FileName = command;

				//strCommandParameters are parameters to pass to program
				pProcess->StartInfo->Arguments = parameters;

				pProcess->StartInfo->UseShellExecute = false;

				//Set output of program to be written to process output stream
				pProcess->StartInfo->RedirectStandardOutput = true;

				//Optional
				// pProcess.StartInfo.WorkingDirectory = strWorkingDirectory;

				//Start the process
				pProcess->Start();

				//Get program output
				String ^strOutput = pProcess->StandardOutput->ReadToEnd();

				//Wait for process to finish
				pProcess->WaitForExit();
				return strOutput;

			}


		private:
			void InitializeInstanceFields()
			{
				procs = gcnew WebServer();
				serverMgr = gcnew ServerManager();
			}

		public:
			AspNet()
			{
				InitializeInstanceFields();
			}
		};
	}
}