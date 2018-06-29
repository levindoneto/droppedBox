#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

#include "../headers/file.hpp"
#include "../../headers/dropboxUtils.h"

File::File(string filepath)
  : filepath(filepath), modification_time_str(""), access_time_str(""), creation_time_str("") {
  update_info();
}

string File::time_to_string(time_t timestamp) {
  char buffer[256];
  struct tm *tm = localtime(&timestamp);
  strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m/%Y", tm);
  return string(buffer, strlen("HH:MM:SS DD/MM/YYYY"));
}

bool File::update_info() {
  if (stat(filepath.c_str(), &info) >= INIT) {
    modification_time_str = time_to_string(info.st_mtime);
    access_time_str = time_to_string(info.st_atime);
    creation_time_str = time_to_string(info.st_ctime);
    return true;
  }
  else {
    return false;
  }
}

list<File> File::list_directory(string dirpath) {
  struct dirent *dir_entry;
  list<File> listOfFileInfo;
  DIR *directory;
  directory = opendir(dirpath.c_str());
  if (directory) {
    while ((dir_entry = readdir(directory))) {
      string filename = string(dir_entry->d_name);
      if (filename != "." && filename != "..") {
        File file(dirpath + SLASH + filename);
        listOfFileInfo.push_back(file);
      }
    }
  }
  closedir(directory);
  return listOfFileInfo;
}

list<string> File::listNamesOfFiles(string dirpath) {
  list <string> nameOfTheFiles;
  DIR *directoryPointer;
  struct dirent *ep;
  directoryPointer = opendir(dirpath.c_str());
  if (directoryPointer != NULL) {
    while ((ep = readdir(directoryPointer))) {
      if(strcmp(ep->d_name, "..") != EQUAL && strcmp(ep->d_name, CURRENT_FOLDER) != EQUAL) {
        nameOfTheFiles.push_back(ep->d_name);
      }
    }
    (void) closedir (directoryPointer);
  }
  else {
    char error[ERROR_MSG_SIZE] = "Error on opening the directory";
    throwError(error);
  }
  return nameOfTheFiles;
}

string File::stringify() {
  string fileAsAstring = "";
  fileAsAstring += obatingNameOfTheFile(filepath);
  fileAsAstring += SEP_LIST_NO_SPACES;
  fileAsAstring += creation_time_str;
  fileAsAstring += SEP_LIST_NO_SPACES;
  fileAsAstring += modification_time_str;
  fileAsAstring += SEP_LIST_NO_SPACES;
  fileAsAstring += access_time_str;
  fileAsAstring += SEP_LIST_NO_SPACES;
  return fileAsAstring;
}

string File::list_directory_str(string dirpath) {
  string folderInAList = "";
  list<File> folderListGot = File::list_directory(dirpath);
  for (File &file : folderListGot) {
    folderInAList += file.stringify() + "\n";
  }
  return folderInAList;
}

unsigned int getLenOfTheFile(string nameOfTheFile) {
  struct stat info;
  if (stat(nameOfTheFile.c_str(), &info) != EQUAL) {
    return INIT;
  }
  return info.st_size;
}

string File::working_directory() {
    return string(getenv("PWD"));
}

void File::print_listOfFileInfo(string listOfFileInfo) {
  if (listOfFileInfo.empty()) listOfFileInfo = "There is no file...";
  listOfFileInfo.insert(INIT, "Name | Creation time | Modification time |Last access time|\n");
  print_table(listOfFileInfo);
}

string File::ll(string dirpath) {
  DIR *directoryPointer;
  struct dirent *ep;
  struct stat fileInfo;
  string printedFilesRawWithTimes = "";
  struct tm *tm;
  char modificationTime[ISO_TIME_LEN_MAX] = {INIT};
  char creationTime[ISO_TIME_LEN_MAX] = {INIT};
  char accessTime[ISO_TIME_LEN_MAX] = {INIT};

  directoryPointer = opendir(dirpath.c_str());
  if (directoryPointer != NULL) {
    printedFilesRawWithTimes += "Name | Creation Time | Modification Time | Access Time\n";
    while ((ep = readdir(directoryPointer))) {
      if(strcmp(ep->d_name, "..") != EQUAL && strcmp(ep->d_name, CURRENT_FOLDER) != EQUAL) {
        char fullpath[LEN_PATH_FOLDER];
        strcpy(fullpath,dirpath.c_str());
        strcat(fullpath,"/");
        strcat(fullpath,ep->d_name);

        if (stat(fullpath, &fileInfo) == ERROR) {
          char error[ERROR_MSG_SIZE] = "Issue on opening the directory";
          printf(error);
          continue;
        }
        tm = localtime(&fileInfo.st_mtime);
        strftime(modificationTime, sizeof(modificationTime), "%H:%M:%S %d/%m/%Y", tm);
        tm = localtime(&fileInfo.st_ctime);
        strftime(creationTime, sizeof(creationTime), "%H:%M:%S %d/%m/%Y", tm);
        tm = localtime(&fileInfo.st_atime);
        strftime(accessTime, sizeof(accessTime), "%H:%M:%S %d/%m/%Y", tm);

        printedFilesRawWithTimes += ep->d_name;
        printedFilesRawWithTimes += "| ";
        printedFilesRawWithTimes += creationTime;
        printedFilesRawWithTimes += "| ";
        printedFilesRawWithTimes += modificationTime;
        printedFilesRawWithTimes += "| ";
        printedFilesRawWithTimes += accessTime;
        printedFilesRawWithTimes += "|\n";
      }
    }
    (void) closedir (directoryPointer);
  }
  else {
    char error[ERROR_MSG_SIZE] = "Error on opening the directory";
    throwError(error);
  }
  return printedFilesRawWithTimes;
}

void File::createFolderForFiles(string path) {
  int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status < EQUAL && errno != EEXIST) {
    throwError("error creating folder for files");
  }
}

void File::set_modification_time(int modtime) {
  struct utimbuf ubuf;
  ubuf.modtime = modtime;
  if (utime(filepath.c_str(), &ubuf) == EQUAL) update_info();
  else perror("utime - error");
}

unsigned int File::size() {
  return info.st_size;
}

unsigned int File::modification_time() {
  if (update_info()) return info.st_mtime;
  else return INIT;
}

string File::name() {
    return obtaingJustNameOfTheFile(filepath);
}

File File::parse_file(string textFile) {
  int name_sep = textFile.find('|');
  int creation_sep = textFile.find('|', name_sep + 1);
  int mod_sep = textFile.find('|', creation_sep + 1);
  int access_sep = textFile.find_last_of('|');
  int name_len = name_sep;
  int mod_len = mod_sep - creation_sep - 1;
  int access_len = access_sep - mod_sep - 1;
  int creation_len = creation_sep - name_sep - 1;
  string name = textFile.substr(INIT, name_len);
  string creation_time = textFile.substr(name_sep + 1, creation_len);
  string mod_time = textFile.substr(creation_sep + 1, mod_len);
  string access_time = textFile.substr(mod_sep + 1, access_len);
  File file(name);
  file.creation_time_str = creation_time;
  file.modification_time_str = mod_time;
  file.access_time_str = access_time;

  return file;
}
