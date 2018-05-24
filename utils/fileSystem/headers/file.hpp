#ifndef FILE_H
#define FILE_H

#include "../../headers/dropboxUtils.h"

class File {
  public:
    File() {};
    void createFolderForFiles(string path);
    static string ll(string dirpath);
    unsigned int getLenOfTheFile(string nameOfTheFile);
    static list<string> listNamesOfFiles(string dirpath);
};

#endif
