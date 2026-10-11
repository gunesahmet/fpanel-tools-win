#pragma once
#include "MailEnable.h"
#include "HMail.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../env/Services.h"

using namespace Microsoft::Win32;
using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class mailsrvmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();
		static Environments::Services ^services = gcnew Environments::Services();
		static Mail::Enable ^mailEnable = gcnew Mail::Enable();
		static HMail::Server ^hMail = gcnew HMail::Server();

		static String ^list;
		static String ^sub_port;
		static String ^sub_port_status;
		static String ^sub_port_auth_mode;
		static String ^sub_ssl_cert;

		static String ^ssl_name;
		static String ^certificate_path;
		static String ^private_key_path;

		static String ^smtp_in_connection_limit;
		static String ^smtp_out_connection_limit;
		static String ^max_msg_size;
		static String ^max_msg_size_status;
		static String ^max_queue_lifetime;
		static String ^smtpd_recipient_limit;
		static String ^spf;
		static String ^spf_white_lists;
		static String ^spf_white_lists_status;
		static String ^spf_best_guess;
		static String ^spf_best_guesses;
		static String ^spf_local_policy_status;
		static String ^spf_pass_local;
		static String ^spam_service;

		static String ^dns_rbl_status;
		static String ^dns_rbl_mode;
		static String ^me_dns_black_list_status;
		static String ^me_dns_black_list;

		static String ^url_rbl_status;
		static String ^url_rbl_mode;
		static String ^me_url_black_list_status;
		static String ^me_url_black_list;


		static String ^spam_status;
		static String ^razor;
		static String ^pyzor;
		static String ^bayes;
		static String ^rbl_check;
		static String ^report;
		static String ^score;


		static String ^max_smtp_threads;
		static String ^smtp_no_tries;
		static String ^smtp_tries_min;
		static String ^smtp_relayer;
		static String ^smtp_relayer_port;
		static String ^smtp_relayer_auth;
		static String ^smtp_relayer_user;
		static String ^smtp_relayer_passwd;
		static String ^smtp_relayer_security;
		static String ^smtp_auth_plain;
		static String ^allow_empty_sender;
		static String ^allow_invalid_end;
		static String ^dis_invalid_client;
		static String ^max_inv_command;
		static String ^delivery_bind_to_ip;
		static String ^max_rec_batch;
		static String ^add_del_head;
		static String ^rule_loop_limit;
		static String ^max_mx_host;
		static String ^hostname;
		static String ^max_pop_threads;
		static String ^max_imap_threads;


		static String ^spam_mark_threshold;
		static String ^add_header_spam;
		static String ^add_reason_spam;
		static String ^add_msg_subj;
		static String ^add_msg_subj_txt;
		static String ^spam_del_threshold;
		static String ^spam_max_msg_size;
		static String ^use_spf;
		static String ^use_spf_score;
		static String ^check_host_helo;
		static String ^check_host_helo_score;
		static String ^use_mx_checks;
		static String ^use_mx_checks_score;
		static String ^verify_dkim;
		static String ^verify_dkim_score;

		static bool mail_enable_status;
		static bool hmail_status;

		static void detectServer();

		static void init();

		static void getBlacklists();

		static void spamassassinSettings();

		static void addWhiteList();

		static void addBlackList();

		static void deleteWhiteList();

		static void deleteBlackList();

		static void spamassassinInit();

		static void retrieveSettings();
		
		static void saveSSL();

	};
}
