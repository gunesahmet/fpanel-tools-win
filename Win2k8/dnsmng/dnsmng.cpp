#include "dnsmng.h"

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	void dnsmng::Main(array<String^> ^Args)
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
		new_domain = main_case["new-domain"];
		ipaddr = main_case["ipaddr"];
		old_ipaddr = main_case["old-ipaddr"];
		domain_alias_list = main_case["alias"];
		type = main_case["type"];
		host = main_case["host"];
		value = main_case["value"];
		new_host = main_case["new-host"];
		new_value = main_case["new-value"];
		opt = main_case["opt"];
		domain_list = main_case["domain-list"];
		port = main_case["port"];
		weight = main_case["weight"];
		old_type = main_case["old-type"];
		old_host = main_case["old-host"];
		old_value = main_case["old-value"];
		zone_data = main_case["zone-data"];

		if (process == "init") {
			init();
		}
		else if (process == "create") {
			create();
		}
		else if (process == "update") {
			update();
		}
		else if (process == "remove") {
			remove();
		}
		else if (process == "create-rec") {
			createRecord();
		}
		else if (process == "update-rec") {
			updateRecord();
		}
		else if (process == "remove-rec") {
			removeRecord();
		}
		else if (process == "change-ipaddr") {
			changeIpAddr();
		}
		else if (process == "restore") {
			restore();
		}
		else if (process == "restore-temp") {
			restoreTemplate();
		}
		else if (process == "create-rec-temp") {
			createRecordTemplate();
		}
		else if (process == "update-rec-temp") {
			updateRecordTemplate();
		}
		else if (process == "remove-rec-temp") {
			removeRecordTemplate();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	Int64 dnsmng::soaSerial()
	{
		return utils->unixTimeStamp();
	}

	void dnsmng::init()
	{
		if (String::IsNullOrEmpty(domain)) {
			logger->errorLog("Domain name is null", 1305);
			System::Environment::Exit(1305);
		}

		try {
			String ^decoded_content = utils->exec(env->fcrypt, "-d --in=\"" + zone_data + "\"");
			decoded_content = utils->Trim(decoded_content);
			if (!String::IsNullOrEmpty(decoded_content)) {
				utils->writeFile(env->dns_zone_dir + domain, decoded_content);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng: " + e->Message + " Command: " + executed_command, 1333);
		}

	}

	void dnsmng::create()
	{
		try {
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Arguments is null", 1306);
				System::Environment::Exit(1306);
			}

			if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + domain + "\"") == true) {
				logger->errorLog(domain + " already exists in dns zone conf", 1307);
				System::Environment::Exit(1307);
			}

			utils->appendFile(env->dns_zone_conf, "zone \"" + domain + "\" { type master; file \"" + env->dns_zone_dir->Replace("\\", "/") + domain + "\";};");
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1320);
		}

	}

	void dnsmng::update()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(ipaddr) || String::IsNullOrEmpty(new_domain)) {
				logger->errorLog("Arguments is null", 1306);
				System::Environment::Exit(1306);
			}

			if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + domain + "\"") == false) {
				logger->errorLog(domain + " not found in dns zone conf", 1308);
				System::Environment::Exit(1308);
			}

			if (validate->checkFile(env->dns_zone_dir + domain) == false) {
				logger->errorLog(domain + " dns file not found", 1309);
				System::Environment::Exit(1309);
			}

			if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + new_domain + "\"") == false) {
				logger->errorLog(new_domain + " already exists in dns zone conf", 1310);
				System::Environment::Exit(1310);
			}

			if (validate->checkFile(env->dns_zone_dir + new_domain) == true) {
				logger->errorLog(new_domain + " dns file already exists", 1311);
				System::Environment::Exit(1311);
			}

			utils->removeFile(env->dns_zone_dir + domain);
			utils->removeLineFromFile(env->dns_zone_conf, "^zone.*\"" + domain + "\"");
			utils->appendFile(env->dns_zone_conf, "zone \"" + new_domain + "\" { type master; file \"" + env->dns_zone_dir->Replace("\\", "/") + new_domain + "\";};");

			utils->exec(env->servicesmng, "--dns --proc=reload");
			logger->infoLog(domain + " zone updated", 1303);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1321);
		}
	}

	void dnsmng::remove()
	{
		try {
			if (String::IsNullOrEmpty(domain)) {
				logger->errorLog("Arguments is null", 1312);
				System::Environment::Exit(1312);
			}

			if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + domain + "\"") == false) {
				logger->errorLog(domain + " not found in dns zone conf", 1313);
				System::Environment::Exit(1313);
			}

			if (validate->checkFile(env->dns_zone_dir + domain) == false) {
				logger->errorLog(domain + " dns file not found", 1314);
				System::Environment::Exit(1314);
			}
			utils->removeLineFromFile(env->dns_zone_conf, "^zone.*\"" + domain + "\"");
			utils->removeFile(env->dns_zone_dir + domain);
			logger->infoLog(domain + " zone removed", 1304);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1322);
		}
	}

	void dnsmng::createRecord()
	{
		if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(host) || String::IsNullOrEmpty(type) || String::IsNullOrEmpty(value)) {
			logger->errorLog("Argument missing", 1316);
			System::Environment::Exit(1316);
		}
		if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + domain + "\"") == false) {
			logger->errorLog(domain + " not found in dns zone conf", 1317);
			System::Environment::Exit(1317);
		}

		if (validate->checkFile(env->dns_zone_dir + domain) == false) {
			logger->errorLog(domain + " dns file not found", 1318);
			System::Environment::Exit(1318);
		}

		if (validate->fileIsContains(env->dns_zone_conf, "^" + host + ".*IN.*" + type + ".*" + value) == true) {
			logger->errorLog("Record already exists in " + domain + " zone file", 1319);
			System::Environment::Exit(1319);
		}
		array<String^> ^domain_aliases = domainAliases(domain);

		if (type == "A") {
			utils->appendFile(env->dns_zone_dir + domain, host + "               IN " + type + "   " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "               IN " + type + "    " + value);
				}
			}
		}
		else if (type == "MX") {
			utils->appendFile(env->dns_zone_dir + domain, domain + ".               IN " + type + " " + opt + " " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, alias + ".               IN " + type + " " + opt + " " + value->Replace(domain, alias));
				}
			}
		}
		else if (type == "CNAME") {
			utils->appendFile(env->dns_zone_dir + domain, host + "               IN " + type + "   " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "               IN " + type + "    " + value);
				}
			}
		}
		else if (type == "PTR") {
			utils->appendFile(env->dns_zone_dir + domain, host + "               IN " + type + "   " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "               IN " + type + "    " + value);
				}
			}
		}
		else if (type == "NS") {
			utils->appendFile(env->dns_zone_dir + domain, host + "               IN " + type + "   " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "               IN " + type + "    " + value);
				}
			}
		}
		else if (type == "TXT") {
			utils->appendFile(env->dns_zone_dir + domain, host + "               IN " + type + "   " + "\"" + value + "\"");
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "               IN " + type + "    " + "\"" + value + "\"");
				}
			}
		}
		else if (type == "SRV") {
			utils->appendFile(env->dns_zone_dir + domain, host + "              86400 IN " + type + " " + opt + " " + weight + " " + port + " " + value);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->appendFile(env->dns_zone_dir + alias, host->Replace(domain, alias) + "              86400 IN " + type + " " + opt + " " + weight + " " + port + " " + value);
				}
			}
		}

		changeSoaSerial(domain);
		changeSoaRecords(domain);
		if (domain_aliases->Length > 0) {
			for each (auto alias in domain_aliases)
			{
				changeSoaRecords(alias);
				changeSoaSerial(alias);
			}
		}

		logger->infoLog("DNS record created for " + domain, 1305);
		utils->exec(env->servicesmng, "--dns --proc=reload");
	}

	void dnsmng::updateRecord()
	{

	}

	void dnsmng::removeRecord()
	{
		try {
			if (String::IsNullOrEmpty(domain) || String::IsNullOrEmpty(host) || String::IsNullOrEmpty(type) || String::IsNullOrEmpty(value)) {
				logger->errorLog("Argument missing", 1316);
				System::Environment::Exit(1316);
			}

			if (validate->fileIsContains(env->dns_zone_conf, "^zone.*\"" + domain + "\"") == false) {
				logger->errorLog(domain + " not found in dns zone conf", 1307);
				System::Environment::Exit(1307);
			}

			if (validate->checkFile(env->dns_zone_dir + domain) == false) {
				logger->errorLog(domain + " dns file not found", 1318);
				System::Environment::Exit(1318);
			}

			array<String^> ^domain_aliases = domainAliases(domain);
			String ^replace_str;
			if (type != "TXT") {
				replace_str = "^" + host + ".*IN.*" + type + ".*" + value;
			}
			else {
				replace_str = "^" + host + ".*IN.*" + type + ".*" + "\"" + value + "\"";
			}
			utils->removeLineFromFile(env->dns_zone_dir + domain, replace_str);
			if (domain_aliases->Length > 0) {
				for each (auto alias in domain_aliases)
				{
					utils->removeLineFromFile(env->dns_zone_dir + alias, replace_str->Replace(domain, alias));
				}
			}

			logger->infoLog("DNS Record removed from " + domain, 1307);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1324);
		}
	}

	void dnsmng::changeIpAddr()
	{

	}

	void dnsmng::restore()
	{

	}

	void dnsmng::restoreTemplate()
	{
		try {
			if (validate->checkFile(env->dns_zone_original_file) == true) {
				utils->copyFile(env->dns_zone_original_file, env->dns_zone_template_file);
			}
			else {
				logger->errorLog("Original template zone file not found", 1330);
				System::Environment::Exit(1330);
			}
			logger->infoLog("DNS Zone file restored", 1310);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1325);
		}


	}

	void dnsmng::createRecordTemplate()
	{
		try {
			if (String::IsNullOrEmpty(host) || String::IsNullOrEmpty(type) || String::IsNullOrEmpty(value)) {
				logger->errorLog("Argument missing", 1331);
				System::Environment::Exit(1331);
			}

			if (type == "A") {
				utils->appendFile(env->dns_zone_template_file, host + "               IN " + type + "   " + value);
			}
			else if (type == "MX") {
				utils->appendFile(env->dns_zone_template_file, "DOMAINNAME               IN " + type + " " + opt + "   " + value);
			}
			else if (type == "CNAME") {
				utils->appendFile(env->dns_zone_template_file, host + "               IN " + type + "   " + value);
			}
			else if (type == "PTR") {
				utils->appendFile(env->dns_zone_template_file, host + "               IN " + type + "   " + value);
			}
			else if (type == "NS") {
				utils->appendFile(env->dns_zone_template_file, host + "               IN " + type + "   " + value);
			}
			else if (type == "TXT") {
				utils->appendFile(env->dns_zone_template_file, host + "               IN " + type + "   " + "\"" + value + "\"");
			}

			logger->infoLog("Created record for DNS Template", 1311);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1326);
		}
	}

	void dnsmng::updateRecordTemplate()
	{
		try {
			if (String::IsNullOrEmpty(host) || String::IsNullOrEmpty(type) || String::IsNullOrEmpty(value)) {
				logger->errorLog("Argument missing", 1331);
				System::Environment::Exit(1331);
			}

			if ((old_host == nullptr) || (old_type == nullptr) || (old_value == nullptr)) {
				logger->errorLog("Argument missing", 1331);
				System::Environment::Exit(1331);
			}

			if (type != "TXT")
				utils->replaceLine(env->dns_zone_template_file, "^" + old_host + ".*IN.*" + old_type + ".*" + old_value, host + "               IN " + type + "   " + value);
			else
				utils->replaceLine(env->dns_zone_template_file, "^" + old_host + ".*IN.*" + old_type + ".*" + "\"" + old_value + "\"", host + "               IN " + type + "   " + "\"" + value + "\"");

		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1327);
		}


	}

	void dnsmng::removeRecordTemplate()
	{
		try {
			String ^replace_str;
			if (type != "TXT") {
				replace_str = "^" + host + ".*IN.*" + type + ".*" + value;
			}
			else {
				replace_str = "^" + host + ".*IN.*" + type + ".*" + "\"" + value + "\"";
			}

			utils->removeLineFromFile(env->dns_zone_template_file + domain, replace_str);
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1328);
		}

	}

	void dnsmng::createDnsZone(String ^domain, String ^ipaddr)
	{
		try {
			Int64 soaserial = soaSerial();

			if (validate->checkFile(env->dns_template_file) == true) {
				StreamReader ^sr = File::OpenText(env->dns_template_file);
				try {
					array<String^> ^lines = File::ReadAllLines(env->dns_template_file);
					StreamWriter ^sw = gcnew StreamWriter(env->dns_zone_dir + domain);
					try {

						for each (String ^line in lines)
						{
							if (line != nullptr) {
								line->Replace("DOMAINNAME", domain);
								line->Replace("IPADDR", ipaddr);
								line->Replace("SOASERIAL", soaserial.ToString());
								sw->WriteLine(line);
							}
						}
						sw->Close();
					}
					finally
					{
						delete sw;
					}
					sr->Close();
				}
				finally
				{
					delete sr;
				}
			}
			else {
				logger->errorLog("DNS Template file doesnt exists", 1304);
				System::Environment::Exit(1304);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1329);
		}


	}

	void dnsmng::changeSoaRecords(String ^domainname)
	{
		try {
			array<String^> ^ns_records = gcnew array<String^> { };

			Regex ^searchRegex = gcnew Regex("^" + domainname + ".*IN.*NS");

			if (validate->checkFile(env->vhosts_dir + domainname) == true) {
				StreamReader ^sr = File::OpenText(env->dns_template_file);
				try {
					array<String^> ^lines = File::ReadAllLines(env->dns_template_file);
					int i = 0;
					for each (auto line in lines)
					{
						if (searchRegex->IsMatch(line->Replace("ServerAlias ", "")) == false) {
							if (line != nullptr) {
								array<String^> ^line_splt = line->Split(' ');
								ns_records[i] = line_splt[3]->Trim();
								i++;
							}
						}
					}
					sr->Close();
				}
				finally
				{
					delete sr;
				}
			}

			String ^soa_records = nullptr;

			if (ns_records->Length > 0) {
				soa_records = ns_records[0] + " " + "admin@" + domainname;
			}

			utils->replaceLine(env->dns_zone_dir + domainname, "^^@.*IN.*SOA.*admin@" + domainname + ".*(", "@       IN      SOA     " + soa_records + " (");
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1331);
		}
	}

	void dnsmng::changeSoaSerial(String ^domainname)
	{
		try {
			Int64 soa_serial = soaSerial();
			utils->replaceLine(env->dns_zone_dir + domainname, "^.*;.*Serial", soa_serial + "      ; Serial");
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1332);
		}
	}

	array<String^> ^dnsmng::domainAliases(String ^domainname)
	{
		array<String^> ^aliases = gcnew array<String^> { };
		try {
			Regex ^searchRegex = gcnew Regex("^www.*");

			if (validate->checkFile(env->vhosts_dir + domainname) == true) {
				StreamReader ^sr = File::OpenText(env->dns_template_file);
				try {
					array<String^> ^lines = File::ReadAllLines(env->dns_template_file);
					int i = 0;
					for each (auto line in lines)
					{
						if (searchRegex->IsMatch(line->Replace("ServerAlias ", "")) == false) {
							if (line != nullptr) {
								aliases[i] = line->Replace("ServerAlias ", "");
								i++;
							}
						}

					}
					sr->Close();
				}
				finally
				{
					delete sr;
				}
			}
		}
		catch (Exception ^e) {
			logger->debugLog("dnsmng:" + e->Message, 1333);
		}


		return aliases;
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	dnsmng ^mainProcess = gcnew dnsmng();
	mainProcess->Main(Args);
	return 0;
}