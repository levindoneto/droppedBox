#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>

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

/* Method for listing files in the client or in the server side, relying on
 * the mode passed as one of the parameters.
 */

void changeDirectory(string folderPath) {
  const char *folder = folderPath.c_str();
  DIR* dir = opendir(folder);
  if (dir) {
     /* If the user does not pass a folder as parameter, folder="", and
      * chdir("") is the same as chdir("./")
      */
      chdir(folder);
  }
  else if (ENOENT == errno) {
      printf("The system cannot find the specified directory");
  }
  else {
      printf("\nThe system has found an error\n");
  }
}

void listFiles(int mode, string userId) {
  int i;
  if (mode == CLIENT_LIST) {
    cout << "todo";
  } else if (mode == SERVER_LIST) {
    cout << "todo";
      char buffer[BUFFER_SIZE]; // 1 KB buffer
      //sprintf(buffer, "%d",user->getNumberOfFiles());
      //writeToSocket(socket, buffer);
      /*
      for(f = 0; f < user->getNumberOfFiles(); f++) {
        sprintf(buffer, "%s%s%s \n\t- MT: %s%s%s",
          COLOR_GREEN, client->file_info[i].name, COLOR_RESET,
          COLOR_YELLOW, client->file_info[i].last_modified, COLOR_RESET
        );
        write_to_socket(socket, buffer);
      }
      */
  } else {
      throwError("[List of Files]: Invalid option");
  }
}
