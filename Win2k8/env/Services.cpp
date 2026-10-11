#include "Stdafx.h"
#include "Services.h"

namespace Environments
{

	void Services::startService(String ^serviceName, bool wait)
	{
		try
		{
			ServiceController ^sc = gcnew ServiceController();
			sc->ServiceName = serviceName;

			if (sc->Status == ServiceControllerStatus::Stopped)
			{
				sc->Start();
				if (wait == true)
					sc->WaitForStatus(ServiceControllerStatus::Running);
			}
		}
		catch (...)
		{
		}
	}

	void Services::stopService(String ^serviceName, bool wait)
	{
		try
		{
			ServiceController ^sc = gcnew ServiceController();
			sc->ServiceName = serviceName;

			if (sc->Status == ServiceControllerStatus::Running)
			{
				sc->Stop();
				if (wait == true)
					sc->WaitForStatus(ServiceControllerStatus::Running);
			}
		}
		catch (...)
		{

		}


	}

	bool Services::serviceStatus(String ^serviceName)
	{
		try
		{
			ServiceController ^sc = gcnew ServiceController();
			sc->ServiceName = serviceName;

			if (sc->Status == ServiceControllerStatus::Running)
				return true;
			else
				return false;
		}
		catch (...)
		{
			return false;
		}
	}
}