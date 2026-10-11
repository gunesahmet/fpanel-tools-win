#pragma once
#include "Stdafx.h"
#include "Env.h"
using namespace System;
using namespace System::IO;

namespace Environments
{
	void Env::init()
	{
		this->panel_dir = this->system_path + "fpanel\\";
		this->panel_httpd_dir = this->panel_dir + "httpd\\";
		this->panel_core_dir = this->panel_dir + "core\\";
		this->panel_tool_dir = this->panel_dir + "core\\bin\\";
		this->panel_sbin_dir = this->panel_dir + "core\\sbin\\";
		this->panel_log_dir = this->panel_dir + "tmp\\";
		this->panel_backup_dir = this->panel_dir + "backup\\";
		this->panel_domain_configs_dir = this->panel_dir + "configs\\domains\\";
		this->panel_mysql_dir = this->panel_dir + "mysql\\";
		this->php_dir = this->panel_dir + "php\\";
		this->vhosts_dir = this->system_path + "vhosts\\";
		this->ftproot_dir = this->system_path + "inetpub\\ftproot\\";
		this->dns_dir = this->panel_dir + "bind\\";
		this->dns_zone_conf = this->dns_dir + "etc\\domains.conf";
		this->dns_zone_dir = this->dns_dir + "etc\\domains\\";
		this->dns_zone_template_file = this->dns_dir + "etc\\domains.template";
		this->dns_zone_original_file = this->panel_core_dir + "templates\bind\\domains.template";
		this->panel_config = this->panel_dir + "etc\\panel.conf";
		this->panel_release_conf = this->panel_dir + "etc\\release.conf";
		this->sysconf = this->panel_dir + "etc\\sys.conf";
		this->sys_packages = this->panel_dir + "etc\\.sys.packages";
		this->network_conf = this->panel_dir + "etc\\network.conf";
		this->hostname_conf = this->panel_dir + "etc\\hostname.conf";
		this->hosts_file = this->system_path + "Windows\\System32\\etc\\drivers\\hosts";
		this->mysql_dir = this->panel_dir + "mysql\\";
		this->mssql_dir = this->panel_dir + "mssql\\";
		this->my_cnf = this->mysql_dir + "my.ini";
		this->mail_dir = this->panel_dir + "mail\\";
		this->apps_dir = this->panel_dir + "apps\\";
		this->sqlite = this->panel_dir + "sqlite";
		this->panel_cli = this->panel_dir + "httpd\\htdocs\\cli.php";
		this->webstatistics_tool = this->panel_dir + "core\\sbin\\statistics";
		this->panel_user = "fpanel";
		this->panel_group = "fpanelgrp";
		this->appcmd = this->system32_path + "\\inetsrv\\appcmd.exe";
		this->default_ftp_site = "Default FTP Site";
		this->default_ftp_site_dir = system_path + "inetpub\\ftproot";
		this->default_web_site = "Default Web Site";
		//this->default_ftp_site_dir = this->vhosts_dir + "default";
		this->spam_local = this->panel_dir + "spamassassin\\etc\\spamassassin\\local.cf";

		this->adminmng = this->panel_tool_dir + "adminmng.exe";
		this->appinstaller = this->panel_tool_dir + "appinstaller.exe";
		this->apppoolmng = this->panel_tool_dir + "apppoolmng.exe";
		this->backupclientmng = this->panel_tool_dir + "backupclientmng.exe";
		this->backupmng = this->panel_tool_dir + "backupmng.exe";
		this->backupsrvmng = this->panel_tool_dir + "backupsrvmng.exe";
		this->crontabmng = this->panel_tool_dir + "crontabmng.exe";
		this->databasemng = this->panel_tool_dir + "databasemng.exe";
		this->datemng = this->panel_tool_dir + "datemng.exe";
		this->directorymng = this->panel_tool_dir + "directorymng.exe";
		this->dnsmng = this->panel_tool_dir + "dnsmng.exe";
		this->domaliasmng = this->panel_tool_dir + "domaliasmng.exe";
		this->ftpmng = this->panel_tool_dir + "ftpmng.exe";
		this->logmng = this->panel_tool_dir + "logmng.exe";
		this->mailmng = this->panel_tool_dir + "mailmng.exe";
		this->mailqueuemng = this->panel_tool_dir + "mailqueuemng.exe";
		this->mailsrvmng = this->panel_tool_dir + "mailsrvmng.exe";
		this->migrationmng = this->panel_tool_dir + "migrationmng.exe";
		this->networkmng = this->panel_tool_dir + "networkmng.exe";
		this->packagemng = this->panel_tool_dir + "packagemng.exe";
		this->pagemng = this->panel_tool_dir + "pagemng.exe";
		this->phpmng = this->panel_tool_dir + "phpmng.exe";
		this->protectmng = this->panel_tool_dir + "protectmng.exe";
		this->servicesmng = this->panel_tool_dir + "servicesmng.exe";
		this->sqlmng = this->panel_tool_dir + "sqlmng.exe";
		this->sslmng = this->panel_tool_dir + "sslmng.exe";
		this->subdirectorymng = this->panel_tool_dir + "subdirectorymng.exe";
		this->subusermng = this->panel_tool_dir + "subusermng.exe";
		this->subvhostmng = this->panel_tool_dir + "subvhostmng.exe";
		this->usermng = this->panel_tool_dir + "usermng.exe";
		this->vhostmng = this->panel_tool_dir + "vhostmng.exe";
		this->websrvmng = this->panel_tool_dir + "websrvmng.exe";
		this->permissionmng = this->panel_tool_dir + "permissionmng.exe";
		this->webstatistics = this->panel_tool_dir + "webstatistics.exe";
		this->webstatistics = this->panel_tool_dir + "statisticsmng.exe";
		this->wrapper = this->panel_tool_dir + "wrapper.exe";
		this->systeminfo = this->panel_sbin_dir + "systeminfo.exe";
		this->statistics = this->panel_sbin_dir + "statistics.exe";
		this->fcrypt = this->panel_sbin_dir + "fcrypt.exe";

		this->update_tmp_dir = this->panel_dir + "\\tmp\\update\\";

		array<String^> ^cliConfigs = Env::getCliConfigs(this->panel_config);
		if (cliConfigs->Length > 1) {
			this->cli_username = cliConfigs[0];
			this->cli_passwd = cliConfigs[1];
			this->db_admin_passwd = cliConfigs[2];
		}
	}

	array<String^> ^Env::getCliConfigs(String ^panel_conf)
	{
		if (!File::Exists(panel_conf)) {
			Console::WriteLine("Config file not found");
			System::Environment::Exit(1000);
		}
		array<String^> ^strArray = gcnew array<String^>(0);
		try {
			StreamReader ^streamReader = File::OpenText(panel_conf);
			try {
				for each (String ^readAllLine in File::ReadAllLines(panel_conf)) {
					if (readAllLine != nullptr)
						strArray = readAllLine->Split(' ');
				}
				streamReader->Close();
			}
			finally {
				delete streamReader;
			}
		}
		finally
		{
		}

		return strArray;
	}

	void Env::InitializeInstanceFields()
	{
		system_path = Path::GetPathRoot(Environment::SystemDirectory);
		system32_path = Environment::SystemDirectory;
		repo_server = "http://repo.fpanel.net";
		apps_server = "http://apps.fpanel.net";
		update_server = "http://update.fpanel.net";
		backup_ext = ".zip";
	}
}