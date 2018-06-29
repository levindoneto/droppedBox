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

int getIdOfProcess() {
  return (int) getpid();
}

// Return the ip of the computer where the process is running
string getOwnIpAddress() {
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

list<string> getListOfHostnames() {
  string hostname;
  list<string> hostnames;
  ifstream file("allservers.txt");
  getline(file, hostname); // the 1st line is the own hostname
  while (getline(file, hostname)) {
    hostnames.push_back(hostname);
  }
  return hostnames;
}

string getIpAddress() {
  string ip;
  ifstream file("allservers.txt");
  getline(file, ip);
  return ip;
}

void listClient(string userId) {
  listFiles(CLIENT_LIST, userId);
}

void printElement(string data, int width, char separator) {
  cout << left << setw(width) << setfill(separator) << data;
}

int terminal_width() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

void print_table_row(string row, char delimiter = '|') {
  int pos = INIT;
  int field_width = INIT;
  while (!row.empty()) {
    pos = row.find(delimiter);
    string data = row.substr(INIT, pos);
    if (!field_width) {
      int field_count = count(row.begin(), row.end(), delimiter);
      field_width = terminal_width() / field_count;
    }
    printElement(data, field_width, ' ');
    row.erase(INIT, pos + 1);
  }
}

void print_table(string table) {
  string row;
  istringstream string_stream(table);
  while (getline(string_stream, row)) {
    print_table_row(row);
    cout << endl;
  }
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

string time_to_string(time_t timestamp) {
  char buffer[256];
  struct tm *tm = localtime(&timestamp);
  strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m/%Y", tm);
  return string(buffer, strlen("HH:MM:SS DD/MM/YYYY"));
}
