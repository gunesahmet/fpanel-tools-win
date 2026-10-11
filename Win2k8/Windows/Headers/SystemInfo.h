#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		public ref class StringTool sealed abstract
		{
			/// <summary>
			/// Get a substring of the first N characters.
			/// </summary>
		public:
			static String ^Truncate(String ^source, int length)
			{
				if (source->Length > length) {
					source = source->Substring(0, length);
				}
				return source;
			}

			/// <summary>
			/// Get a substring of the first N characters. [Slow]
			/// </summary>
			static String ^Truncate2(String ^source, int length)
			{
				return source->Substring(0, Math::Min(length, source->Length));
			}
		};

		private ref class Datas
		{
		public:
			String ^outPut;
			String ^memoryUsagePercent;
			String ^jsonData;
		};

		private ref class MemoryData
		{
		public:
			property String ^total;
			property String ^used;
			property String ^free;
		};

		private ref class CpuData
		{
		public:
			property double total;
			property double used;
			property double free;
		};

		private ref class SystemData
		{
		public:
			property String ^os;
			property String ^kernel;
		};

		private ref class NetworkData
		{
		public:
			property String ^hostname;
			property String ^ipaddr;
			property String ^traffic;
		};

		private ref class DiskData
		{
		public:
			property String ^total;
			property String ^used;
			property String ^free;
		};

		private ref class LoadAvgData
		{
		public:
			property String ^min1;
			property String ^min5;
			property String ^min15;
		};

		private ref class SystemInfo
		{
		public:
			property MemoryData ^memory;
			property CpuData ^cpu;
			property SystemData ^system;
			property NetworkData ^network;
			property DiskData ^disk;
			property LoadAvgData ^loadavg;
			property String ^uptime;
			property String ^cpu_info;

		private:
			Datas ^create_datas;
			CpuData ^cpu_init;
			MemoryData ^memory_init;
			SystemData ^system_init;
			NetworkData ^network_init;
			DiskData ^disk_init;
			LoadAvgData ^load_avg_init;

		public:
			void sysInfo()
			{

				this->exec("C:\\fpanel\\core\\sbin\\systeminfo.exe", nullptr);
				this->parseSystemOutput();
				this->createJson();
				Console::WriteLine(create_datas->jsonData);
			}

			void exec(String ^command, String ^parameters)
			{
				System::Diagnostics::Process ^pProcess = gcnew System::Diagnostics::Process();
				pProcess->StartInfo->CreateNoWindow = true;

				pProcess->StartInfo->RedirectStandardOutput = true;
				pProcess->StartInfo->RedirectStandardInput = true;
				pProcess->StartInfo->UseShellExecute = false;

				String ^strCommand = command;
				pProcess->StartInfo->FileName = strCommand;

				if (parameters != nullptr) {
					String ^strCommandParameters = parameters;
					pProcess->StartInfo->Arguments = strCommandParameters;
				}

				pProcess->EnableRaisingEvents = false;

				try {
					pProcess->Start();
				}
				catch (...) {
					throw;
				}

				StreamReader ^streamReader = pProcess->StandardOutput;
				try {
					String ^output = streamReader->ReadToEnd();

					create_datas->outPut = output;
				}
				finally {
					delete streamReader;
				}
				pProcess->WaitForExit();
			}

			void parseSystemOutput()
			{


				array<String^> ^split = create_datas->outPut->Split(gcnew array<Char> {','});
				// cpu
				double used = Convert::ToDouble(split[0]);
				double total = 100;
				double free = (total - used);


				cpu_init->used = used;
				cpu_init->total = total;
				cpu_init->free = free;

				// Console.WriteLine(cpuInit.used);
				// System.Environment.Exit(0);

				// memory
				create_datas->memoryUsagePercent = split[1];
				memory_init->total = split[2];
				memory_init->used = split[3];
				memory_init->free = split[4];

				// System
				system_init->os = split[10];
				system_init->kernel = split[11];

				// Uptime
				this->uptime = split[5];

				// Cpu Info
				this->cpu_info = split[9];

				// Network
				//networkInit.hostname = split[15];
				network_init->hostname = this->readHostname();
				network_init->ipaddr = split[16];
				network_init->traffic = "";

				//Disk
				disk_init->total = split[6];
				disk_init->used = split[8];
				disk_init->free = split[7];

				// LoadAvg
				disk_init->total = split[6];
				disk_init->used = split[8];
				disk_init->free = split[7];
			}

			void createJson()
			{
				String ^json = "{\"cpu\": {\"free\": \"" + cpu_init->free + "\",\"total\": \"" + cpu_init->total + "\",\"used\": \"" + cpu_init->used + "\"},\"cpu_info\": \"" + this->cpu_info + "\",\"disk\": {\"free\": \"" + disk_init->free + "\",\"total\": \"" + disk_init->total + "\",\"used\": \"" + disk_init->used + "\"},\"loadavg\": {\"min1\": \"\",\"min15\": \"\",\"min5\": \"\"},\"memory\": {\"free\": \"" + memory_init->free + "\",\"total\": \"" + memory_init->total + "\",\"used\": \"" + memory_init->used + "\"},\"network\": {\"hostname\": \"" + network_init->hostname + "\",\"ipaddr\": \"" + network_init->ipaddr + "\",\"traffic\": \"\"},\"system\": {\"kernel\": \"" + system_init->kernel + "\",\"os\": \"" + system_init->os + "\"},\"uptime\": \"" + this->uptime + "\"}";
				create_datas->jsonData = json;
			}

			String ^readHostname()
			{
				int counter = 0;
				String ^line;
				String ^hostname = nullptr;
				System::IO::StreamReader ^hostname_conf = gcnew System::IO::StreamReader("C:\\fpanel\\etc\\hostname.conf");
				String ^hostname_with_quote = nullptr;
				String ^hostname_string = nullptr;
				String ^domainname_with_quote = nullptr;
				String ^domainname_string = nullptr;

				while ((line = hostname_conf->ReadLine()) != nullptr) {
					if (line->Trim() != nullptr) {
						hostname = line->Trim()->Replace("\"", "");
					}
					counter++;
				}
				return hostname;
			}


		private:
			void InitializeInstanceFields()
			{
				create_datas = gcnew Datas();
				cpu_init = gcnew CpuData();
				memory_init = gcnew MemoryData();
				system_init = gcnew SystemData();
				network_init = gcnew NetworkData();
				disk_init = gcnew DiskData();
				load_avg_init = gcnew LoadAvgData();
			}

		public:
			SystemInfo()
			{
				InitializeInstanceFields();
			}
		};
	}
}