#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

#include "../headers/file.hpp"
#include "../../headers/dropboxUtils.h"

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

unsigned int getLenOfTheFile(string nameOfTheFile) {
  struct stat info;
  if (stat(nameOfTheFile.c_str(), &info) != EQUAL) {
    return INIT;
  }
  return info.st_size;
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
    printedFilesRawWithTimes += "Name * Creation Time * Modification Time * Access Time\n";
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
