#pragma once
using namespace System;
using namespace System::IO;

namespace Environments
{
	public ref class Env
	{
	public:
		String ^system_path;
		String ^system32_path;
		String ^repo_server;
		String ^apps_server;
		String ^update_server;
		String ^backup_ext;
		String ^panel_dir;
		String ^panel_httpd_dir;
		String ^panel_sbin_dir;
		String ^panel_core_dir;
		String ^panel_log_dir;
		String ^panel_backup_dir;
		String ^panel_domain_configs_dir;
		String ^panel_mysql_dir;
		String ^php_dir;
		String ^vhosts_dir;
		String ^ftproot_dir;
		String ^dns_dir;
		String ^dns_zone_dir;
		String ^dns_zone_conf;
		String ^dns_template_file;
		String ^dns_zone_template_file;
		String ^dns_zone_original_file;
		String ^panel_config;
		String ^panel_release_conf;
		String ^sysconf;
		String ^sys_packages;
		String ^network_conf;
		String ^hostname_conf;
		String ^hosts_file;
		String ^my_cnf;
		String ^mysql_dir;
		String ^mssql_dir;
		String ^mail_dir;
		String ^apps_dir;
		String ^sqlite;
		String ^panel_cli;
		String ^webstatistics_tool;
		String ^panel_user;
		String ^panel_group;
		String ^panel_tool_dir;
		String ^appcmd;
		String ^default_ftp_site;
		String ^default_ftp_site_dir;
		String ^default_web_site;
		String ^default_web_site_dir;
		String ^spam_local;
		String ^adminmng;
		String ^appinstaller;
		String ^apppoolmng;
		String ^backupclientmng;
		String ^backupmng;
		String ^backupsrvmng;
		String ^crontabmng;
		String ^databasemng;
		String ^datemng;
		String ^directorymng;
		String ^dnsmng;
		String ^domaliasmng;
		String ^ftpmng;
		String ^logmng;
		String ^mailmng;
		String ^mailqueuemng;
		String ^mailsrvmng;
		String ^migrationmng;
		String ^networkmng;
		String ^packagemng;
		String ^pagemng;
		String ^phpmng;
		String ^protectmng;
		String ^servicesmng;
		String ^sslmng;
		String ^sqlmng;
		String ^subdirectorymng;
		String ^subusermng;
		String ^subvhostmng;
		String ^usermng;
		String ^vhostmng;
		String ^websrvmng;
		String ^statisticsmng;
		String ^webstatistics;
		String ^permissionmng;
		String ^wrapper;
		String ^systeminfo;
		String ^fcrypt;
		String ^statistics;
		String ^cli_passwd;
		String ^cli_username;
		String ^db_admin_passwd;
		String ^update_tmp_dir;
		String ^classic_default_apppool_v4 = "FpanelAppPoolv4Classic";
		String ^integrated_default_apppool_v4 = "FpanelAppPoolv4Integrated";
		String ^classic_default_apppool_v2 = "FpanelAppPoolv2Classic";
		String ^integrated_default_apppool_v2 = "FpanelAppPoolv2Integrated";
		String ^nomanaged_default_apppool = "FpanelAppPoolNoManaged";
		String ^pool_user = "FpanelAppPool";

		void init();

	private:
		static array<String^> ^getCliConfigs(String ^panel_conf);

	private:
		void InitializeInstanceFields();

public:
		Env()
		{
			InitializeInstanceFields();
		}
	};
}