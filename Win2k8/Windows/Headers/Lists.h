#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Runtime::Serialization;
using namespace System::IO;
using namespace Microsoft::Web::Administration;
using namespace System::Xml;
using namespace System::Text::RegularExpressions;


namespace FPanel
{
	namespace Windows
	{

		private ref class datas
		{
		public:
			String ^mainXML;
		};

		private ref class conn
		{
		public:
			property String ^connectionStringXML;
		};

		private ref class customErr
		{
		public:
			property String ^customErrorXML;
		};

		private ref class compDebug
		{
		public:
			property String ^compilationXML;
		};

		private ref class global
		{
		public:
			property String ^globalizationXML;
		};

		private ref class trustLevel
		{
		public:
			property String ^dotnetTrustLevelXML;
		};

		private ref class session
		{
		public:
			property String ^sessionSettingsXML;
		};

		private ref class mimetypes
		{
		public:
			property String ^mimeListXML;
		};

		private ref class modules
		{
		public:
			property String ^moduleListXML;
		};

		private ref class virtualDirectoryParams
		{
		public:
			property String ^virtualDirectorySettingXML;
		};

		public ref class JSon sealed abstract
		{
		public:
			static String ^XmlToJSON(String ^xml)
			{
				XmlDocument ^doc = gcnew XmlDocument();
				doc->LoadXml(xml);

				return XmlToJSON(doc);
			}
			static String ^XmlToJSON(XmlDocument ^xmlDoc)
			{
				StringBuilder ^sbJSON = gcnew StringBuilder();
				sbJSON->Append("{");
				XmlToJSONnode(sbJSON, xmlDoc->DocumentElement, true);
				sbJSON->Append("}");
				return sbJSON->ToString();
			}

			//  XmlToJSONnode:  Output an XmlElement, possibly as part of a higher array
		private:
			static void XmlToJSONnode(StringBuilder ^sbJSON, XmlElement ^node, bool showNodeName)
			{
				if (showNodeName)
					sbJSON->Append("\"" + SafeJSON(node->Name) + "\":");
				sbJSON->Append("{");
				// Build a sorted list of key-value pairs
				//  where   key is case-sensitive nodeName
				//          value is an ArrayList of string or XmlElement
				//  so that we know whether the nodeName is an array or not.
				System::Collections::Generic::SortedList<String^, Object^> ^childNodeNames = gcnew System::Collections::Generic::SortedList<String^, Object^>();

				//  Add in all node attributes
				if (node->Attributes != nullptr)
					for each (XmlAttribute ^attr in node->Attributes)
						//StoreChildNode(childNodeNames, attr->Name, attr->InnerText); // Original
						StoreChildNode(childNodeNames, attr->Name, attr->ChildNodes);

				//  Add in all nodes
				for each (XmlNode ^cnode in node->ChildNodes)
				{
					if (dynamic_cast<XmlText^>(cnode) != nullptr)
						StoreChildNode(childNodeNames, "value", cnode->InnerText);
					else if (dynamic_cast<XmlElement^>(cnode) != nullptr)
						StoreChildNode(childNodeNames, cnode->Name, cnode);
				}

				// Now output all stored info
				for each (String ^childname in childNodeNames->Keys)
				{
					List<Object^> ^alChild = safe_cast<List<Object^>^>(childNodeNames[childname]);
					if (alChild->Count == 1)
						OutputNode(childname, alChild[0], sbJSON, true);
					else {
						sbJSON->Append("\"" + SafeJSON(childname) + "\":[");
						for each (Object ^Child in alChild)
							OutputNode(childname, Child, sbJSON, false);
						sbJSON->Remove(sbJSON->Length - 2, 2);
						sbJSON->Append("],");
					}
				}
				sbJSON->Remove(sbJSON->Length - 2, 2);
				sbJSON->Append("}");
			}

			//  StoreChildNode: Store data associated with each nodeName
			//                  so that we know whether the nodeName is an array or not.
			static void StoreChildNode(System::Collections::Generic::SortedList<String^, Object^> ^childNodeNames, String ^nodeName, Object ^nodeValue)
			{
				// Pre-process contraction of XmlElement-s
				if (dynamic_cast<XmlElement^>(nodeValue) != nullptr) {
					// Convert  <aa></aa> into "aa":null
					//          <aa>xx</aa> into "aa":"xx"
					XmlNode ^cnode = safe_cast<XmlNode^>(nodeValue);
					if (cnode->Attributes->Count == 0) {
						XmlNodeList ^children = cnode->ChildNodes;
						if (children->Count == 0)
							nodeValue = nullptr;
						else if (children->Count == 1 && (dynamic_cast<XmlText^>(children[0]) != nullptr))
							nodeValue = (safe_cast<XmlText^>(children[0]))->InnerText;
					}
				}
				// Add nodeValue to ArrayList associated with each nodeName
				// If nodeName doesn't exist then add it
				List<Object^> ^ValuesAL;

				if (childNodeNames->ContainsKey(nodeName)) {
					ValuesAL = safe_cast<List<Object^>^>(childNodeNames[nodeName]);
				}
				else {
					ValuesAL = gcnew List<Object^>();
					childNodeNames[nodeName] = ValuesAL;
				}
				ValuesAL->Add(nodeValue);
			}

			static void OutputNode(String ^childname, Object ^alChild, StringBuilder ^sbJSON, bool showNodeName)
			{
				if (alChild == nullptr) {
					if (showNodeName)
						sbJSON->Append("\"" + SafeJSON(childname) + "\":");
					sbJSON->Append("null");
				}
				else if (dynamic_cast<String^>(alChild) != nullptr) {
					if (showNodeName)
						sbJSON->Append("\"" + SafeJSON(childname) + "\":");
					String ^sChild = safe_cast<String^>(alChild);
					sChild = sChild->Trim();
					sbJSON->Append("\"" + SafeJSON(sChild) + "\"");
				}
				else
					XmlToJSONnode(sbJSON, safe_cast<XmlElement^>(alChild), showNodeName);
				sbJSON->Append(", ");
			}

			// Make a string safe for JSON
			static String ^SafeJSON(String ^sIn)
			{
				StringBuilder ^sbOut = gcnew StringBuilder(sIn->Length);
				for each (Char ch in sIn)
				{
					if (Char::IsControl(ch) || ch == '\'') {
						int ich = safe_cast<int>(ch);
						sbOut->Append("\\u" + ich.ToString("x4"));
						continue;
					}
					else if (ch == '\"' || ch == '\\' || ch == '/') {
						sbOut->Append('\\');
					}
					sbOut->Append(ch);
				}
				return sbOut->ToString();
			}
		};

