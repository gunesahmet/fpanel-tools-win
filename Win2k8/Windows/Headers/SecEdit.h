#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Text;

namespace FPanel
{
	namespace Windows
	{
		private ref class SecEdit
		{
		public:
			bool init()
			{
				String ^tempFile = Path::GetTempFileName();
				Process ^p = gcnew Process();
				p->StartInfo->FileName = Environment::ExpandEnvironmentVariables("%SystemRoot%\\system32\\secedit.exe");
				p->StartInfo->Arguments = String::Format("/export /cfg \"{0}\" /quiet", tempFile);
				p->StartInfo->CreateNoWindow = true;
				p->StartInfo->UseShellExecute = false;
				p->Start();
				p->WaitForExit();

				StringBuilder ^newCfg = gcnew StringBuilder();
				array<String^> ^cfg = File::ReadAllLines(tempFile);

				for each (String ^line in cfg)
				{
					if (line->Contains("PasswordComplexity"))
					{
						newCfg->AppendLine(line->Replace("1", "0"));
						continue;
					}
					newCfg->AppendLine(line);
				}
				File::WriteAllText(tempFile, newCfg->ToString());

				Process ^p2 = gcnew Process();
				p2->StartInfo->FileName = Environment::ExpandEnvironmentVariables("%SystemRoot%\\system32\\secedit.exe");
				p2->StartInfo->Arguments = String::Format("/configure /db secedit.sdb /cfg \"{0}\" /quiet", tempFile);
				p2->StartInfo->CreateNoWindow = true;
				p2->StartInfo->UseShellExecute = false;
				p2->Start();
				p2->WaitForExit();

				return true;
			}
		};
	}
}