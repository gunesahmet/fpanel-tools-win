using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Security::Principal;
using namespace System::Text;
using namespace System::Text::RegularExpressions;
using namespace Microsoft::Web::Administration;

namespace Environments
{
	public ref class Validations
	{
	public:
		bool validateDomain(String ^domain);

		bool validateIpAddress(String ^ipaddress);

		bool validateFileName(String ^file_name);

		bool checkUser(String ^username);

		bool checkDirectory(String ^directory);
		
		bool checkDirectoryStartWith(String ^directory, String ^path);

		bool checkFile(String ^file);

		bool fileIsContains(String ^file, String ^rgx);

		bool isDomainExistsIis(String ^domain);
	};
}