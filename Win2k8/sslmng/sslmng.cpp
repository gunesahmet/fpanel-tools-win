#include "sslmng.h"

using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography::X509Certificates;

namespace Tools
{
	void sslmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);

		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");
		domain = main_case["domain"];
		ssl_name = main_case["ssl-name"];
		country = main_case["country"];
		state = main_case["state"];
		city = main_case["city"];
		companyname = main_case["companyname"];
		department = main_case["department"];
		email = main_case["email"];
		www = main_case["www"];
		crypto = main_case["crypto"];
		ipaddr = main_case["ipaddr"];
		private_key = main_case["private-key"];
		pkey = main_case["pkey"];
		crt = main_case["crt"];
		ca_bundle = main_case["bundle"];
		cabundle = main_case["ca-bundle"];
		common_name = main_case["common-name"];


		if (process == "create-csr-key") {
			createCsr();
		}
		else if (process == "install-ssl") {
			insertIisSsl();
		}
		else if (process == "remove-ssl") {
			removeIisSsl();
		}
		else if (process == "write-crt") {
			writeCrt();
		}
		else if (process == "write-pkey") {
			writePkey();
		}
		else if (process == "write-bundle") {
			writeBundle();
		}
		else if (process == "insert-system-ssl-ca") {
			insertSystemCaBundle();
		}
		else if (process == "insert-system-ssl-cert") {
			insertSystemPkey();
		}
		else if (process == "system-ssl") {
			installSystemSsl();
		}
		else if (process == "deactivate-system-ssl") {
			deactivateSystemSsl();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void sslmng::createCsr()
	{
		try {
			if ((domain == nullptr) || (country == nullptr) || (state == nullptr) || (city == nullptr) || (companyname == nullptr) || (department == nullptr) || (crypto == nullptr)) {
				logger->errorLog("Missing arguments", 1901);
				System::Environment::Exit(1901);
			}
			if (validate->isDomainExistsIis(domain) == false) {
				logger->errorLog(domain + " not found", 1902);
				System::Environment::Exit(1901);
			}
			String ^tmp_dir = env->panel_dir + "\\tmp";

			if (www == "off") {
				utils->exec("cmd", "/c " + env->panel_dir + "\\openssl\\bin\\openssl.exe req -nodes -newkey rsa:" + crypto +
					" -keyout " + tmp_dir + "\\" + ssl_name + ".key" +
					" -out " + tmp_dir + "\\" + ssl_name + ".csr" +
					" -subj \"/C=" + country + "/ST=" + state + "/L=" + city + "/O=" + companyname + "/OU=" + department + "/CN=" + domain);
			}
			else {
				utils->exec("cmd", "/c " + env->panel_dir + "\\openssl\\bin\\openssl.exe req -nodes -newkey rsa:" + crypto +
					" -keyout " + tmp_dir + "\\" + ssl_name + ".key" +
					" -out " + tmp_dir + "\\" + ssl_name + ".csr" +
					" -subj \"/C=" + country + "/ST=" + state + "/L=" + city + "/O=" + companyname + "/OU=" + department + "/CN=www." + domain);
			}
			String ^result = "";

			result += "BEGIN CERT KEY" + System::Environment::NewLine;

			StreamReader ^sr = File::OpenText(tmp_dir + "\\" + ssl_name + ".key");
			try {
				array<String^> ^lines = File::ReadAllLines(tmp_dir + "\\" + ssl_name + ".key");

				for each (String ^line in lines) {
					result += line + System::Environment::NewLine;
				}
				sr->Close();
			}
			finally {
				delete sr;
			}
			result += "END CERT KEY" + System::Environment::NewLine;
			result += "BEGIN CERT CSR" + System::Environment::NewLine;
			StreamReader ^srL = File::OpenText(tmp_dir + "\\" + ssl_name + ".csr");
			try {
				array<String^> ^lines = File::ReadAllLines(tmp_dir + "\\" + ssl_name + ".csr");
				for each (String ^line in lines) {
					result += line + System::Environment::NewLine;
				}
				srL->Close();
			}
			finally {
				delete srL;
			}
			result += "END CERT CSR";
			array<System::Byte> ^plainTextBytes = System::Text::Encoding::UTF8->GetBytes(result);
			Console::WriteLine(System::Convert::ToBase64String(plainTextBytes));
			utils->removeFile(tmp_dir + "\\" + ssl_name + ".csr");
			utils->removeFile(tmp_dir + "\\" + ssl_name + ".key");
			logger->infoLog("CSR created for " + domain, 1901);
		}
		catch (Exception ^e) {
			logger->debugLog("sslmng: " + e->Message, 1801);
		}
	}

	array<Byte>^ sslmng::ReadFile(String^ fileName)
	{
		FileStream^ f = gcnew FileStream(fileName, FileMode::Open, FileAccess::Read);
		int size = (int)f->Length;
		array<Byte>^data = gcnew array<Byte>(size);
		size = f->Read(data, 0, size);
		f->Close();
		return data;
	}

	void sslmng::insertIisSsl()
	{
		if ((String::IsNullOrEmpty(common_name)) || (String::IsNullOrEmpty(domain)) || (String::IsNullOrEmpty(ssl_name))){
			logger->errorLog("Missing arguments", 1904);
			System::Environment::Exit(1904);
		}
		//try {
			utils->exec(env->panel_dir + "openssl\\bin\\openssl.exe", "pkcs12 -export -out " +
				env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".pfx -inkey " +
				env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".key -in " +
				env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".crt -certfile " +
				env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".ca" + " -passout pass:");

			String ^cmdO = "echo & echo.|certutil -importpfx WebHosting " + env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".pfx";
			String ^output = utils->exec("cmd.exe", "/c " + cmdO);
			//Console::WriteLine(output);


			X509Store ^store = gcnew X509Store("WebHosting", StoreLocation::LocalMachine);
			store->Open(OpenFlags::ReadWrite|OpenFlags::MaxAllowed);

			array<unsigned char> ^cert_hash;
			String ^cert_hash_string;
			if (!String::IsNullOrWhiteSpace(ssl_name)) {
				X509Certificate2Collection ^certs = store->Certificates;
				for each(X509Certificate2 ^cert in certs) {
					if (cert->SubjectName->Name->Contains("CN=" + common_name)) {
						cert->FriendlyName = ssl_name;
						cert_hash = cert->GetCertHash();
						cert_hash_string = cert->GetCertHashString();
						break;
					}
				}
			}
			store->Close();
		//}
		//catch (Exception ^e) {
		//	logger->errorLog(e->Message, 1905);
		//	Console::WriteLine(e);
		//	System::Environment::Exit(1905);
		//}
		if (String::IsNullOrEmpty(cert_hash_string)) {
			logger->errorLog("Certificate can not be found", 1905);
			Console::WriteLine("Certificate can not be found");
			System::Environment::Exit(1905);
		}

		if (ipaddr == nullptr) {
			ipaddr = "*";
		}

		enableSslSite(ipaddr, domain);

		//try {
			ServerManager ^serverManager = gcnew ServerManager();

			Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

			ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

			ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

			ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain);
			if (siteElement == nullptr) {
				Console::WriteLine("domain not found");
				logger->errorLog("domain not found", 1905);
				System::Environment::Exit(1905);
			}

			ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");
			Site ^site = serverManager->Sites[domain];

	

			for each(Binding ^binding in site->Bindings) {
				if (binding->Protocol->Equals("https")) {
					binding->CertificateHash = cert_hash;
					binding->CertificateStoreName = "WebHosting";
					binding->SslFlags = SslFlags::None;
				}
			}
			serverManager->CommitChanges();
			delete serverManager;
			//site->Bindings->Add(ipaddr + ":443:" + domain, certificate->GetCertHash(), "MY", SslFlags::Sni);

		//}
		//catch (ServerManagerException ^e) {
		//	logger->errorLog(e->Message, 1906);
		//	Console::WriteLine(e);
		//	System::Environment::Exit(1906);
		//}
		//utils->removeFile(env->panel_dir + "\\configs\\" + domain + "\\ssl\\" + ssl_name + ".pfx");
	}

	void sslmng::removeIisSsl()
	{

		try {
			ServerManager ^serverManager = gcnew ServerManager();
			try {
				Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

				ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

				ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

				ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain);
				if (siteElement == nullptr)
					System::Environment::Exit(0);


				ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");

				ConfigurationElement ^bindingElement = FindElement(bindingsCollection, "binding", "protocol", "https", "bindingInformation", ipaddr + ":443:" + domain);
				if (bindingElement == nullptr)
					System::Environment::Exit(0);

				bindingsCollection->Remove(bindingElement);

				serverManager->CommitChanges();
			}
			finally
			{
				delete serverManager;
			}

			X509Certificate2 ^certificate = gcnew X509Certificate2(env->panel_dir + "\\configs\\" + domain + "\\ssl\\" + ssl_name + ".pfx");

			X509Store ^store = gcnew X509Store(StoreName::My, StoreLocation::LocalMachine);
			store->Open(OpenFlags::OpenExistingOnly | OpenFlags::ReadWrite);
			cert_hash = certificate->GetCertHash();
			store->Remove(certificate);
			store->Close();
		}
		catch (Exception ^e) {
			logger->debugLog("sslmng: " + e->Message, 1803);
		}
	}

	void sslmng::writeBundle()
	{
		Console::WriteLine("ca_bundle: " + ca_bundle);
		String ^decoded_ca_bundle = utils->Base64Decode(ca_bundle);
		Console::WriteLine("decoded_ca_bundle: " + decoded_ca_bundle);

		utils->writeFile(env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".ca", decoded_ca_bundle);
	}

	void sslmng::writeCrt()
	{
		Console::WriteLine("ca_bundle: " + ca_bundle);
		String ^decoded_crt = utils->Base64Decode(crt);
		Console::WriteLine("decoded_ca_bundle: " + decoded_crt);
		utils->writeFile(env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".crt", decoded_crt);
	}

	void sslmng::writePkey()
	{
		Console::WriteLine("ca_bundle: " + ca_bundle);
		String ^decoded_private_key = utils->Base64Decode(private_key);
		Console::WriteLine("decoded_private_key: " + decoded_private_key);
		utils->writeFile(env->panel_domain_configs_dir + domain + "\\ssl\\" + ssl_name + ".key", decoded_private_key);
	}

	void sslmng::enableSslSite(String ^ipaddr, String ^domain)
	{
		//try {
			ServerManager ^serverManager = gcnew ServerManager();

			Microsoft::Web::Administration::Configuration ^config = serverManager->GetApplicationHostConfiguration();

			ConfigurationSection ^sitesSection = config->GetSection("system.applicationHost/sites");

			ConfigurationElementCollection ^sitesCollection = sitesSection->GetCollection();

			ConfigurationElement ^siteElement = FindElement(sitesCollection, "site", "name", domain);
			if (siteElement == nullptr)
				throw gcnew InvalidOperationException("Element not found!");

			ConfigurationElementCollection ^bindingsCollection = siteElement->GetCollection("bindings");
			Site ^site = serverManager->Sites[domain];
			bool have = false;
			for each(Binding ^binding in site->Bindings)
			{
				if (binding->Protocol == "https") {
					have = true;
				}
			}
			if (!have) {
				ConfigurationElement ^bindingElement = bindingsCollection->CreateElement("binding");
				bindingElement["protocol"] = "https";
				bindingElement["bindingInformation"] = "*:443:" + domain;
				bindingElement["sslFlags"] = SslFlags::None;
				bindingsCollection->Add(bindingElement);
				serverManager->CommitChanges();
			}
			delete serverManager;
		//}
		//catch (Exception ^e) {
		//	Console::WriteLine(e);
		//	//System::Environment::Exit(0);
		//}
	}

	ConfigurationElement ^sslmng::FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues)
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

	void sslmng::insertSystemCaBundle()
	{
		if (cabundle == nullptr) {
			logger->errorLog("Missing arguments", 1901);
			System::Environment::Exit(1901);
		}
		if (!Directory::Exists(env->panel_httpd_dir + "/conf/ssl/")) {
			utils->createFolder(env->panel_httpd_dir + "/conf/ssl/");
		}

		String ^clear_ca = utils->exec(env->fcrypt, "-d --in=\"" + cabundle + "\"");
		utils->writeFile(env->panel_httpd_dir + "/conf/ssl/ca.pem", clear_ca);
	}

	void sslmng::insertSystemPkey()
	{
		if ((pkey == nullptr) || (crt == nullptr)) {
			logger->errorLog("Missing arguments", 1901);
			System::Environment::Exit(1901);
		}
		if (!Directory::Exists(env->panel_httpd_dir + "/conf/ssl/")) {
			utils->createFolder(env->panel_httpd_dir + "/conf/ssl/");
		}
		String ^clear_crt = utils->exec(env->fcrypt, "-d --in=\"" + crt + "\"");
		String ^clear_pkey = utils->exec(env->fcrypt, "-d --in=\"" + pkey + "\"");
		String ^combined_file = clear_pkey + "\n" + clear_crt;
		utils->writeFile(env->panel_httpd_dir + "/conf/ssl/server.pem", combined_file);
	}

	void sslmng::deactivateSystemSsl()
	{
		utils->writeFile(env->panel_httpd_dir + "/conf/httpd.conf", httpd_conf);
		services->stopService("FPanel Httpd Service", false);
		Thread::Sleep(1000);
		services->startService("FPanel Httpd Service", false);
		Thread::Sleep(2000);
	}

	void sslmng::installSystemSsl()
	{
		if ((!File::Exists(env->panel_httpd_dir + "/conf/ssl/server.pem")) || (!File::Exists(env->panel_httpd_dir + "/conf/ssl/ca.pem"))) {
			logger->errorLog("Missing certificate files", 1901);
			System::Environment::Exit(1901);
		}

		utils->writeFile(env->panel_httpd_dir + "/conf/httpd.conf", httpd_conf);
		try {
			String ^httpd_conf_ssl = "$SERVER[\"socket\"] == \":8443\" {\n" +
				"	ssl.engine = \"enable\"\n" +
				"	ssl.pemfile = server_root + \"/conf/ssl/server.pem\"\n" +
				"	ssl.ca-file = server_root + \"/conf/ssl/ca.pem\"\n" +
				"	server.document-root = server_root + \"/htdocs\"\n" +
				"}";
			utils->writeFile(env->panel_httpd_dir + "/conf/httpd.conf", httpd_conf + httpd_conf_ssl);

			services->stopService("FPanel Httpd Service", false);
			Thread::Sleep(1000);
			services->startService("FPanel Httpd Service", false);
			Thread::Sleep(2000);
		}
		catch (Exception ^e) {
			logger->errorLog("Missing arguments " + e->Message->ToString(), 1901);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	sslmng ^mainProcess = gcnew sslmng();
	mainProcess->Main(Args);
	return 0;
}