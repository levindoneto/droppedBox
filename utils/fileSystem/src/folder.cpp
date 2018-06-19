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
#include "../../headers/dropboxUtils.h"
#include "../../headers/udpUtils.hpp"

using namespace std;

string getHome() {
  string homePath;
  string userFolderPath;
  homePath = getpwuid(getuid())->pw_dir; // Get user's home folder
  return homePath;
}

vector<string> getTimes(string filePath) {
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

string timesToString(vector<string> times, int mode) {
  string timesRaw = "";
  for(int i = 0; i < times.size(); i++) {
    times.at(i).erase(times.at(i).length()-1); // Remove \n
    if (mode == CLIENT_LIST) {
      timesRaw += " | " + times.at(i);
    } else {
      timesRaw += times.at(i) + " | ";
    }
  }
  return timesRaw;
}

void listFolder(string folderPath) {
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
        string nameOfTheFile = entry->d_name;
        string slash = SLASH;
        string filePath = folderPath + slash + nameOfTheFile;
        cout << left << setw(nameWidth) << setfill(separator) << entry->d_name;
        cout << left << setw(numWidth) << setfill(separator) << timesToString(getTimes(filePath), CLIENT_LIST) << endl;
      }
    }
    closedir(dir);
  } else {
      throwError(strerror(errno));
    }
}

/* Method for listing files in the client or in the server side, relying on
 * the mode passed as one of the parameters.
 */
string listFiles(int mode, string userId) {
  int i;
  if (mode == CLIENT_LIST) {
    string userFolderPath;
    userFolderPath = getHome() + SYNC_DIR_PREFIX + userId;
    listFolder(userFolderPath);
    return userId;
  } else {
      throwError("[Folder::listFiles]: Invalid mode");
  }
}

/* Method for verifyinf if a file exists with a path */
bool fileInFolder(string filePath) {
  if(!ifstream(filePath)) return false;
  else return true;
}
