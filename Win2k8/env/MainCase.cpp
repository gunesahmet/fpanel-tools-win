#include "Stdafx.h"
#include "MainCase.h"
using namespace System;
using namespace System::Collections::Specialized;
using namespace System::Text::RegularExpressions;

namespace Environments
{
	Arguments::Arguments(array<String^> ^Args)
	{
		Parameters = gcnew StringDictionary();
		Regex ^Spliter = gcnew Regex("^-{1,2}|^/|=|:", RegexOptions::IgnoreCase | RegexOptions::Compiled);
		Regex ^Remover = gcnew Regex("^['""]?(.*?)['""]?$", RegexOptions::IgnoreCase | RegexOptions::Compiled);
		String ^Parameter = nullptr;
		array<String^> ^Parts;
		for each (String ^Txt in Args)
		{
			Parts = Spliter->Split(Txt, 3);
			switch (Parts->Length) {
			case 1:
				if (Parameter != nullptr) {
					if (!Parameters->ContainsKey(Parameter)) {
						Parts[0] = Remover->Replace(Parts[0], "$1");
						Parameters->Add(Parameter, Parts[0]);
					}
					Parameter = nullptr;
				}
				break;
			case 2:
				if (Parameter != nullptr) {
					if (!Parameters->ContainsKey(Parameter)) {
						/*		Console::Write("Parameter: ");
								Console::WriteLine(Parameter);*/
						Parameters->Add(Parameter, "true");
					}
				}
				Parameter = Parts[1];
				break;
			case 3:
				if (Parameter != nullptr) {
					if (!Parameters->ContainsKey(Parameter)) {
						/*			Console::Write("Parameter: ");
									Console::WriteLine(Parameter);*/
						Parameters->Add(Parameter, "true");
					}
				}

				Parameter = Parts[1];
				if (!Parameters->ContainsKey(Parameter)) {
					/*	Console::Write("Parts[2]: ");
						Console::WriteLine(Parts[2]);*/
					Parts[2] = Remover->Replace(Parts[2], "$1");
					Parameters->Add(Parameter, Parts[2]);
				}

				Parameter = nullptr;
				break;
			}
		}
		if (Parameter != nullptr) {
			if (!Parameters->ContainsKey(Parameter))
				Parameters->Add(Parameter, "true");
		}
	}

	String ^Arguments::default::get(String ^Param)
	{
		return (Parameters[Param]);
	}
}