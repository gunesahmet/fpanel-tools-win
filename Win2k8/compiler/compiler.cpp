#include "compiler.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections;

namespace Tools
{
	void compiler::Main(array<String^> ^Args)
	{
		Arguments ^main_case = gcnew Arguments(Args);
		String ^toolname = main_case["tool"];
		String ^path = main_case["path"];
		String ^target_path = main_case["target"];

		if (path == nullptr) {
			//path = "D:\\Projects\\Windows\\fpanel-tools-win\\Release\\";
			path = "H:\\Windows\\fpanel-tools-win\\Win2k8\\x64\\Release\\";
		}

		if (target_path == nullptr) {
			target_path = "\"H:\\Windows\\fpanel-tools-win\\Win2k8\\x64\\Release\\Enc\\<AssemblyFileName>\"";
		}

		if (toolname != nullptr) {
			compileTool(path, toolname, target_path);
		}
		else {
			compile(path, target_path);
		}
		System::Environment::Exit(0);
	}

	void compiler::compile(String ^path, String ^target_path)
	{
		DirectoryInfo ^d = gcnew DirectoryInfo(path);
		array<FileInfo^> ^Files = d->GetFiles("*.exe");
		String ^str = "";
		for each (FileInfo ^file in Files) {
			if ((!String::Equals(file->Name, "compiler.exe", StringComparison::OrdinalIgnoreCase))) {
				compileTool(path, file->Name, target_path);
				utils->removeFile(path);
			}
		}

	}

	void compiler::compileTool(String ^path, String ^tool, String ^target_path)
	{
		// "C:\Program Files (x86)\Eziriz\.NET Reactor\dotNET_Reactor.Console.exe" -file "H:\Windows\fpanel-tools-win\Win2k8\Release\adminmng.exe" 
		// For Ftp
		// 
		String ^params = " -mono 1 -visualstyles 0 -antitamp 1 -anti_debug 1 -hide_calls 1 -hide_calls_internals 1 -control_flow_obfuscation 1 -flow_level 9 -resourceencryption 1 -virtualization 1 -corebundle 1 -merge_namespaces 1 -mapping_file 1 -all_params 1 -naming unprintable -obfuscate_public_types 1";


		String ^output = utils->exec("\"H:\\General\\Portable\\dotNET_Reactor.Console.exe\"",
			" -file " +
			" \"" +
			path +
			tool +
			"\"" +
			params +
			" -targetfile " + target_path
		);
		utils->copyFile(path + "Enc/" + tool, "H:/Windows/fpanel-windows-packages/sources/panel-core/bin/" + tool);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	compiler ^mainProcess = gcnew compiler();
	mainProcess->Main(Args);
	return 0;
}