		private ref class Lists
		{
		public:
			String ^appcmd;

		private:
			datas ^createdDatas;
			conn ^connSettings;
			customErr ^customErrors;
			compDebug ^compilationDebug;
			global ^globaliz;
			trustLevel ^dotnetAccess;
			session ^sessionSet;
			virtualDirectoryParams ^virtualDirParams;
			mimetypes ^mimeTypes;
			modules ^modules_Renamed;
			ServerManager ^serverMgr;

		public:
			void settings(String ^sitename, String ^list, String ^vDirName)
			{
				if (list == "vdir") {
					this->virtualDirectorySettings(sitename, vDirName);
					String ^xml = virtualDirParams->virtualDirectorySettingXML;
					Console::WriteLine(xml);
				}
				else if (list == "aspnet") {
					this->connectionStringSettings(sitename);
					this->customErrorDocuments(sitename);
					this->compilationAndDebugging(sitename);
					this->globalization(sitename);
					this->dotNetTrustLevel(sitename);
					this->sessionSettings(sitename);
					this->mergeXML(connSettings->connectionStringXML, customErrors->customErrorXML, compilationDebug->compilationXML,
						globaliz->globalizationXML, dotnetAccess->dotnetTrustLevelXML, sessionSet->sessionSettingsXML);
					String ^mainXML = createdDatas->mainXML;
					Console::WriteLine(mainXML);
				}
				else if (list == "mimetype") {
					this->mimeTypeList(sitename);
					String ^mimeXML = mimeTypes->mimeListXML;
					String ^mimeJson = JSon::XmlToJSON(mimeXML);
					Console::WriteLine(Base64Encode(mimeXML));
				}
				else if (list == "modules") {
					this->moduleList();
					String ^modulesXML = modules_Renamed->moduleListXML;
					String ^modulesJson = JSon::XmlToJSON(modulesXML);
					modulesJson->Replace("\\", "\\\\");
					Console::WriteLine(modulesXML);
				}
				else {
				}
			}

			void connectionStringSettings(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^connStringsSection = config->GetSection("connectionStrings", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<connectionStrings>" + "</connectionStrings>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					for each (ConfigurationElement ^element in connStringsSection->GetCollection())
					{
						if (String::IsNullOrEmpty(element->GetAttribute("name")->Value->ToString()))
							break;
						XmlElement ^childNode = doc->CreateElement(element->GetAttribute("name")->Value->ToString());

						XmlNode ^parentNode = doc->SelectSingleNode("connectionStrings");
						parentNode->AppendChild(childNode);
						XmlNode ^createdParentNode = doc->SelectSingleNode("connectionStrings/" + element->GetAttribute("name")->Value->ToString());

						XmlElement ^elem = doc->CreateElement("name");
						elem->InnerText = element->GetAttribute("name")->Value->ToString();
						createdParentNode->AppendChild(elem);

						XmlElement ^elem2 = doc->CreateElement("connectionString");
						elem2->InnerText = element->GetAttribute("connectionString")->Value->ToString();
						createdParentNode->AppendChild(elem2);


						XmlElement ^elem3 = doc->CreateElement("providerName");
						elem3->InnerText = element->GetAttribute("providerName")->Value->ToString();
						createdParentNode->AppendChild(elem3);
					}
					connSettings->connectionStringXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Lists::connectionStringSettings. Error: " + e->Message, 1105);
				}
			}

			void customErrorDocuments(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^customErrorsSection = config->GetSection("system.web/customErrors", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<customErrors>" + "</customErrors>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlElement ^childNodeMode = doc->CreateElement("customErrorMode");
					root->AppendChild(childNodeMode);
					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("customErrors/customErrorMode");

					XmlElement ^elem3 = doc->CreateElement("mode");
					elem3->InnerText = customErrorsSection->GetAttributeValue("mode")->ToString();
					createdParentNodeLast->AppendChild(elem3);

					XmlElement ^childNodeMain = doc->CreateElement("customErrorList");
					root->AppendChild(childNodeMain);

					XmlNode ^createdParentNodeMain = doc->SelectSingleNode("customErrors/customErrorList");

					for each (ConfigurationElement ^element in customErrorsSection->GetCollection())
					{
						System::String ^errorCode = Regex::Replace(element->GetAttribute("statusCode")->Value->ToString(), "[^A-Za-z0-9 _]", "");
						XmlElement ^childNode = doc->CreateElement("error" + errorCode);
						createdParentNodeMain->AppendChild(childNode);

						XmlNode ^createdParentNode = doc->SelectSingleNode("customErrors/customErrorList/" + "error" + errorCode);

						XmlElement ^elem = doc->CreateElement("redirect");
						elem->InnerText = element->GetAttribute("redirect")->Value->ToString();
						createdParentNode->AppendChild(elem);

						XmlElement ^elem2 = doc->CreateElement("statusCode");
						elem2->InnerText = element->GetAttribute("statusCode")->Value->ToString();
						createdParentNode->AppendChild(elem2);
					}

					customErrors->customErrorXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Lists::customErrorDocuments. Error: " + e->Message, 1105);
				}
			}

			void compilationAndDebugging(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^section = config->GetSection("system.web/compilation", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<compilation>" + "</compilation>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlElement ^customErrorMode = doc->CreateElement("compilation");

					XmlNode ^customErrorNode = doc->SelectSingleNode("compilation");
					customErrorNode->AppendChild(customErrorMode);

					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("compilation/compilation");

					XmlElement ^elem = doc->CreateElement("debug");
					elem->InnerText = section->GetAttributeValue("debug")->ToString();
					createdParentNodeLast->AppendChild(elem);

					XmlElement ^elem1 = doc->CreateElement("defaultLanguage");
					elem1->InnerText = section->GetAttributeValue("defaultLanguage")->ToString();
					createdParentNodeLast->AppendChild(elem1);

					compilationDebug->compilationXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Lists::compilationAndDebugging. Error: " + e->Message, 1105);
				}
			}

			void globalization(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					//Get the configuration section for modules
					ConfigurationSection ^section = config->GetSection("system.web/globalization", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<globalization>" + "</globalization>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlElement ^customErrorMode = doc->CreateElement("globalization");

					XmlNode ^customErrorNode = doc->SelectSingleNode("globalization");
					customErrorNode->AppendChild(customErrorMode);

					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("globalization/globalization");

					XmlElement ^elem = doc->CreateElement("requestEncoding");
					elem->InnerText = section->GetAttributeValue("requestEncoding")->ToString();
					createdParentNodeLast->AppendChild(elem);

					XmlElement ^elem1 = doc->CreateElement("responseEncoding");
					elem1->InnerText = section->GetAttributeValue("responseEncoding")->ToString();
					createdParentNodeLast->AppendChild(elem1);

					XmlElement ^elem2 = doc->CreateElement("fileEncoding");
					elem2->InnerText = section->GetAttributeValue("fileEncoding")->ToString();
					createdParentNodeLast->AppendChild(elem2);

					XmlElement ^elem3 = doc->CreateElement("culture");
					elem3->InnerText = section->GetAttributeValue("culture")->ToString();
					createdParentNodeLast->AppendChild(elem3);

					XmlElement ^elem4 = doc->CreateElement("uiCulture");
					elem4->InnerText = section->GetAttributeValue("uiCulture")->ToString();
					createdParentNodeLast->AppendChild(elem4);

					globaliz->globalizationXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error Lists::globalization. Error: " + e->Message, 1105);
				}
			}

			void dotNetTrustLevel(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^trustSection = config->GetSection("system.web/trust", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<dotNetTrustLevel>" + "</dotNetTrustLevel>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlElement ^customErrorMode = doc->CreateElement("dotNetTrustLevel");

					XmlNode ^customErrorNode = doc->SelectSingleNode("dotNetTrustLevel");
					customErrorNode->AppendChild(customErrorMode);

					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("dotNetTrustLevel/dotNetTrustLevel");

					XmlElement ^elem = doc->CreateElement("level");
					elem->InnerText = trustSection->GetAttributeValue("level")->ToString();
					createdParentNodeLast->AppendChild(elem);

					dotnetAccess->dotnetTrustLevelXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name  + ":dotNetTrustLevel" + ". Error: " + e->Message, 1105);
				}
			}

