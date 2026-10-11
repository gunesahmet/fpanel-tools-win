#include "filemng.h"

using namespace System;
using namespace System::IO;
using namespace System::Web::Script::Serialization;

namespace Tools
{
	void filemng::Main(array<String^> ^Args)
	{
		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str, 999);


		if (utils->isAdmin() == false) {
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0) {
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");

		dir = main_case["dir"];
		domain = main_case["domain"];
		prefix = main_case["prefix"];
		address = main_case["address"];
		user = main_case["user"];
		folder_name = main_case["folder-name"];
		new_folder_name = main_case["new-folder-name"];
		file_name = main_case["file-name"];
		new_file_name = main_case["new-file-name"];
		file_content = main_case["file-content"];
		file_path = main_case["file-path"];
		source_file = main_case["source-file"];
		source_dir = main_case["source-dir"];
		target_dir = main_case["target-dir"];
		path = main_case["path"];
		password = main_case["password"];
		ftp_addr = main_case["address"];

		if (process == "file-list") {
			fileList(dir);
		}
		else if (process == "create-file") {
			createFile(dir, file_name);
		}
		else if (process == "move-file") {
			moveFileFolder(source_dir, source_file, target_dir);
		}
		else if (process == "rename-file") {
			renameFile(dir, file_name, new_file_name);
		}
		else if (process == "remove-file") {
			removeFile(dir, file_name);
		}
		else if (process == "get-file-content") {
			getFileContent(dir, file_name);
		}
		else if (process == "save-file-content") {
			saveFileContent(dir, file_name, file_content, file_path);
		}
		else if (process == "folder-list") {
			folderList(dir);
		}
		else if (process == "create-folder") {
			createFolder(dir, folder_name);
		}
		else if (process == "rename-folder") {
			renameFolder(dir, folder_name, new_folder_name);
		}
		else if (process == "remove-folder") {
			removeFolder(dir, folder_name);
		}
		else if (process == "get-permission") {
			getPermission();
		}
		else if (process == "set-permission") {
			setPermission();
		}
		else if (process == "upload-ftp") {
			uploadFtp(dir, user, password, file_name, ftp_addr, source_file);
		}
		else {
			System::Environment::Exit(0);
		}
	}

	// Bitti
	void filemng::scanRecursive(String ^current_main_path, String ^dir_name, filemng::FolderList ^folder_list)
	{
		array<String^> ^all_files = Directory::GetDirectories(current_main_path + dir_name, "*");
		folder_list->name = dir_name;
		folder_list->childs = gcnew List<filemng::FolderList>();
		if (all_files->Length > 0) {
			for (int i = 0; i < all_files->Length; i++) {
				filemng::FolderList ^folder_list_childs = gcnew filemng::FolderList();
				String ^current_folder_name = all_files[i]->Replace(current_main_path + dir_name + "\\", "");
				scanRecursive(current_main_path + dir_name + "\\" , current_folder_name, folder_list_childs);
				folder_list->childs->Add(*folder_list_childs);
			}
		}
	}

