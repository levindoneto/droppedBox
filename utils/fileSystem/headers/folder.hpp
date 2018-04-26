#pragma once

#include <string>

#define BEGIN_STR 0
#define END_PATH '/' // "/" would be a apointer to a string with len=1

using namespace std;

class Folder {
  private:
    int inotify;
    string folderPath;
  public:
    ~Folder();
    Folder() {};
    Folder(string folderPath);
    int inotifyEvent(string userFolder);
    int createFolder(string userId);
    string getFolderPath(); // it takes note of changes in the folder
};
