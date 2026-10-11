#include "../env/Env.h"
#include "../env/Logger.h"
#include "../env/Validations.h"
#include "../env/Networking.h"
#include "../env/MainCase.h"

using namespace System;
using namespace System::IO;
using namespace Environments;
using namespace System::Text::RegularExpressions;

namespace Tools
{
	private ref class filemng
	{
	public:
		static void Main(array<String^> ^Args);
		value class ChildList
		{
		public:
			String ^type;
			String ^name;
			String ^mod_time;
			String ^size;
		};
		value class FileList
		{
		public:
			String ^name;
			List<ChildList> ^childs;
		};

		value class FolderList
		{
		public:
			String ^name;
			List<FolderList> ^childs;
		};
	private:
		static String ^DIRECTORY_SEPERATOR = "\\";

		static FUtils ^utils = gcnew FUtils();
		static Env ^env = gcnew Env();
		static FileLogger ^logger = gcnew FileLogger();
		static Validations ^validate = gcnew Validations();
		static Networking ^network = gcnew Networking();

		static String ^dir;
		static String ^domain;
		static String ^prefix;
		static String ^address;
		static String ^user;
		static String ^folder_name;
		static String ^new_folder_name;
		static String ^file_name;
		static String ^new_file_name;
		static String ^file_content;
		static String ^file_path;
		static String ^source_file;
		static String ^source_dir;
		static String ^target_dir;
		static String ^path;
		static String ^password;
		static String ^ftp_addr;


		static void scanRecursive(String ^current_main_path, String ^dir_name, filemng::FolderList ^folder_list);
		static void fileList(String ^dir_name);
		static void createFile(String ^dir_name, String ^file_name);
		static void createFolder(String ^dir_name, String ^file_name);
		static void renameFile(String ^dir_name, String ^file_name, String ^new_file_name);
		static void removeFile(String ^dir_name, String ^file_name);
		static void getFileContent(String ^path, String ^file_path);
		static void saveFileContent(String ^_path, String ^_file_name, String ^_file_content, String ^_file_path);
		static void folderList(String ^dir);
		static void renameFolder(String ^dir_name, String ^folder_name, String ^new_folder_name);
		static void removeFolder(String ^dir_name, String ^folder_name);
		static void moveFileFolder(String ^source_path, String ^source_file, String ^target_path);
		static void getPermission();
		static void setPermission();
		static void uploadFtp(String ^dir, String ^username, String ^password, String ^file_name, String ^ftp_address, String ^source_file);
	};
}