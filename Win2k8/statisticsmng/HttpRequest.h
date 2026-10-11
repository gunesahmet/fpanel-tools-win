#pragma once
using namespace System::Net::Http;

namespace Tools
{
	private ref class HttpRequest
	{
	private:
		static initonly HttpClient ^client = gcnew HttpClient();

	};
}
