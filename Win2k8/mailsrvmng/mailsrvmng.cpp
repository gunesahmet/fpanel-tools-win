#include "mailsrvmng.h"



namespace Tools
{
	void mailsrvmng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);

		//detectServer();
		mail_enable_status = false;
		hmail_status = true;
		if (mail_enable_status) {
			String ^file = "C:\\fpanel\\mail\\bin\\MailEnable.Administration.dll";
			{
				try {
					System::Reflection::Assembly::ReflectionOnlyLoadFrom(file);
					System::Reflection::Assembly::LoadFrom(file);
				}
				catch (Exception ^ee) {
					Console::WriteLine("MailEnable not installed on this server.");
					System::Environment::Exit(1);
				}
			}
			mailEnable->setEnv(env);
			mailEnable->setLogger(logger);
		}
		else if (hmail_status) {
			String ^file = "C:\\fpanel\\mail\\Bin\\Interop.hMailServer.dll";
			{
				try {
					System::Reflection::Assembly::ReflectionOnlyLoadFrom(file);
					System::Reflection::Assembly::LoadFrom(file);
				}
				catch (Exception ^ee) {
					Console::WriteLine("hMail Server not installed on this server.");
					System::Environment::Exit(1);
				}
	
			}
			hMail->setEnv(env);
			hMail->setLogger(logger);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");

		// MailEnable
		sub_port = main_case["sub-port"];
		sub_port_status = main_case["sub-port-status"];
		sub_port_auth_mode = main_case["sub-port-auth-mode"];
		sub_ssl_cert = main_case["sub-ssl-cert"];

		ssl_name = main_case["ssl-name"];
		certificate_path = main_case["cert-path"];
		private_key_path = main_case["priv-path"];


		smtp_in_connection_limit = main_case["conn-in"];
		smtp_out_connection_limit = main_case["conn-out"];
		max_msg_size_status = main_case["max-msg-size-status"];
		max_msg_size = main_case["databytes"];
		max_queue_lifetime = main_case["lifetime"];
		smtpd_recipient_limit = main_case["max-rcpt"];

		spf = main_case["spf"];
		spf_white_lists = main_case["spf-white-lists"];
		spf_best_guess = main_case["spf-best-guess"];
		spf_best_guesses = main_case["spf-best-guesses"];
		spf_white_lists_status = main_case["spf-white-lists-status"];
		spf_pass_local = main_case["spf-pass-local"];

		dns_rbl_status = main_case["dns-rbl-status"];
		dns_rbl_mode = main_case["dns-rbl-mode"];
		me_dns_black_list = main_case["me-dns-blacklist"];
		me_dns_black_list_status = main_case["me-dns-blacklist-status"];

		url_rbl_status = main_case["url-rbl-status"];
		url_rbl_mode = main_case["url-rbl-mode"];
		me_url_black_list = main_case["me-url-blacklist"];
		me_url_black_list_status = main_case["me-url-blacklist-status"];

		list = main_case["list"];

		// Spamassassin
		spam_service = main_case["spam-serv"];
		spam_status = main_case["serv"];
		razor = main_case["razor"];
		pyzor = main_case["pyzor"];
		bayes = main_case["bayes"];
		rbl_check = main_case["rbl-check"];
		report = main_case["report"];
		score = main_case["score"];

		// hMailServer
		max_smtp_threads = main_case["max-smtp-threads"];

		smtp_no_tries = main_case["smtp-no-tries"];
		smtp_tries_min = main_case["smtp-tries-min"];

		smtp_relayer = main_case["smtp-relayer"];
		smtp_relayer_port = main_case["smtp-relayer-port"];
		smtp_relayer_auth = main_case["smtp-relayer-auth"];
		smtp_relayer_user = main_case["smtp-relayer-user"];
		smtp_relayer_passwd = main_case["smtp-relayer-passwd"];
		smtp_relayer_security = main_case["smtp-relayer-security"];

		smtp_auth_plain = main_case["smtp-auth-plain"];
		allow_empty_sender = main_case["allow-empty-sender"];
		allow_invalid_end = main_case["allow-invalid-end"];
		dis_invalid_client = main_case["dis-invalid-client"];
		max_inv_command = main_case["max-inv-command"];

		delivery_bind_to_ip = main_case["delivery-bind-to-ip"];
		max_rec_batch = main_case["max-rec-batch"];
		add_del_head = main_case["add-del-head"];
		rule_loop_limit = main_case["rule-loop-limit"];
		max_mx_host = main_case["max-mx-host"];

		hostname = main_case["hostname"];

		max_pop_threads = main_case["max-pop-threads"];
		max_imap_threads = main_case["max-imap-threads"];
		
		
		spam_mark_threshold = main_case["spam-mark-threshold"];
		add_header_spam = main_case["add-header-spam"];
		add_reason_spam = main_case["add-reason-spam"];
		add_msg_subj = main_case["add-msg-subj"];
		add_msg_subj_txt = main_case["add-msg-subj-txt"];
		spam_del_threshold = main_case["spam-del-threshold"];
		spam_max_msg_size = main_case["spam-max-msg-size"];

		use_spf = main_case["use-spf"];
		use_spf_score = main_case["use-spf-score"];

		check_host_helo = main_case["check-host-helo"];
		check_host_helo_score = main_case["check-host-helo-score"];

		use_mx_checks = main_case["use-mx-checks"];
		use_mx_checks_score = main_case["use-mx-checks-score"];

		verify_dkim = main_case["verify-dkim"];
		verify_dkim_score = main_case["verify-dkim-score"];

		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		if (process == "init") {
			init();
		}
		else if (process == "get-blacklists") {
			getBlacklists();
		}
		else if (process == "add-wlist") {
			addWhiteList();
		}
		else if (process == "add-blist") {
			addBlackList();
		}
		else if (process == "del-wlist") {
			deleteWhiteList();
		}
		else if (process == "del-blist") {
			deleteBlackList();
		}
		else if (process == "spam") {
			spamassassinSettings();
		}
		else if (process == "spam-init") {
			spamassassinSettings();
		}
		else if (process == "retrieve") {
			retrieveSettings();
		}
		else {
			System::Environment::Exit(0);
		}
	}

	void mailsrvmng::detectServer()
	{
		mail_enable_status = validate->fileIsContains(env->sys_packages, "mail_enable");
		hmail_status = validate->fileIsContains(env->sys_packages, "hmail");
	}

	void mailsrvmng::init()
	{
		if (mail_enable_status) {
			mailEnable->init(spf, spf_best_guess, spf_best_guesses, spf_white_lists_status, spf_white_lists, spf_pass_local, max_msg_size_status,
				max_msg_size, sub_port_status, sub_port, sub_port_auth_mode, smtpd_recipient_limit, smtp_in_connection_limit, 
				smtp_out_connection_limit, max_queue_lifetime, dns_rbl_status, dns_rbl_mode, url_rbl_status, url_rbl_mode, me_dns_black_list, 
				me_url_black_list);
		}
		else if (hmail_status) {
			Console::WriteLine("max_smtp_threads: " + max_smtp_threads);
			Console::WriteLine("max_msg_size: " + max_msg_size);
			Console::WriteLine("smtp_no_tries: " + smtp_no_tries);
			Console::WriteLine("smtp_tries_min: " + smtp_tries_min);
			Console::WriteLine("smtp_relayer: " + smtp_relayer);
			Console::WriteLine("smtp_relayer_port: " + smtp_relayer_port);
			Console::WriteLine("smtp_relayer_auth: " + smtp_relayer_auth);
			Console::WriteLine("smtp_relayer_user: " + smtp_relayer_user);
			Console::WriteLine("smtp_relayer_passwd: " + smtp_relayer_passwd);
			Console::WriteLine("smtp_relayer_security: " + smtp_relayer_security);
			Console::WriteLine("smtp_auth_plain: " + smtp_auth_plain);
			Console::WriteLine("allow_empty_sender: " + allow_empty_sender);
			Console::WriteLine("allow_invalid_end: " + allow_invalid_end);
			Console::WriteLine("dis_invalid_client: " + dis_invalid_client);
			Console::WriteLine("max_inv_command: " + max_inv_command);
			Console::WriteLine("delivery_bind_to_ip: " + delivery_bind_to_ip);
			Console::WriteLine("max_rec_batch: " + max_rec_batch);
			Console::WriteLine("add_del_head: " + add_del_head);
			Console::WriteLine("rule_loop_limit: " + rule_loop_limit);
			Console::WriteLine("max_mx_host: " + max_mx_host);
			Console::WriteLine("hostname: " + hostname);
			Console::WriteLine("max_pop_threads: " + max_pop_threads);
			Console::WriteLine("max_imap_threads: " + max_imap_threads);
			Console::WriteLine("sub_port_status: " + sub_port_status);
			Console::WriteLine("sub_port: " + sub_port);
			Console::WriteLine("sub_port_auth_mode: " + sub_port_auth_mode);
			Console::WriteLine("sub_ssl_cert: " + sub_ssl_cert);
			Console::WriteLine("spam_mark_threshold: " + spam_mark_threshold);
			Console::WriteLine("add_header_spam: " + add_header_spam);
			Console::WriteLine("add_reason_spam: " + add_reason_spam);
			Console::WriteLine("add_msg_subj: " + add_msg_subj);
			Console::WriteLine("add_msg_subj_txt: " + add_msg_subj_txt);
			Console::WriteLine("spam_del_threshold: " + spam_del_threshold);
			Console::WriteLine("spam_max_msg_size: " + spam_max_msg_size);
			Console::WriteLine("use_spf: " + use_spf);
			Console::WriteLine("use_spf_score: " + use_spf_score);
			Console::WriteLine("check_host_helo: " + check_host_helo);
			Console::WriteLine("check_host_helo_score: " + check_host_helo_score);
			Console::WriteLine("use_mx_checks: " + use_mx_checks);
			Console::WriteLine("use_mx_checks_score: " + use_mx_checks_score);
			Console::WriteLine("verify_dkim: " + verify_dkim);
			Console::WriteLine("verify_dkim_score: " + verify_dkim_score);

			//System::Environment::Exit(0);
			hMail->initSmtp(max_smtp_threads, max_msg_size, smtp_no_tries, smtp_tries_min, smtp_relayer, smtp_relayer_port,
				smtp_relayer_auth, smtp_relayer_user, smtp_relayer_passwd, smtp_relayer_security, smtp_auth_plain,
				allow_empty_sender, allow_invalid_end, dis_invalid_client, max_inv_command, delivery_bind_to_ip,
				max_rec_batch, add_del_head, rule_loop_limit, max_mx_host, hostname, max_pop_threads, max_imap_threads, 
				sub_port_status, sub_port, sub_port_auth_mode, sub_ssl_cert);

			hMail->initAntiSpam(spam_mark_threshold, add_header_spam,
				add_reason_spam, add_msg_subj, add_msg_subj_txt, spam_del_threshold, spam_max_msg_size,
				use_spf, use_spf_score, check_host_helo, check_host_helo_score, use_mx_checks, use_mx_checks_score,
				verify_dkim, verify_dkim_score);
		}
	}

	void mailsrvmng::getBlacklists()
	{
		if (mail_enable_status) {
			mailEnable->getMailEnableBlackLists();
		}
		else if (hmail_status) {
			hMail->getHMailBlackLists();
		}
	}

	void mailsrvmng::spamassassinSettings()
	{
		if (mail_enable_status) {
			mailEnable->spamassassinSettings(spam_status);
		}
		else if (hmail_status) {
			hMail->spamassassinSettings(spam_status);
		}

		if ((razor != nullptr) && (razor == "on")) {
			String ^use_razor2 = "^use_razor2.*";
			utils->replaceLine(env->spam_local, use_razor2, "use_razor2 1");

			use_razor2 = "^#.*use_razor2.*";
			utils->replaceLine(env->spam_local, use_razor2, "use_razor2 1");
		}
		else {
			String ^use_razor2 = "^use_razor2.*";
			utils->replaceLine(env->spam_local, use_razor2, "# use_razor2 1");
		}

		if ((score != nullptr)) {
			String ^required_score = "^required_score.*";
			utils->replaceLine(env->spam_local, required_score, "required_score " + score);

			required_score = "^#.*required_score.*";
			utils->replaceLine(env->spam_local, required_score, "required_score " + score);
		}
		else {
			String ^required_score = "^required_score.*";
			utils->replaceLine(env->spam_local, required_score, "# required_score 5.0");
		}

		if ((report != nullptr) && (report == "on")) {
			String ^report_safe = "^report_safe.*";
			utils->replaceLine(env->spam_local, report_safe, "report_safe 1");

			report_safe = "^#.*report_safe.*";
			utils->replaceLine(env->spam_local, report_safe, "report_safe 1");
		}
		else {
			String ^report_safe = "^report_safe.*";
			utils->replaceLine(env->spam_local, report_safe, "# report_safe 1");
		}

		if ((bayes != nullptr) && (bayes == "on")) {
			String ^use_bayes = "^use_bayes.*";
			utils->replaceLine(env->spam_local, use_bayes, "use_bayes 1");
			String ^bayes_auto_learn = "^bayes_auto_learn.*";
			utils->replaceLine(env->spam_local, bayes_auto_learn, "bayes_auto_learn 1");

			use_bayes = "^#.*use_bayes.*";
			utils->replaceLine(env->spam_local, use_bayes, "use_bayes 1");
			bayes_auto_learn = "^#.*bayes_auto_learn.*";
			utils->replaceLine(env->spam_local, bayes_auto_learn, "bayes_auto_learn 1");

		}
		else {
			String ^use_bayes = "^use_bayes.*";
			utils->replaceLine(env->spam_local, use_bayes, "# use_bayes 1");
			String ^bayes_auto_learn = "^bayes_auto_learn.*";
			utils->replaceLine(env->spam_local, bayes_auto_learn, "# bayes_auto_learn 1");
		}

		if ((pyzor != nullptr) && (pyzor == "on")) {
			String ^use_pyzor = "^use_pyzor.*";
			utils->replaceLine(env->spam_local, use_pyzor, "use_pyzor 1");

			use_pyzor = "^#.*use_pyzor.*";
			utils->replaceLine(env->spam_local, use_pyzor, "use_pyzor 1");
		}
		else {
			String ^use_pyzor = "^use_pyzor.*";
			utils->replaceLine(env->spam_local, use_pyzor, "# use_pyzor 1");
		}

		if ((rbl_check != nullptr) && (rbl_check == "on")) {
			String ^skip_rbl_checks = "^skip_rbl_checks.*";
			utils->replaceLine(env->spam_local, skip_rbl_checks, "skip_rbl_checks 0");

			skip_rbl_checks = "^#.*skip_rbl_checks.*";
			utils->replaceLine(env->spam_local, skip_rbl_checks, "skip_rbl_checks 0");
		}
		else {
			String ^skip_rbl_checks = "^skip_rbl_checks.*";
			utils->replaceLine(env->spam_local, skip_rbl_checks, "# skip_rbl_checks 1");

			skip_rbl_checks = "^skip_rbl_checks.*";
			utils->replaceLine(env->spam_local, skip_rbl_checks, "skip_rbl_checks 1");
		}
	}

	void mailsrvmng::addWhiteList()
	{
		try {
			array<String^> ^wlist = list->Split('#');
			for (int i = 0; i < wlist->Length; i++) {
				utils->appendFile(env->spam_local, "whitelist_from " + wlist[i]);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("mailsrvmng: " + e->Message, 1804);
		}
	}

	void mailsrvmng::addBlackList()
	{
		try {
			array<String^> ^blist = list->Split('#');
			for (int i = 0; i < blist->Length; i++) {
				utils->appendFile(env->spam_local, "blacklist_from " + blist[i]);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("mailsrvmng: " + e->Message, 1804);
		}

	}

	void mailsrvmng::deleteWhiteList()
	{
		try {
			array<String^> ^wlist = list->Split('#');
			for (int i = 0; i < wlist->Length; i++) {
				utils->removeLineFromFile(env->spam_local, "^whitelist_from " + wlist[i]);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("mailsrvmng: " + e->Message, 1804);
		}
	}

	void mailsrvmng::deleteBlackList()
	{
		try {
			array<String^> ^blist = list->Split('#');
			for (int i = 0; i < blist->Length; i++) {
				utils->removeLineFromFile(env->spam_local, "blacklist_from " + blist[i]);
			}
		}
		catch (Exception ^e) {
			logger->debugLog("mailsrvmng: " + e->Message, 1804);
		}
	}

	void mailsrvmng::spamassassinInit()
	{
		// Spamassassin gerekli
		try {
			String ^required_score = "^#.*required_score.*5.0";
			String ^report_safe = "^#.*report_safe.*1";
			String ^use_bayes = "^#.*use_bayes.*1";
			String ^use_pyzor = "^#.*use_pyzor.*1";
			String ^use_razor2 = "^#.*use_razor2.*1";
			String ^skip_rbl_checks = "^#.*skip_rbl_checks.*";
			String ^bayes_auto_learn = "^#.*bayes_auto_learn.*1";
			utils->replaceLine(env->spam_local, required_score, "required_score 5.0");
			utils->replaceLine(env->spam_local, report_safe, "report_safe 1");
			utils->replaceLine(env->spam_local, use_bayes, "use_bayes 1");
			utils->replaceLine(env->spam_local, use_pyzor, "use_pyzor 1");
			utils->replaceLine(env->spam_local, use_razor2, "use_razor2 1");
			utils->replaceLine(env->spam_local, skip_rbl_checks, "skip_rbl_checks 0");
			utils->replaceLine(env->spam_local, bayes_auto_learn, "bayes_auto_learn 1");

			if (mail_enable_status) {
				mailEnable->spamassassinInit();
			}
			else if (hmail_status) {
				hMail->spamassassinInit();
			}
			
		}
		catch (Exception ^e) {
			logger->debugLog("mailsrvmng: " + e->Message, 1804);
		}
	}

	void mailsrvmng::retrieveSettings()
	{
		if (mail_enable_status) {
			mailEnable->retrieveSettings();
		}
		else if (hmail_status) {
			hMail->retrieveSettings();
		}
	}

	void mailsrvmng::saveSSL()
	{
		if (mail_enable_status) {
			mailEnable->saveSSL(ssl_name, certificate_path, private_key_path);
		}
		else if (hmail_status) {
			hMail->saveSSL(ssl_name, certificate_path, private_key_path);
		}
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	mailsrvmng ^mainProcess = gcnew mailsrvmng();
	mainProcess->Main(Args);
	return 0;
}