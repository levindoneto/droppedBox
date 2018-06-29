#ifndef FILE_H
#define FILE_H

#include "../../headers/dropboxUtils.h"

class File {
  public:
    // Attributes
    string filepath;
    string modification_time_str;
    string access_time_str;
    string creation_time_str;
    // File constructor
    File() {};
    File(string filepath);
    // Methods
    string name();
    unsigned int size();
    unsigned int modification_time();
    string time_to_string(time_t timestamp);
    void set_modification_time(int modtime);
    string stringify();
    static string working_directory();
    static list<File> list_directory(string dirpath);
    static list<string> listNamesOfFiles(string dirpath);
    static string list_directory_str(string dirpath);
    void print_listOfFileInfo(string listOfFileInfo);
    string ll(string dirpath);
    static File parse_file(string file_str);
    static void createFolderForFiles(string path);
  private:
    struct stat info;
    bool update_info();
};

#endif