	// Bitti
	void filemng::fileList(String ^dir_name)
	{
		if (dir_name == DIRECTORY_SEPERATOR) {
			dir_name = "";
		}
		else if (dir_name == "/") {
			dir_name = "";
		}
		else {
			utils->replaceAll(dir_name, "/", DIRECTORY_SEPERATOR);
			utils->replaceAll(dir_name, "\\\\", DIRECTORY_SEPERATOR);
		}

		String ^main_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR;
		utils->replaceAll(main_path, "\\\\", DIRECTORY_SEPERATOR);

		filemng::FileList ^file_list = gcnew filemng::FileList();
		file_list->name = dir_name;
		file_list->childs = gcnew List<filemng::ChildList>();
		array<String^> ^all_folders = Directory::GetDirectories(env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name, "*");

		for (int i = 0; i < all_folders->Length; i++) {
			String ^current_file = all_folders[i];
			utils->replaceAll(current_file, "\\\\", DIRECTORY_SEPERATOR);

			FileInfo ^file_info = gcnew FileInfo(current_file);
			String ^current_file_name = current_file->Replace(env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name, "");
			current_file_name = current_file_name->Replace(main_path, "");
			utils->replaceAll(current_file_name, "\\\\", DIRECTORY_SEPERATOR);

			//Console::WriteLine(current_file);
			//Console::WriteLine(main_path);
			//Console::WriteLine(current_file_name);

			ChildList ^child_list = gcnew ChildList();
			FileAttributes attr = File::GetAttributes(current_file);
			child_list->type = "dir";
			child_list->size = "4096";

			child_list->name = current_file_name;
			child_list->mod_time = file_info->LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss");
			file_list->childs->Add(*child_list);
		}
	
		array<String^> ^all_files = Directory::GetFiles(env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name, "*");
		for (int i = 0; i < all_files->Length; i++) {
			FileInfo ^file_info = gcnew FileInfo(all_files[i]);
			String ^current_file = all_files[i];
			utils->replaceAll(current_file, "\\\\", DIRECTORY_SEPERATOR);

			String ^current_file_name = current_file->Replace(main_path, "");
			utils->replaceAll(current_file_name, "\\\\", DIRECTORY_SEPERATOR);

			//Console::WriteLine(all_files[i]);
			//Console::WriteLine(main_path);
			//Console::WriteLine(current_file_name);

			ChildList ^child_list = gcnew ChildList();
			child_list->type = "file";
			child_list->name = current_file_name;
			child_list->size = file_info->Length.ToString();
			child_list->mod_time = file_info->LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss");
			file_list->childs->Add(*child_list);
		}

		JavaScriptSerializer ^serializer = gcnew JavaScriptSerializer();
		System::String ^serializedResult = serializer->Serialize(file_list);
		Console::WriteLine(serializedResult);
	}

	// Bitti
	void filemng::folderList(String ^dir_name)
	{
		if (dir_name == DIRECTORY_SEPERATOR) {
			dir_name = "";
		}
		else if (dir_name == "/") {
			dir_name = "";
		}
		else {
			utils->replaceAll(dir_name, "/", DIRECTORY_SEPERATOR);
			utils->replaceAll(dir_name, "\\\\", DIRECTORY_SEPERATOR);
		}

		String ^main_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR;
		utils->replaceAll(main_path, "\\\\", DIRECTORY_SEPERATOR);

		filemng::FolderList ^folder_list = gcnew filemng::FolderList();
		array<String^> ^all_files = Directory::GetDirectories(env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name, "*");
		folder_list->name = dir_name;
		folder_list->childs = gcnew List<filemng::FolderList>();
		for (int i = 0; i < all_files->Length; i++) {
			filemng::FolderList ^folder_list_childs = gcnew filemng::FolderList();

			String ^current_file = all_files[i];
			utils->replaceAll(current_file, "\\\\", DIRECTORY_SEPERATOR);

			String ^current_file_name = current_file->Replace(main_path, "");
			utils->replaceAll(current_file_name, "\\\\", DIRECTORY_SEPERATOR);
			scanRecursive(main_path, current_file_name, folder_list_childs);
			folder_list->childs->Add(*folder_list_childs);
		}

		JavaScriptSerializer ^serializer = gcnew JavaScriptSerializer();
		System::String ^serializedResult = serializer->Serialize(folder_list);
		Console::WriteLine(serializedResult);
	}

