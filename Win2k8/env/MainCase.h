using namespace System;
using namespace System::Collections::Specialized;
using namespace System::Text::RegularExpressions;

namespace Environments
{
	public ref class Arguments
	{
	private:
		StringDictionary ^Parameters;
	public:
		Arguments(array<String^> ^Args);
		property String ^default[String^]
		{
			String ^get(String ^Param);
		}
	};
}