#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::ServiceProcess;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class Services
		{
		public:
			void startService(String ^service_name)
			{
				Console::WriteLine("Name: " + service_name);
				ServiceController ^myService = gcnew ServiceController();
				myService->ServiceName = service_name;
				String ^svcStatus = myService->Status.ToString();

				if (svcStatus == "Stopped")
				{
					myService->Start();
					String ^svcStatusWas = "";
					while (svcStatus != "Running")
					{
						if (svcStatus != svcStatusWas)
						{
							Console::WriteLine("Status: " + svcStatus);
						}
						svcStatusWas = svcStatus;
						myService->Refresh();
						svcStatus = myService->Status.ToString();
					}
				}
				Console::WriteLine("Service Started!!");

			}

			void stopService(String ^service_name)
			{
				ServiceController ^myService = gcnew ServiceController();
				myService->ServiceName = service_name;
				String ^svcStatus = myService->Status.ToString();

				if (svcStatus == "Running")
				{
					myService->Stop();
					String ^svcStatusWas = "";
					while (svcStatus != "Stopped")
					{
						svcStatusWas = svcStatus;
						myService->Refresh();
						svcStatus = myService->Status.ToString();
					}
				}
				Console::WriteLine("Service Stopped!!");

			}

			void restartService(String ^service_name)
			{
				this->stopService(service_name);
				this->startService(service_name);

			}
		};
	}
}