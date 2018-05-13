#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <vector>
#include <iomanip> // List layout

#include "../headers/folder.hpp"
#include "../../headers/dropboxUtils.hpp"
#include "../../headers/udpUtils.hpp"

using namespace std;

Folder::~Folder() {
  cout << "Remove watch of sync...";
  // TODO: Remove watch when an obj folder is destroyed
}

Folder::Folder(string folderPath) {
  int lenName = folderPath.size();
    this->folderPath = folderPath;
}

int Folder::createFolder(string userId) {
  folderPath = this->folderPath;
  string userFolder = folderPath + userId;
  int folderCreationStatus;
  const char *userFolderChar = userFolder.c_str();

  folderCreationStatus = mkdir(
    userFolderChar,
    S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
  );

  if((folderCreationStatus < 0) && (errno != EEXIST)) {
    throwError("[Folder::createFolder]: Error creating folder");
  }

  return folderCreationStatus;
}

string Folder::getFolderPath() {
  return this->folderPath + END_PATH;
}

vector<string> Folder::getTimes(string filePath) {
  struct stat buffer;
  const char *filePathChar = filePath.c_str();
  vector<string> fileTimes;

  if (stat(filePathChar, &buffer) == ERROR) {
    throwError("[Folder::getTimes]: Error on getting to the file");
  }

  fileTimes.push_back(ctime(&buffer.st_atim.tv_sec));
  fileTimes.push_back(ctime(&buffer.st_mtim.tv_sec));
  fileTimes.push_back(ctime(&buffer.st_ctim.tv_sec));

  return fileTimes;
}

string Folder::timesToString(vector<string> times, int mode) {
  string timesRaw = "";
  for(int i = 0; i < times.size(); i++) {
    times.at(i).erase(times.at(i).length()-1); // Remove \n
    if (mode = CLIENT_LIST) {
      timesRaw += " | " + times.at(i);
    } else {
      timesRaw += times.at(i) + " | ";
    }
  }
  return timesRaw;
}

void Folder::changeDirectory(string folderPath) {
  const char *folder = folderPath.c_str();
  DIR* dir = opendir(folder);
  if (dir) {
    /* If the user does not pass a folder as parameter, folder="", and
     * chdir("") is the same as chdir("./")
     */
    chdir(folder);
  } else if (ENOENT == errno) {
      throwError("[Folder::changeDirectory]: The system cannot find the specified directory");
  } else {
      throwError("[Folder::changeDirectory]: The system has found an error");
  }
}

void Folder::listFolder(string folderPath) {
  const char *folder = folderPath.c_str();
  const char separator = SEPARATOR_LIST;
  const int nameWidth = SIZE_FILENAME_LIST;
  const int numWidth = SIZE_CTIMES_LIST;
  if (strcmp(EMPTY_PATH, folder) == EQUAL) {
    folder = CURRENT_FOLDER;
  }
  DIR * dir = opendir(folder);
  if (dir) {
    cout << FILENAME_LABEL << ACCESS_TIME_LABEL << MODIFICATION_TIME_LABEL
      << CREATION_TIME_LABEL << endl;
    struct dirent *entry;
    while ((entry = readdir(dir)) != EQUAL) {
      if (entry->d_type != DT_DIR) { // If entry is a file
        cout << left << setw(nameWidth) << setfill(separator) << entry->d_name;
        cout << left << setw(numWidth) << setfill(separator) << timesToString(getTimes(folderPath), CLIENT_LIST) << endl;
      }
    }
    closedir(dir);
  } else {
      throwError(strerror(errno));
    }
}

string Folder::listFolderToString(string folderPath) {
  const char *folder = folderPath.c_str();
  const char separator = SEPARATOR_LIST;
  const int nameWidth = SIZE_FILENAME_LIST;
  const int numWidth = SIZE_CTIMES_LIST;
  stringstream folderList;
  string folderListStr;
  if (strcmp(EMPTY_PATH, folder) == EQUAL) {
    folder = CURRENT_FOLDER;
  }
  DIR * dir = opendir(folder);
  if (dir) {
    string fn = FILENAME_LABEL_S;
    string at = ACCESS_TIME_LABEL_S;
    string mt = MODIFICATION_TIME_LABEL_S;
    string ct = CREATION_TIME_LABEL_S;
    string br = BREAK_LINE;
    string formatLabelName = FORMAT_NAME_FILE_S;
    folderListStr += at + mt + ct + fn + br;
    struct dirent *entry;
    while ((entry = readdir(dir)) != EQUAL) {
      if (entry->d_type != DT_DIR) { // If entry is a file
        folderListStr += timesToString(getTimes(folderPath), SERVER_LIST);
        folderListStr += formatLabelName + entry->d_name + br;
      }
    }
    closedir(dir);
  } else {
      throwError(strerror(errno));
  }
  return folderListStr;
}


/* Method for listing files in the client or in the server side, relying on
 * the mode passed as one of the parameters.
 */
string Folder::listFiles(int mode, string userId) {
  int i;
  if (mode == CLIENT_LIST) {
    string homePath;
    string userFolderPath;
    homePath = getpwuid(getuid())->pw_dir; // Get user's home folder
    userFolderPath = homePath + "/sync_dir_" + userId;
    listFolder(userFolderPath);
    return userId;
  } else if (mode == SERVER_LIST) {
      string userFolderDB;
      string db = DATABASE;
      userFolderDB = db  + userId;
      //cout << userFolderDB;
      return listFolderToString(userFolderDB);
  } else {
      throwError("[Folder::listFiles]: Invalid mode");
  }
}