			//void lockDotNetTrustLevel(String ^siteName)
			//{
			//	try {
			//		Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
			//		ConfigurationSection ^trustSection = config->GetSection("system.web/trust", siteName);
			//		trustSection->SetMetadata("lockItem", true);
			//		serverMgr->CommitChanges();
			//	}
			//	catch (Exception ^e) {
			//		Logger::errorLog("Error " + this->GetType()->Name + ":lockDotNetTrustLevel" + ". Error: " + e->Message, 1105);
			//	}
			//}

			void sessionSettings(String ^siteName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					//Get the configuration section for modules
					ConfigurationSection ^sectionAuth = config->GetSection("system.web/authentication", siteName);
					ConfigurationSection ^sectionState = config->GetSection("system.web/sessionState", siteName);

					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<sessionSettings>" + "</sessionSettings>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlElement ^customErrorMode = doc->CreateElement("sessionSettings");

					XmlNode ^customErrorNode = doc->SelectSingleNode("sessionSettings");
					customErrorNode->AppendChild(customErrorMode);

					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("sessionSettings/sessionSettings");

					XmlElement ^elem = doc->CreateElement("mode");
					elem->InnerText = sectionAuth->GetAttributeValue("mode")->ToString();
					createdParentNodeLast->AppendChild(elem);

					XmlElement ^customErrorMode1 = doc->CreateElement("sessionState");

					XmlNode ^customErrorNode1 = doc->SelectSingleNode("sessionSettings");
					customErrorNode1->AppendChild(customErrorMode1);

					XmlNode ^createdParentNodeLast1 = doc->SelectSingleNode("sessionSettings/sessionState");

					XmlElement ^elem1 = doc->CreateElement("timeout");
					elem1->InnerText = sectionState->GetAttributeValue("timeout")->ToString();
					createdParentNodeLast1->AppendChild(elem1);

					sessionSet->sessionSettingsXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":sessionSettings" + ". Error: " + e->Message, 1105);
				}
			}

			void virtualDirectorySettings(String ^siteName, String ^vDirName)
			{
				try {
					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<virtualDirectorySettings>" + "</virtualDirectorySettings>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;

					XmlNode ^createdParentNodeLast = doc->SelectSingleNode("virtualDirectorySettings");

					XmlElement ^directoryBrowse = doc->CreateElement("directoryBrowse");
					directoryBrowse->InnerText = this->getCollectionIIS(siteName, "system.webServer/directoryBrowse", "enabled", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(directoryBrowse);

					XmlElement ^handlers = doc->CreateElement("handlers");
					handlers->InnerText = this->getAccessPolicy(siteName, vDirName);
					createdParentNodeLast->AppendChild(handlers);

					XmlElement ^httpLogging = doc->CreateElement("httpLogging");
					httpLogging->InnerText = this->getCollectionIIS(siteName, "system.webServer/httpLogging", "dontLog", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(httpLogging);

					XmlElement ^enableParentPaths = doc->CreateElement("enableParentPaths");
					enableParentPaths->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "enableParentPaths", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(enableParentPaths);

					XmlElement ^executeInMta = doc->CreateElement("executeInMta");
					executeInMta->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "executeInMta", vDirName, "comPlus", true);
					createdParentNodeLast->AppendChild(executeInMta);

					XmlElement ^defaultDocumentStatus = doc->CreateElement("defaultDocumentStatus");
					defaultDocumentStatus->InnerText = this->getCollectionIIS(siteName, "system.webServer/defaultDocument", "enabled", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(defaultDocumentStatus);

					XmlElement ^anonymousAuthentication = doc->CreateElement("anonymousAuthentication");
					anonymousAuthentication->InnerText = this->getCollectionIIS(siteName, "system.webServer/security/authentication/anonymousAuthentication", "enabled", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(anonymousAuthentication);

					XmlElement ^sslFlags = doc->CreateElement("sslFlags");
					sslFlags->InnerText = this->getCollectionIIS(siteName, "system.webServer/security/access", "sslFlags", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(sslFlags);

					XmlElement ^bufferingLimit = doc->CreateElement("bufferingLimit");
					bufferingLimit->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "bufferingLimit", vDirName, "limits", true);
					createdParentNodeLast->AppendChild(bufferingLimit);

					XmlElement ^maxRequestEntityAllowed = doc->CreateElement("maxRequestEntityAllowed");
					maxRequestEntityAllowed->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "maxRequestEntityAllowed", vDirName, "limits", true);
					createdParentNodeLast->AppendChild(maxRequestEntityAllowed);

					XmlElement ^appAllowClientDebug = doc->CreateElement("appAllowClientDebug");
					appAllowClientDebug->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "appAllowClientDebug", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(appAllowClientDebug);

					XmlElement ^appAllowDebugging = doc->CreateElement("appAllowDebugging");
					appAllowDebugging->InnerText = this->getCollectionIIS(siteName, "system.webServer/asp", "appAllowDebugging", vDirName, nullptr, false);
					createdParentNodeLast->AppendChild(appAllowDebugging);

					XmlElement ^defaultDocuments = doc->CreateElement("defaultDocuments");
					defaultDocuments->InnerText = this->getDefaultDocuments(siteName, vDirName);

					createdParentNodeLast->AppendChild(defaultDocuments);
					virtualDirParams->virtualDirectorySettingXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":virtualDirectorySettings" + ". Error: " + e->Message, 1105);
				}
			}

			void mergeXML(String ^connectionStringXML, String ^customErrorXML, String ^compilationXML, 
				String ^globalizationXML, String ^dotnetTrustLevelXML, String ^sessionSettingsXML)
			{

				XmlDocument ^mainXML = gcnew XmlDocument();
				String ^docXml = "<ASP.NET>" + connectionStringXML + customErrorXML + compilationXML + globalizationXML + dotnetTrustLevelXML + sessionSettingsXML + "</ASP.NET>";
				mainXML->Load(gcnew StringReader(docXml));


				/*
				XElement connectionStringSettings = mainXML.Parse(connectionStringXML);

				XElement customErrorDocuments = XElement.Parse(customErrorXML);

				XElement compilationAndDebugging = XElement.Parse(compilationXML);

				XElement globalization = XElement.Parse(globalizationXML);

				XElement dotNetTrustLevel = XElement.Parse(dotnetTrustLevelXML);

				XElement sessionSettings = XElement.Parse(sessionSettingsXML);


				mainXML.Add(
					from el in connectionStringSettings.Elements()
					select el
				);

				mainXML.Add(
					from el in customErrorDocuments.Elements()
					select el
				);

				mainXML.Add(
					from el in compilationAndDebugging.Elements()
					select el
				);

				mainXML.Add(
					from el in globalization.Elements()
					select el
				);

				mainXML.Add(
					from el in dotNetTrustLevel.Elements()
					select el
				);

				mainXML.Add(
					from el in sessionSettings.Elements()
					select el
				);
				*/
				createdDatas->mainXML = mainXML->OuterXml->ToString();

			}

			String ^getAccessPolicy(String ^siteName, String ^vDirName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					if (vDirName != nullptr) {
						ConfigurationSection ^configurationSection = config->GetSection("system.webServer/handlers", siteName + "/" + vDirName);
						return configurationSection->GetAttribute("accessPolicy")->Value->GetHashCode().ToString();
					}
					else {
						ConfigurationSection ^configurationSection = config->GetSection("system.webServer/handlers", siteName);
						return configurationSection->GetAttribute("accessPolicy")->Value->GetHashCode().ToString();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":getAccessPolicy" + ". Error: " + e->Message, 1105);
				}
				return {};
			}

			String ^getCollectionIIS(String ^siteName, String ^section, String ^value, String ^vDirName, String ^childElement, bool withChild)
			{
				try {
					if (withChild == true) {
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
						if (vDirName != nullptr) {
							ConfigurationSection ^sectionVirtualDir = config->GetSection(section, siteName + "/" + vDirName);
							ConfigurationElement ^sectionElement = sectionVirtualDir->GetChildElement(childElement);
							return sectionElement->GetAttributeValue(value)->ToString();
						}
						else {
							ConfigurationSection ^sectionRoot = config->GetSection(section, siteName);
							ConfigurationElement ^sectionElement = sectionRoot->GetChildElement(childElement);
							return sectionElement->GetAttributeValue(value)->ToString();
						}
					}
					else {
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
						if (vDirName != nullptr) {
							ConfigurationSection ^sectionVirtualDir = config->GetSection(section, siteName + "/" + vDirName);
							return sectionVirtualDir->GetAttribute(value)->Value->ToString();
						}
						else {
							ConfigurationSection ^sectionRoot = config->GetSection(section, siteName);
							return sectionRoot->GetAttribute(value)->Value->ToString();
						}
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":getCollectionIIS" + ". Error: " + e->Message, 1105);
				}
				return {};
			}

			String ^getDefaultDocuments(String ^siteName, String ^vDirName)
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					if (vDirName == nullptr) {
						ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName);
						ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");

						String ^defaultDocs = nullptr;

						for each (ConfigurationElement ^element in filesCollection->GetCollection())
						{
							if (defaultDocs == nullptr) {
								defaultDocs = element->GetAttribute("value")->Value->ToString();
							}
							else {
								defaultDocs = defaultDocs + "," + element->GetAttribute("value")->Value->ToString();
							}
						}
						return defaultDocs;
					}
					else {
						ConfigurationSection ^defaultDocumentSection = config->GetSection("system.webServer/defaultDocument", siteName + "/" + vDirName);
						ConfigurationElementCollection ^filesCollection = defaultDocumentSection->GetCollection("files");
						String ^defaultDocs = nullptr;

						for each (ConfigurationElement ^element in filesCollection->GetCollection())
						{
							if (defaultDocs == nullptr) {
								defaultDocs = element->GetAttribute("value")->Value->ToString();
							}
							else {
								defaultDocs = defaultDocs + "," + element->GetAttribute("value")->Value->ToString();
							}
						}

						return defaultDocs;
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":getDefaultDocuments" + ". Error: " + e->Message, 1105);
				}
				return {};
			}

			void mimeTypeList(String ^siteName)
			{
				try {
					if (siteName != nullptr) {
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
						ConfigurationSection ^mimeTypeSection = config->GetSection("system.webServer/staticContent", siteName);
						XmlDocument ^doc = gcnew XmlDocument();
						String ^xmlString = "<mimetypes>" + "</mimetypes>";
						doc->Load(gcnew StringReader(xmlString));
						XmlElement ^root = doc->DocumentElement;
						for each (ConfigurationElement ^element in mimeTypeSection->GetCollection())
						{
							System::String ^nameMime = Regex::Replace(element->GetAttribute("fileExtension")->Value->ToString()->Replace(".", ""), "[^A-Za-z _]", "");
							System::String ^extMime = Regex::Replace(element->GetAttribute("mimeType")->Value->ToString(), "[^A-Za-z0-9 _]", "");

							XmlElement ^childNode = doc->CreateElement("mime" + nameMime);
							root->AppendChild(childNode);
							XmlNode ^createdParentNode = doc->SelectSingleNode("mimetypes/" + "mime" + nameMime);

							XmlElement ^elem = doc->CreateElement("fileExtension");
							elem->InnerText = element->GetAttribute("fileExtension")->Value->ToString();
							createdParentNode->AppendChild(elem);

							XmlElement ^elem2 = doc->CreateElement("mimeType");
							elem2->InnerText = element->GetAttribute("mimeType")->Value->ToString();
							createdParentNode->AppendChild(elem2);
						}
						mimeTypes->mimeListXML = doc->OuterXml->ToString();
					}
					else {
						Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
						ConfigurationSection ^mimeTypeSection = config->GetSection("system.webServer/staticContent");
						XmlDocument ^doc = gcnew XmlDocument();
						String ^xmlString = "<mimetypes>" + "</mimetypes>";
						doc->Load(gcnew StringReader(xmlString));
						XmlElement ^root = doc->DocumentElement;
						for each (ConfigurationElement ^element in mimeTypeSection->GetCollection())
						{
							System::String ^nameMime = Regex::Replace(element->GetAttribute("fileExtension")->Value->ToString()->Replace(".", ""), "[^A-Za-z0-9 _]", "");
							XmlElement ^childNode = doc->CreateElement("mime" + nameMime);
							root->AppendChild(childNode);

							XmlNode ^createdParentNode = doc->SelectSingleNode("mimetypes/" + "mime" + nameMime);
							XmlElement ^elem = doc->CreateElement("fileExtension");
							elem->InnerText = element->GetAttribute("fileExtension")->Value->ToString();
							createdParentNode->AppendChild(elem);
							XmlElement ^elem2 = doc->CreateElement("mimeType");
							elem2->InnerText = element->GetAttribute("mimeType")->Value->ToString();
							createdParentNode->AppendChild(elem2);
						}
						mimeTypes->mimeListXML = doc->OuterXml->ToString();
					}
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":mimeTypeList" + ". Error: " + e->Message, 1105);
				}
			}

			void moduleList()
			{
				try {
					Microsoft::Web::Administration::Configuration ^config = serverMgr->GetApplicationHostConfiguration();
					ConfigurationSection ^modulesSection = config->GetSection("system.webServer/globalModules");
					XmlDocument ^doc = gcnew XmlDocument();
					String ^xmlString = "<modules>" + "</modules>";
					doc->Load(gcnew StringReader(xmlString));
					XmlElement ^root = doc->DocumentElement;
					for each (ConfigurationElement ^element in modulesSection->GetCollection())
					{
						XmlElement ^childNode = doc->CreateElement(element->GetAttribute("name")->Value->ToString());
						//XmlElement childNode = doc.CreateElement("module");
						root->AppendChild(childNode);
						XmlNode ^createdParentNode = doc->SelectSingleNode("modules/" + element->GetAttribute("name")->Value->ToString());
						XmlElement ^name = doc->CreateElement("name");
						name->InnerText = element->GetAttribute("name")->Value->ToString();
						createdParentNode->AppendChild(name);
						XmlElement ^type = doc->CreateElement("image");
						type->InnerText = element->GetAttribute("image")->Value->ToString();
						XmlElement ^preCondition = doc->CreateElement("preCondition");
						preCondition->InnerText = element->GetAttribute("preCondition")->Value->ToString();
						createdParentNode->AppendChild(preCondition);
						createdParentNode->AppendChild(type);
					}
					modules_Renamed->moduleListXML = doc->OuterXml->ToString();
				}
				catch (Exception ^e) {
					Logger::errorLog("Error " + this->GetType()->Name + ":moduleList" + ". Error: " + e->Message, 1105);
				}
			}

			String ^Base64Encode(String ^plainText)
			{
				array<System::Byte> ^plainTextBytes = System::Text::Encoding::UTF8->GetBytes(plainText);
				return System::Convert::ToBase64String(plainTextBytes);
			}

		private:
			void InitializeInstanceFields()
			{
				Environment::CurrentDirectory = Environment::GetEnvironmentVariable("windir");
				DirectoryInfo^ info = gcnew DirectoryInfo(".");
				appcmd = info->FullName + "\\system32\\inetsrv\\appcmd.exe";

				createdDatas = gcnew datas();
				connSettings = gcnew conn();
				customErrors = gcnew customErr();
				compilationDebug = gcnew compDebug();
				globaliz = gcnew global();
				dotnetAccess = gcnew trustLevel();
				sessionSet = gcnew session();
				virtualDirParams = gcnew virtualDirectoryParams();
				mimeTypes = gcnew mimetypes();
				modules_Renamed = gcnew modules();
				serverMgr = gcnew ServerManager();
			}

			static String ^CsvStringAdd(String ^value, String ^csvList)
			{
				if (String::IsNullOrEmpty(csvList)) {
					return value;
				}

				if (CsvStringContains(value, csvList)) {
					return csvList;
				}

				return String::Concat(csvList, ",", value);
			}

			static bool CsvStringContains(String ^value, String ^csvList)
			{
				if (String::IsNullOrEmpty(csvList)) {
					return false;
				}

				array<String^> ^items = csvList->Split(',');

				for each (String ^item in items) {
					String ^current = item->Trim();

					if (String::Equals(current, value, StringComparison::OrdinalIgnoreCase)) {
						return true;
					}
				}

				return false;
			}

		public:
			Lists()
			{
				InitializeInstanceFields();
			}
		};

	}
}