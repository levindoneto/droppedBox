#ifndef FILE_H
#define FILE_H

#include "../../headers/dropboxUtils.h"

class File {
  public:
    File() {};
    File(string filepath);
    void createFolderForFiles(string path);
    static string ll(string dirpath);
    unsigned int getLenOfTheFile(string nameOfTheFile);
    static list<string> listNamesOfFiles(string dirpath);
    void set_modification_time(int modtime);
    string filepath;
    string modification_time_str;
    string access_time_str;
    string creation_time_str;
  private:
    string time_to_string(time_t timestamp);
    bool update_info();
    struct stat info;
};

#endif
