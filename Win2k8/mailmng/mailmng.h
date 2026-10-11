#include "../env/Env.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"
#include "../env/Services.h"

#include "MailEnable.h"
#include "HMail.h"
using namespace Environments;

namespace Tools
{
	private ref class mailmng
	{
	public:
		static void Main(array<String^> ^Args);

	private:
		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static Mail::Enable ^mailEnable = gcnew Mail::Enable();
		static HMail::Server ^hMail = gcnew HMail::Server();

		static String ^domain;
		static String ^mail_addr;
		static String ^passwd;
		static String ^quota;
		static String ^options;
		static String ^forward;
		static String ^filename;

		//static private string no_inbox;
		static String ^drop;
		static String ^new_passwd;
		static String ^mail_alias;
		static String ^mail_forward;
		static String ^mail_status;

		static bool mail_enable_status;

		static bool hmail_status;

		static void detectServer();
	
		static void createDomain();

		static void updateDomain();

		static void removeDomain();

		static void createMailBox();

		static void updateMailBox();

		static void removeMailBox();

		static void createMailAlias();

		static void removeMailAlias();

		static void suspendDomain();

		static void unsuspendDomain();

		static void suspendMail();

		static void unsuspendMail();

		static void createMailForward();

		static void removeMailForward();

		static void domainSettings();
	};
}