#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/MainCase.h"
#include "../env/Networking.h"
#include "../env/Services.h"

using namespace Microsoft::Web::Administration;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography::X509Certificates;
using namespace Environments;

namespace Tools
{
	public ref class sslmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Services ^services = gcnew Services();

		static String ^domain;
		static String ^ssl_name;
		static array<Byte> ^cert_hash;

		static String ^country;
		static String ^state;
		static String ^city;
		static String ^companyname;
		static String ^department;
		static String ^email;
		static String ^www;
		static String ^crypto;
		static String ^ipaddr;
		static String ^private_key;
		static String ^pkey;
		static String ^crt;
		static String ^ca_bundle;
		static String ^cabundle;
		static String ^common_name;
		static String ^httpd_conf = "var.server_root 	= \"c:/fpanel/httpd\"\n" +
			"var.log_root    	= server_root + \"/var/logs\"\n" +
			"var.state_dir      = server_root + \"/var/run\"\n" +
			"var.temp_dir    	= server_root + \"/tmp\"\n" +
			"var.cache_dir      = server_root + \"/var/cache\"\n" +
			"var.cert_dir    	= server_root + \"/cert\"\n" +
			"compress.cache-dir = server_root + \"/var/cache\"\n" +
			"compress.filetype  = (\"application/x-javascript\", \"application/javascript\", \"text/javascript\", \"text/x-js\", \"text/css\", \"text/html\", \"text/plain\")\n\n" +

			"server.modules              = (\n" +
			"                               \"mod_access\",\n" +
			"                               \"mod_cgi\",\n" +
			"                               \"mod_fastcgi\",\n" +
			"                               \"mod_rewrite\",\n" +
			"							    \"mod_compress\",\n" +
			"                               )\n" +
			"include \"c:/fpanel/httpd/conf/mimetype.conf\"\n\n" +

			"server.document-root        	= server_root + \"/htdocs\"\n" +
			"server.upload-dirs          	= ( temp_dir )\n" +
			"index-file.names            	= ( \"index.php\", \"index.html\")\n" +
			"server.event-handler        	= \"libev\"\n" +
			"static-file.exclude-extensions = ( \".php\" )\n" +
			"server.use-ipv6            	= \"disable\"\n" +
			"server.errorlog             	= log_root + \"/error.log\"\n\n" +

			"server.port                 = 8880\n" +
			"server.max-fds              = 2048\n" +
			"server.stat-cache-engine    = \"simple\"\n" +
			"server.max-connections      = 2048\n" +
			"server.follow-symlink       = \"enable\"\n" +
			"cgi.assign                 	= ( \".php\" => server_root + \"/php/php-cgi.exe\")\n" +
			"url.rewrite 				= (\n" +
			"	\"^/(templates|language)/(.*)\" => \"/$0/$1\",\n" +
			"	\"^/favicon.ico\" => \"/$0/$1\",\n" +
			"	\"^/(.*)\" => \"/index.php?_url=/$1\"\n" +
			")\n" +

			"$SERVER[\"socket\"] == \":8880\" {\n" +
			"	ssl.engine                  = \"disable\"\n" +
			"	server.document-root        = server_root + \"/htdocs\"\n" +
			"}\n";

		static array<String^>^listSsl()
		{
			List<String^> ^list = gcnew List<String^>();
			X509Store ^store = gcnew X509Store("Web Hosting", StoreLocation::LocalMachine);
			store->Open(OpenFlags::ReadOnly);

			for each(System::Security::Cryptography::X509Certificates::X509Certificate2 ^cert in store->Certificates) {
				Console::WriteLine(cert->FriendlyName);
				list->Add(cert->FriendlyName);
			}

			store->Close();
			return list->ToArray();
		}

		static void removeCert(String^ ssl_name)
		{
			X509Store ^store = gcnew X509Store("Web Hosting", StoreLocation::LocalMachine);

			store->Open(OpenFlags::ReadWrite);
			for each(System::Security::Cryptography::X509Certificates::X509Certificate2 ^cert in store->Certificates->Find(X509FindType::FindBySubjectName, ssl_name, false))
			{
				if (cert != nullptr) {
					if (cert->FriendlyName == ssl_name) {
						store->Remove(cert);
						break;
					}
				}
			}
			store->Close();
		}


		static void addCert(X509Certificate2^ certificate)
		{
			X509Store ^store = gcnew X509Store("Web Hosting", StoreLocation::LocalMachine);
			store->Open(OpenFlags::OpenExistingOnly | OpenFlags::ReadWrite);
			cert_hash = certificate->GetCertHash();
			certificate->FriendlyName = ssl_name;
			store->Add(certificate);
			store->Close();
		}

		static void createCsr();

		static void insertIisSsl();

		static void removeIisSsl();

		static void writeBundle();
		
		static void writeCrt();
		
		static void writePkey();

		static void enableSslSite(String ^ipaddr, String ^domain);

		static void insertSystemCaBundle();

		static void insertSystemPkey();

		static void installSystemSsl();

		static void deactivateSystemSsl();

		static array<Byte>^ ReadFile(String^ fileName);

		static ConfigurationElement ^FindElement(ConfigurationElementCollection ^collection, String ^elementTagName, ... array<String^> ^keyValues);
	};
}
