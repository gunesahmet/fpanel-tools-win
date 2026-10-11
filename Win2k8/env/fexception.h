using namespace System;

public ref class FExceptions sealed abstract
{
public:
	static int LineNumber(Exception ^ex)
	{
		int lineNumber = 0;
		String ^ const lineSearch = ":line ";
		int index = ex->StackTrace->LastIndexOf(lineSearch);
		if (index != -1) {
			String ^lineNumberText = ex->StackTrace->Substring(index + lineSearch->Length);
			if (int::TryParse(lineNumberText, lineNumber)) {
			}
		}
		return lineNumber;
	}
};