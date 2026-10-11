//
// This code has been adapted from http://www.codeproject.com/KB/cs/lsadotnet.aspx
// The rights enumeration code came from http://www.tech-archive.net/Archive/DotNet/microsoft.public.dotnet.framework.interop/2004-11/0394.html
//
// Windows Security via .NET is covered on by Pluralsight:http://alt.pluralsight.com/wiki/default.aspx/Keith.GuideBook/HomePage.html
//

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;
using namespace System::Text;

namespace LSA
{
	//
	// Provides methods the local security authority which controls user rights. Managed via secpol.msc normally.
	//
	public ref class LocalSecurityAuthorityController
	{
	public:
		[StructLayout(LayoutKind::Sequential)]
		value class LSA_UNICODE_STRING
		{
		public:
			UInt16 Length;
			UInt16 MaximumLength;
			IntPtr Buffer;
		};

	private:
		[StructLayout(LayoutKind::Sequential)]
		value class LSA_OBJECT_ATTRIBUTES
		{
		public:
			int Length;
			IntPtr RootDirectory;
			LSA_UNICODE_STRING ObjectName;
			UInt32 Attributes;
			IntPtr SecurityDescriptor;
			IntPtr SecurityQualityOfService;
		};

	private:
		[Flags]
		enum class LSA_AccessPolicy : Int64
		{
			POLICY_VIEW_LOCAL_INFORMATION = 0x00000001LL,
			POLICY_VIEW_AUDIT_INFORMATION = 0x00000002LL,
			POLICY_GET_PRIVATE_INFORMATION = 0x00000004LL,
			POLICY_TRUST_ADMIN = 0x00000008LL,
			POLICY_CREATE_ACCOUNT = 0x00000010LL,
			POLICY_CREATE_SECRET = 0x00000020LL,
			POLICY_CREATE_PRIVILEGE = 0x00000040LL,
			POLICY_SET_DEFAULT_QUOTA_LIMITS = 0x00000080LL,
			POLICY_SET_AUDIT_REQUIREMENTS = 0x00000100LL,
			POLICY_AUDIT_LOG_ADMIN = 0x00000200LL,
			POLICY_SERVER_ADMIN = 0x00000400LL,
			POLICY_LOOKUP_NAMES = 0x00000800LL,
			POLICY_NOTIFICATION = 0x00001000LL
		};

	private:
		literal int Access = safe_cast<int>(LSA_AccessPolicy::POLICY_AUDIT_LOG_ADMIN | LSA_AccessPolicy::POLICY_CREATE_ACCOUNT | LSA_AccessPolicy::POLICY_CREATE_PRIVILEGE | LSA_AccessPolicy::POLICY_CREATE_SECRET | LSA_AccessPolicy::POLICY_GET_PRIVATE_INFORMATION | LSA_AccessPolicy::POLICY_LOOKUP_NAMES | LSA_AccessPolicy::POLICY_NOTIFICATION | LSA_AccessPolicy::POLICY_SERVER_ADMIN | LSA_AccessPolicy::POLICY_SET_AUDIT_REQUIREMENTS | LSA_AccessPolicy::POLICY_SET_DEFAULT_QUOTA_LIMITS | LSA_AccessPolicy::POLICY_TRUST_ADMIN | LSA_AccessPolicy::POLICY_VIEW_AUDIT_INFORMATION | LSA_AccessPolicy::POLICY_VIEW_LOCAL_INFORMATION);

		[DllImport("advapi32.dll", PreserveSig = true)]
		static UInt32 LsaOpenPolicy(LSA_UNICODE_STRING %SystemName, LSA_OBJECT_ATTRIBUTES %ObjectAttributes, Int32 DesiredAccess, [System::Runtime::InteropServices::Out] IntPtr %PolicyHandle);

		[DllImport("advapi32.dll", SetLastError = true, PreserveSig = true)]
		static UInt32 LsaAddAccountRights(IntPtr PolicyHandle, IntPtr AccountSid, array<LSA_UNICODE_STRING> ^UserRights, int CountOfRights);

	public:
		[DllImport("advapi32")]
		static void FreeSid(IntPtr pSid);

	private:
		[DllImport("advapi32.dll", CharSet = CharSet::Auto, SetLastError = true, PreserveSig = true)]
		static bool LookupAccountName(String ^lpSystemName, String ^lpAccountName, IntPtr psid, int %cbsid, StringBuilder ^domainName, int %cbdomainLength, int %use);

		[DllImport("advapi32.dll")]
		static bool IsValidSid(IntPtr pSid);

		[DllImport("advapi32.dll")]
		static int LsaClose(IntPtr ObjectHandle);

		[DllImport("kernel32.dll")]
		static int GetLastError();

		[DllImport("advapi32.dll")]
		static int LsaNtStatusToWinError(UInt32 status);

		[DllImport("advapi32.dll", SetLastError = true, PreserveSig = true)]
		static UInt32 LsaEnumerateAccountRights(IntPtr PolicyHandle, IntPtr AccountSid, [System::Runtime::InteropServices::Out] IntPtr %UserRightsPtr, [System::Runtime::InteropServices::Out] int %CountOfRights);

		// Returns the Local Security Authority rights granted to the account
	public:
		IList<String^> ^GetRights(String ^accountName)
		{
			IList<String^> ^rights = gcnew List<String^>();
			String ^errorMessage = String::Empty;

			Int64 winErrorCode = 0;
			IntPtr sid = IntPtr::Zero;
			int sidSize = 0;
			StringBuilder ^domainName = gcnew StringBuilder();
			int nameSize = 0;
			int accountType = 0;

			LookupAccountName(String::Empty, accountName, sid, sidSize, domainName, nameSize, accountType);

			domainName = gcnew StringBuilder(nameSize);
			sid = Marshal::AllocHGlobal(sidSize);

			if (!LookupAccountName(String::Empty, accountName, sid, sidSize, domainName, nameSize, accountType))
			{
				winErrorCode = GetLastError();
				errorMessage = ("LookupAccountName failed: " + winErrorCode);
				throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
			}
			else
			{
				LSA_UNICODE_STRING systemName = LSA_UNICODE_STRING();

				IntPtr policyHandle = IntPtr::Zero;
				IntPtr userRightsPtr = IntPtr::Zero;
				int countOfRights = 0;

				LSA_OBJECT_ATTRIBUTES objectAttributes = CreateLSAObject();

				UInt32 policyStatus = LsaOpenPolicy(systemName, objectAttributes, Access, policyHandle);
				winErrorCode = LsaNtStatusToWinError(policyStatus);

				if (winErrorCode != 0)
				{
					errorMessage = String::Format("OpenPolicy failed: {0}.", winErrorCode);
					throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
				}
				else
				{
					try
					{
						UInt32 result = LsaEnumerateAccountRights(policyHandle, sid, userRightsPtr, countOfRights);
						winErrorCode = LsaNtStatusToWinError(result);
						if (winErrorCode != 0)
						{
							if (winErrorCode == 2)
							{
								return gcnew List<String^>();
							}
							errorMessage = String::Format("LsaEnumerateAccountRights failed: {0}", winErrorCode);
							throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
						}

						Int32 ptr = userRightsPtr.ToInt32();
						LSA_UNICODE_STRING userRight;

						for (int i = 0; i < countOfRights; i++)
						{
							userRight = safe_cast<LSA_UNICODE_STRING>(Marshal::PtrToStructure(IntPtr(ptr), LSA_UNICODE_STRING::typeid));
							String ^userRightStr = Marshal::PtrToStringAuto(userRight.Buffer);
							rights->Add(userRightStr);
							ptr += Marshal::SizeOf(userRight);
						}
					}
					finally
					{
						LsaClose(policyHandle);
					}
				}
				FreeSid(sid);
			}
			return rights;
		}

		// Adds a privilege to an account
		void SetRight(String ^accountName, String ^privilegeName)
		{
			Int64 winErrorCode = 0;
			String ^errorMessage = String::Empty;

			IntPtr sid = IntPtr::Zero;
			int sidSize = 0;
			StringBuilder ^domainName = gcnew StringBuilder();
			int nameSize = 0;
			int accountType = 0;

			LookupAccountName(String::Empty, accountName, sid, sidSize, domainName, nameSize, accountType);

			domainName = gcnew StringBuilder(nameSize);
			sid = Marshal::AllocHGlobal(sidSize);

			if (!LookupAccountName(String::Empty, accountName, sid, sidSize, domainName, nameSize, accountType))
			{
				winErrorCode = GetLastError();
				errorMessage = String::Format("LookupAccountName failed: {0}", winErrorCode);
				throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
			}
			else
			{
				LSA_UNICODE_STRING systemName = LSA_UNICODE_STRING();
				IntPtr policyHandle = IntPtr::Zero;
				LSA_OBJECT_ATTRIBUTES objectAttributes = CreateLSAObject();

				UInt32 resultPolicy = LsaOpenPolicy(systemName, objectAttributes, Access, policyHandle);
				winErrorCode = LsaNtStatusToWinError(resultPolicy);

				if (winErrorCode != 0)
				{
					errorMessage = String::Format("OpenPolicy failed: {0} ", winErrorCode);
					throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
				}
				else
				{
					try
					{
						array<LSA_UNICODE_STRING> ^userRights = gcnew array<LSA_UNICODE_STRING>(1);
						userRights[0] = LSA_UNICODE_STRING();
						userRights[0].Buffer = Marshal::StringToHGlobalUni(privilegeName);
						userRights[0].Length = safe_cast<UInt16>(privilegeName->Length * UnicodeEncoding::CharSize);
						userRights[0].MaximumLength = safe_cast<UInt16>((privilegeName->Length + 1) * UnicodeEncoding::CharSize);

						UInt32 res = LsaAddAccountRights(policyHandle, sid, userRights, 1);
						winErrorCode = LsaNtStatusToWinError(res);
						if (winErrorCode != 0)
						{
							errorMessage = String::Format("LsaAddAccountRights failed: {0}", winErrorCode);
							throw gcnew Win32Exception(safe_cast<int>(winErrorCode), errorMessage);
						}
					}
					finally
					{
						LsaClose(policyHandle);
					}
				}
				FreeSid(sid);
			}
		}

	private:
		static LSA_OBJECT_ATTRIBUTES CreateLSAObject()
		{
			LSA_OBJECT_ATTRIBUTES newInstance = LSA_OBJECT_ATTRIBUTES();

			newInstance.Length = 0;
			newInstance.RootDirectory = IntPtr::Zero;
			newInstance.Attributes = 0;
			newInstance.SecurityDescriptor = IntPtr::Zero;
			newInstance.SecurityQualityOfService = IntPtr::Zero;

			return newInstance;
		}
	};

	// Local security rights managed by the Local Security Authority
	public ref class LocalSecurityAuthorityRights
	{
		// Log on as a service right
	public:
		literal String ^LogonAsService = "SeServiceLogonRight";
		// Log on as a batch job right
		literal String ^LogonAsBatchJob = "SeBatchLogonRight";
		// Interactive log on right
		literal String ^InteractiveLogon = "SeInteractiveLogonRight";
		// Network log on right
		literal String ^NetworkLogon = "SeNetworkLogonRight";
		// Generate security audit logs right
		literal String ^GenerateSecurityAudits = "SeAuditPrivilege";
	};

	/* added wrapper for PowerShell */

	public ref class LSAWrapper
	{
	public:
		static IList<String^> ^GetRights(String ^accountName)
		{
			return (gcnew LocalSecurityAuthorityController())->GetRights(accountName);
		}

		static void SetRight(String ^accountName, String ^privilegeName)
		{
			(gcnew LocalSecurityAuthorityController())->SetRight(accountName, privilegeName);
		}
	};
}