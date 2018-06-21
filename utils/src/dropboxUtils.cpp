#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <iostream>
#include <string>
#include <stdlib.h>
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

pid_t getIdOfProcess() {
  return getpid();
}


string getipAddress() {
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    family = ifa->ifa_addr->sa_family;

    if (family == AF_INET) {
      s = getnameinfo(
       ifa->ifa_addr, sizeof(struct sockaddr_in),
       host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST
      );
      if (s != 0) {
        printf("getnameinfo() failed: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
      }
      if (strcmp(ifa->ifa_name, "lo") != 0) {
        return host;
      }
    }
  }
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
