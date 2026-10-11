using namespace System;
using namespace System::Collections::Generic;
using namespace System::ServiceProcess;
using namespace System::Text;
using namespace System::Threading;

namespace Environments
{
	public ref class Services
	{
	public:
		void startService(String ^serviceName, bool wait);

		void stopService(String ^serviceName, bool wait);

		bool serviceStatus(String ^serviceName);
	};
}