	// Bitti
	void filemng::createFile(String ^dir_name, String ^file_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + file_name;
			if (File::Exists(file_path)) {
				return;
			}
			file_path = file_path->Replace("/", "\\");
			FileStream ^file = File::Create(file_path);
			file->Close();
		}
		catch (...) {

		}
	}

	// Bitti
	void filemng::createFolder(String ^dir_name, String ^folder_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + folder_name;
			file_path = file_path->Replace("/", "\\");
			if (Directory::Exists(file_path)) {
				return;
			}

			DirectoryInfo ^dir_info = Directory::CreateDirectory(file_path);
		}
		catch(...) {

		}
		
	}
	
	//Bitti
	void filemng::removeFolder(String ^dir_name, String ^folder_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + folder_name;
			file_path = file_path->Replace("//", "/");
			file_path = file_path->Replace("/", "\\");
			Console::WriteLine(file_path);
			if (!Directory::Exists(file_path)) {
				return;
			}
			DirectoryInfo ^dir_info = gcnew DirectoryInfo(file_path);
			dir_info->Delete(true);
		}
		catch (...) {

		}
	}

	//Bitti
	void filemng::removeFile(String ^dir_name, String ^file_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + file_name;
			if (!File::Exists(file_path)) {
				return;
			}
			FileInfo ^file_info = gcnew FileInfo(file_path);
			file_info->Delete();
		}
		catch (...) {

		}
	}

	// Bitti
	void filemng::renameFile(String ^dir_name, String ^file_name, String ^new_file_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + file_name;
			String ^new_file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + new_file_name;
			if (!File::Exists(file_path)) {
				return;
			}
			FileInfo ^file_info = gcnew FileInfo(file_path);
			file_info->MoveTo(new_file_path);
		}
		catch (...) {
		}
	}

	// Bitti
	void filemng::renameFolder(String ^dir_name, String ^folder_name, String ^new_folder_name)
	{
		try {
			dir_name = dir_name->Replace("/\\", "/");
			if (dir_name == "/") {
				return;
			}
			String ^dir_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + folder_name;
			String ^new_dir_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + dir_name + DIRECTORY_SEPERATOR + new_folder_name;
			if (!Directory::Exists(dir_path)) {
				return;
			}
			DirectoryInfo ^dir_info = gcnew DirectoryInfo(dir_path);
			dir_info->MoveTo(new_dir_path);
		}
		catch (...) {
		}
	}

	// Bitti
	void filemng::moveFileFolder(String ^source_path, String ^source_file, String ^target_path)
	{
		try {
			source_path = source_path->Replace("..", "");
			source_path = source_path->Replace("/\\", "/");
			target_path = target_path->Replace("..", "");
			target_path = target_path->Replace("/\\", "/");

			source_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + source_path + DIRECTORY_SEPERATOR + source_file;
			target_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + target_path + DIRECTORY_SEPERATOR + source_file;

			FileAttributes attr = File::GetAttributes(source_path);

			if ((attr & FileAttributes::Directory) == FileAttributes::Directory) {
				DirectoryInfo ^dir_info = gcnew DirectoryInfo(source_path);
				dir_info->MoveTo(target_path);
			}
			else {
				FileInfo ^file_info = gcnew FileInfo(source_path);
				file_info->MoveTo(target_path);
			}
		}
		catch (...) {
		}
	}

	// Bitti
	void filemng::getFileContent(String ^path, String ^file_path)
	{
		if (path == "/") {
			path = "";
		}
		else {
			path = path->Replace("/", "\\");
		}
		file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + path + DIRECTORY_SEPERATOR + file_path;
		file_path->Replace("//", "/");
		String ^text = System::IO::File::ReadAllText(file_path);
		Console::WriteLine(utils->Base64Encode(text));
	}

	// Bitti
	void filemng::saveFileContent(String ^_dir, String ^_file_name, String ^_file_content, String ^_file_path)
	{
		if (_dir == "/") {
			_dir = "";
		}
		else {
			_dir = _dir->Replace("/", "\\");
		}

		String ^w_file_path = env->vhosts_dir + prefix + DIRECTORY_SEPERATOR + _dir + DIRECTORY_SEPERATOR + _file_name;
		String ^fcontent;
		if (!String::IsNullOrEmpty(_file_path)) {
			fcontent = utils->Base64Decode(File::ReadAllText(_file_path)->Replace("\"", ""));
		}
		else {
			fcontent = utils->Base64Decode(_file_content);
		}
		System::IO::File::WriteAllText(w_file_path, fcontent);
	}

	// Bitti
	void filemng::getPermission()
	{

	}

	// Bitti
	void filemng::setPermission()
	{

	}

	// Bitti
	void filemng::uploadFtp(String ^dir, String ^username, String ^password, String ^file_name, String ^ftp_address, String ^source_file)
	{
		dir = dir->Replace("\\", "/");
		dir = dir->Replace("//", "/");
		source_file = source_file->Replace("\\\\", "\\");
		password = utils->exec(env->fcrypt, "-d --in=\"" + password + "\"");
		password = utils->Trim(password);
		System::Net::WebClient ^client = gcnew System::Net::WebClient();
		client->Credentials = gcnew System::Net::NetworkCredential(username, password);
		client->UploadFile("ftp://"+ address + dir + DIRECTORY_SEPERATOR + file_name, System::Net::WebRequestMethods::Ftp::UploadFile, source_file);
	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	filemng ^mainProcess = gcnew filemng();
	mainProcess->Main(Args);
	return 0;
}