#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include "../headers/dropboxUtils.hpp"

using namespace std;
string clientFolderPath;

void throwError (char* errorMessage) {
  perror(errorMessage);
  exit(TRUE);
}

// TODO: Remake this function to make it OO

void getClientFolderPath(string folderPath) {
  clientFolderPath = folderPath;
}

unsigned int fileSize(string filePath) {
  const char *filePathChar = filePath.c_str();
  FILE * f = fopen(filePathChar, "r");
  fseek(f, 0, SEEK_END);
  unsigned long len = (unsigned long)ftell(f);
  fclose(f);
  return len;
}
