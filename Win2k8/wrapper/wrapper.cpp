#include "wrapper.h"

using namespace System;
using namespace System::IO;

namespace Tools
{
	void wrapper::Main(array<String^> ^Args) {
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

		user = main_case["user"];
		prefix = main_case["prefix"];
		passwd = main_case["passwd"];
		domain = main_case["domain"];
		subdomain = main_case["subdomain"];
		ipaddr = main_case["ipaddr"];
		old_ipaddr = main_case["old-ipaddr"];
		domain_list = main_case["domain-list"];
		sub_list = main_case["sub-list"];
		ssl = main_case["ssl"];
		ssl_name = main_case["ssl-name"];
		php = main_case["php"];
		safemode = main_case["safemode"];
		cgi = main_case["cgi"];
		perl = main_case["perl"];
		python = main_case["python"];
		stats = main_case["stats"];
		mail = main_case["mail"];
		disk_quota = main_case["disk-quota"];
		mail_quota = main_case["mail-quota"];
		bw_quota = main_case["bw-quota"];
		quota_over = main_case["quota-over"];
		new_quota = main_case["new-quota"];
		new_passwd = main_case["new-passwd"];
		new_user = main_case["new-user"];
		host = main_case["host"];
		type = main_case["type"];
		value = main_case["value"];
		old_host = main_case["old-host"];
		old_type = main_case["old-type"];
		old_value = main_case["old-value"];
		opt = main_case["opt"];
		mail_addr = main_case["mail-addr"];
		options = main_case["options"];
		forward = main_case["forward"];
		file = main_case["file"];
		no_inbox = main_case["no-inbox"];
		alias = main_case["alias"];
		new_alias = main_case["new-alias"];
		mail_serv = main_case["mail-serv"];
		web_serv = main_case["web-serv"];
		sync = main_case["sync"];
		pro_dir = main_case["pro-dir"];
		old_pro_dir = main_case["old-pro-dir"];
		pro_text = main_case["pro-text"];
		netmask = main_case["netmask"];
		eth = main_case["int"];
		shared = main_case["shared"];
		parentpaths = main_case["parentpaths"];
		limit = main_case["limit"];
		asp = main_case["asp"];
		aspnet = main_case["aspnet"];
		aspnet_ver = main_case["aspnet-ver"];
		appool = main_case["appool"];
		php_ver = main_case["php-ver"];
		sub_user = main_case["sub-user"];
		new_sub_user = main_case["new-sub-user"];
		force = main_case["force"];
		ftp_type = main_case["ftp-type"];
		pool_mode = main_case["pool-mode"];

		if (process == "create-vhost") {
			createVhost();
		}
		else if (process == "update-vhost") {
			updateVhost();
		}
		else if (process == "remove-vhost") {
			removeVhost();
		}
		else if (process == "suspend-vhost") {
			suspendVhost();
		}
		else if (process == "unsuspend-vhost") {
			unsuspendVhost();
		}
		else if (process == "create-vhost-sub") {
			createVhostSub();
		}
		else if (process == "update-vhost-sub") {
			updateVhostSub();
		}
		else if (process == "remove-vhost-sub") {
			removeVhostSub();
		}
		else if (process == "update-user") {
			updateUser();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void wrapper::createVhost() {
		utils->exec(env->usermng, "--create --user=" + user +
			" --passwd=" + passwd +
			" --prefix=" + prefix +
			" --domain=" + domain);
		Thread::Sleep(100);

		utils->exec(env->directorymng, "--create --user=" + user + " --domain=" + domain + " --prefix=" + prefix);
		Thread::Sleep(100);

		utils->exec(env->vhostmng, "--create --domain=" + domain +
			" --user=" + user +
			" --prefix=" + prefix +
			" --passwd=" + passwd +
			" --ipaddr=" + ipaddr +
			" --ssl=" + ssl +
			" --php=" + php +
			" --cgi=" + cgi +
			" --perl=" + perl +
			" --python=" + python +
			" --stats=" + stats +
			" --parentpaths=" + parentpaths +
			" --limit=" + limit +
			" --asp=" + asp +
			" --aspnet=" + aspnet +
			" --aspnet-ver=" + aspnet_ver +
			" --appool=" + appool +
			" --php-ver=" + php_ver +
			" --pool-mode=" + pool_mode);

		if (mail == "on") {
			if (mail_quota == "0") {
				utils->exec(env->mailmng, "--create --domain=" + domain + " --passwd=" + passwd);
			}
			else {
				utils->exec(env->mailmng, "--create --domain=" + domain + " --passwd=" + passwd + " --quota=" + mail_quota);
			}
		}
	}

	void wrapper::updateVhost() {
		utils->exec(env->vhostmng, "--update --domain=" + domain +
			" --user=" + user +
			" --prefix=" + prefix +
			" --passwd=" + passwd +
			" --ipaddr=" + ipaddr +
			" --ssl=" + ssl +
			" --php=" + php +
			" --cgi=" + cgi +
			" --perl=" + perl +
			" --python=" + python +
			" --stats=" + stats +
			" --parentpaths=" + parentpaths +
			" --limit=" + limit +
			" --asp=" + asp +
			" --aspnet=" + aspnet +
			" --aspnet-ver=" + aspnet_ver +
			" --appool=" + appool +
			" --php-ver=" + php_ver +
			" --pool-mode=" + pool_mode
		);

		if ((old_ipaddr != nullptr) && !String::IsNullOrEmpty(old_ipaddr))
			utils->exec(env->dnsmng, "--change-ipaddr --domain=" + domain +
				" --ipaddr=" + ipaddr +
				" --old-ipaddr=" + old_ipaddr +
				" --domain-list=" + domain
			);

		if ((sub_list != nullptr) && !String::IsNullOrEmpty(sub_list)) {
			array<String^> ^subdomain_lists = sub_list->Split('#');
			for each (auto index in subdomain_lists) {
				utils->exec(env->subvhostmng, "--change-ipaddr --subdomain=" + index +
					" --domain=" + domain +
					" --old-ipaddr=" + old_ipaddr +
					" --ipaddr=" + ipaddr);
			}
		}
	}

	void wrapper::removeVhost() {
		utils->exec(env->backupmng, "--remove-domain --domain=" + domain + " --prefix=" + prefix);
		utils->exec(env->crontabmng, "--remove-perm --user=" + user + " --prefix=" + prefix);
		utils->exec(env->dnsmng, "--remove --domain=" + domain);
		utils->exec(env->mailmng, "--remove --domain=" + domain);
		utils->exec(env->vhostmng, "--remove --domain=" + domain + " --prefix=" + prefix);
		utils->exec(env->apppoolmng, "--remove --domain=" + domain + " --prefix=" + prefix);
		utils->exec(env->directorymng, "--remove --domain=" + domain + " --user=" + user + " --prefix=" + prefix);
		utils->exec(env->usermng, "--remove --user=" + user + " --prefix=" + prefix);
		utils->exec(env->servicesmng, "--dns --proc=reload");
	}

	void wrapper::suspendVhost() {
		utils->exec(env->vhostmng, "--suspend --domain=" + domain);
		utils->exec(env->usermng, "--suspend --user=" + user);
		utils->exec(env->mailmng, "--suspend --domain=" + domain);
	}

	void wrapper::unsuspendVhost() {
		utils->exec(env->vhostmng, "--unsuspend --domain=" + domain);
		utils->exec(env->usermng, "--unsuspend --user=" + user);
		utils->exec(env->mailmng, "--unsuspend --domain=" + domain);
	}

	void wrapper::createVhostSub() {
		if (ftp_type == "native") {
			utils->exec(env->subdirectorymng, "--create --sub-user=" + sub_user + " --user=" + user + " --domain=" + domain + " --subdomain=" + subdomain + " --ftp-type=" + ftp_type + " --prefix=" + prefix);
		}
		else {
			utils->exec(env->subdirectorymng, "--create --user=" + user + " --domain=" + domain + " --subdomain=" + subdomain + " --ftp-type=" + ftp_type + " --prefix=" + prefix);
		}

		utils->exec(env->subvhostmng, "--create --domain=" + domain + " --ipaddr=" + ipaddr + " --php=" + php + " --cgi=" + cgi + " --perl=" + perl + " --python=" + python + " --asp=" + asp + " --aspnet=" + aspnet + " --aspnet-ver=" + aspnet_ver + " --php-ver=" + php_ver + " --subdomain=" + subdomain + " --prefix=" + prefix);
	}

	void wrapper::updateVhostSub() {
		if (new_sub_user != nullptr) {
			utils->exec(env->subusermng, "--update --sub-user=" + sub_user + " --new-sub-user=" + new_sub_user + " --passwd=" + passwd + " --domain=" + domain + " --subdomain=" + subdomain + " --prefix=" + prefix);
			utils->exec(env->subdirectorymng, "--update --sub-user=" + sub_user + " --new-sub-user=" + new_sub_user + " --user=" + user + " --domain=" + domain + " --subdomain=" + subdomain + " --ftp-type=" + ftp_type + " --prefix=" + prefix);
		}
		utils->exec(env->subvhostmng, "--update --domain=" + domain + " --subdomain=" + subdomain + " --ipaddr=" + ipaddr + " --php=" + php + " --cgi=" + cgi + " --perl=" + perl + " --python=" + python + " --asp=" + asp + " --aspnet=" + aspnet + " --aspnet-ver=" + aspnet_ver + " --php-ver=" + php_ver + " --user=" + user + " --prefix=" + prefix);
	}

	void wrapper::removeVhostSub() {
		utils->exec(env->subvhostmng, "--remove --domain=" + domain + " --subdomain=" + subdomain + " --prefix=" + prefix);

		if (ftp_type == "native") {
			utils->exec(env->subdirectorymng, "--remove --domain=" + domain + " --subdomain=" + subdomain + " --ftp-type=" + ftp_type + " --sub-user=" + sub_user + " --prefix=" + prefix);
			utils->exec(env->subusermng, "--remove --sub-user=" + sub_user + " --prefix=" + prefix);
			utils->exec(env->crontabmng, "--remove-perm --user=" + sub_user + " --prefix=" + prefix);
		}
		else {
			utils->exec(env->subdirectorymng, "--remove --domain=" + domain + " --subdomain=" + subdomain + " --sub-user=" + sub_user + " --prefix=" + prefix);
		}
	}

	void wrapper::updateUser() {
		if (new_passwd != nullptr) {
			utils->exec(env->usermng, "--update --user=" + user + " --new-passwd=" + new_passwd + " --prefix=" + prefix);
		}

		if (new_user != nullptr) {
			utils->exec(env->crontabmng, "--update --user=" + user + " --new-user=" + new_user + " --prefix=" + prefix);
			utils->exec(env->usermng, "--update --user=" + user + " --new-user=" + new_user + " --prefix=" + prefix);
			utils->exec(env->directorymng, "--update --domain=" + domain + " --new-user=" + new_user + " --prefix=" + prefix);
			utils->exec(env->vhostmng, "--update-user --domain=" + domain + " --user=" + user + " --new-user=" + new_user + " --prefix=" + prefix);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args) {
	wrapper ^mainProcess = gcnew wrapper();
	mainProcess->Main(Args);
	return 0;
}