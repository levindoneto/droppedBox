#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>

#include "../headers/folder.hpp"
#include "../../headers/dropboxUtils.hpp"

using namespace std;

Folder::~Folder() {
  cout << "Remove watch of sync...";
  // TODO: Remove watch when an obj folder is destroyed
}

Folder::Folder(string folderPath) {
  int lenName = folderPath.size();

  //if (folderPath.back() != END_PATH) { // str::back = last char of the string
    this->folderPath = folderPath;
  //} else {
    //this->folderPath = folderPath.substr(BEGIN_STR, lenName-1); // delete last char (/)
  //}
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
    throwError("Error creating folder");
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

  if (stat(filePathChar, &buffer) == -1) {
    throwError("Error:");
  }

  fileTimes.push_back(ctime(&buffer.st_atim.tv_sec));
  fileTimes.push_back(ctime(&buffer.st_mtim.tv_sec));
  fileTimes.push_back(ctime(&buffer.st_ctim.tv_sec));

  return fileTimes;
}
