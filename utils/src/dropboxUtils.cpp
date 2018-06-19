#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <iomanip> // setfill, setw
#include "../headers/dropboxUtils.h"
#include "../fileSystem/headers/folder.hpp"

using namespace std;

list<string> filesMerging;
string clientFolderPath;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;

void throwError (char* errorData) {
  perror(errorData);
  exit(TRUE);
}

unsigned int fileSize(string pathOfTheFile) {
  const char *filePathChar = pathOfTheFile.c_str();
  FILE * f = fopen(filePathChar, "r");
  fseek(f, INIT, SEEK_END);
  unsigned long len = (unsigned long)ftell(f);
  fclose(f);
  return len;
}

bool allowSending (string idOfTheFile) {
  pthread_mutex_lock(&sync_mutex);
  bool can_transfer = true;
  for (string &fname : filesMerging) {
    if (idOfTheFile == fname) {
      can_transfer = false;
      break;
    }
  }
  if (can_transfer) {
    filesMerging.push_back(idOfTheFile);
  }
  pthread_mutex_unlock(&sync_mutex);
  return can_transfer;
}

void unlock_file (string idOfTheFile) {
  pthread_mutex_lock(&sync_mutex);
  filesMerging.remove(idOfTheFile);
  pthread_mutex_unlock(&sync_mutex);
}

unsigned int obtainTSofFile(string pathOfFileForTS) {
  struct stat result;
  if (stat(pathOfFileForTS.c_str(), &result) == EQUAL) {
    return result.st_mtime;
  } else {
      return ERROR;
  }
}

string obatingNameOfTheFile(string pathOfTheFile) {
  return pathOfTheFile.substr(pathOfTheFile.find_last_of(SLASH) + 1);
}

string obtaingJustNameOfTheFile(string nameOfTheFile) {
  return nameOfTheFile.substr(INIT, nameOfTheFile.find_last_of('.'));
}

void listClient(string userId) {
  listFiles(CLIENT_LIST, userId);
}

void printElement(string data, int width, char separator) {
  cout << left << setw(width) << setfill(separator) << data;
}

void formatListOfArqs(string filelist) {
  char separator = FORMAT_NAME_FILE_S;
  int fieldWidth = SIZE_FILENAME_LIST;
  string delimiter = SEP_LIST_NO_SPACES;
  string tworkedProperlen;
  int pos = INIT;
  while ((pos = filelist.find(delimiter)) != std::string::npos) {
    tworkedProperlen = filelist.substr(INIT, pos);
    printElement(tworkedProperlen, fieldWidth, separator);
    filelist.erase(INIT, pos + delimiter.length());
  }
  cout << endl;
}
