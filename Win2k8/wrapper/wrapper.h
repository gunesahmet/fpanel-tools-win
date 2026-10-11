//.h file code:

#include "../env/Utils.h"
#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace Environments;

namespace Tools
{
	private ref class wrapper
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();

		static String ^user;
		static String ^prefix;
		static String ^passwd;
		static String ^domain;
		static String ^subdomain;
		static String ^ipaddr;
		static String ^old_ipaddr;
		static String ^domain_list;
		static String ^sub_list;
		static String ^ssl;
		static String ^ssl_name;
		static String ^php;
		static String ^safemode;
		static String ^cgi;
		static String ^perl;
		static String ^python;
		static String ^stats;
		static String ^mail;
		static String ^disk_quota;
		static String ^mail_quota;
		static String ^bw_quota;
		static String ^quota_over;
		static String ^new_quota;
		static String ^new_passwd;
		static String ^new_user;
		static String ^host;
		static String ^type;
		static String ^value;
		static String ^old_host;
		static String ^old_type;
		static String ^old_value;
		static String ^opt;
		static String ^mail_addr;
		static String ^options;
		static String ^forward;
		static String ^file;
		static String ^no_inbox;
		static String ^alias;
		static String ^new_alias;
		static String ^mail_serv;
		static String ^web_serv;
		static String ^sync;
		static String ^pro_dir;
		static String ^old_pro_dir;
		static String ^pro_text;
		static String ^netmask;
		static String ^eth;
		static String ^shared;
		static String ^parentpaths;
		static String ^limit;
		static String ^asp;
		static String ^aspnet;
		static String ^aspnet_ver;
		static String ^appool;
		static String ^php_ver;
		static String ^sub_user;
		static String ^new_sub_user;
		static String ^force;
		static String ^ftp_type;
		static String ^pool_mode;

		static void createVhost();
		static void updateVhost();
		static void removeVhost();
		static void suspendVhost();
		static void unsuspendVhost();

		static void createVhostSub();
		static void updateVhostSub();
		static void removeVhostSub();

		static void updateUser();
		
	};
}
