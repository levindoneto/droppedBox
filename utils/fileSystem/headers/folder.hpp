#pragma once

#include <string>
#include <vector>

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
    int createFolder(string userId);
    string getFolderPath(); // it takes note of changes in the folder
    vector<string> getTimes(string filePath);
    string timesToString(vector<string> times, int mode);
    void changeDirectory(string folderPath);
    void listFolder(string folderPath);
    string listFolderToString(string folderPath);
    string listFiles(int mode, string userId);
};
