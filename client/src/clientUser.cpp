#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <vector>
#include <string>

#include "../headers/clientUser.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
using namespace std;

ClientUser::ClientUser(string userId, Folder *userFolder, char* ip, int port) {
  this->userId = userId;
  this->userFolder = userFolder;
  this->ip = ip;
  this->port = port;
  this->socketDescriptor = this->loginServer();
}

void ClientUser::startThreads(){
  thread inotifyThread = thread(&ClientUser::inotifyEvent, this);
  thread syncDirThread = thread(&ClientUser::syncDirLoop, this);
  thread userLoop = thread(&ClientUser::userLoop, this);
  thread commandLoopThread = thread(&ClientUser::commandLoop, this);
  inotifyThread.join();
  userLoop.join();
}

void ClientUser::syncDirLoop() {
  usleep(10000000); //10 seconds
  cout << "ITS ALIVE! sync" << endl;
  while(TRUE);
}

void ClientUser::commandLoop() {
  cout << "ITS ALIVE! command" << endl;
  while(TRUE);
}

void ClientUser::userLoop() {
  cout << "ITS ALIVE! user" << endl;
  int resp;
  string command;
  vector<string> commandToRun;
  showMenu();
  while(resp != EXIT) {
    //commandToRun = getUserCommand();
    command = commandToRun.front();
    //parameter = commandToRun.back();
    cout << command << endl;
  };
}

void ClientUser::inotifyEvent() {
  cout << "inotify" << endl;
  int init;
  int i;
  int watchedFolder;
  int length;
  char buffer[EVENT_BUF_LEN];

  string folderStr = this->userFolder->getHome();
  folderStr += "/sync_dir_" + this->userId;
  cout << folderStr << endl;

  char folder[CHUNCK_SIZE];
  folderStr.copy(folder, folderStr.length(), 0);

  init = inotify_init();
  if (init == ERROR) {
    throwError("Could not initialize inotify");
  }

  watchedFolder = inotify_add_watch(init, folder, INOTIFY_EVENTS);

  if (watchedFolder == ERROR) {
    throwError("It could not watch that folder");
  }

  while(TRUE) {
    cout << "inotifyTRUE" << endl;
    i = 0;
    length = read(init, buffer, EVENT_BUF_LEN);

    while (i < length) {
      struct inotify_event *event = (struct inotify_event *) &buffer[i];
      if (event->len) {
        if (event->mask & IN_CREATE) {
            cout << "Arquivo criado" << '\n';
        }
        else if (event->mask & IN_MODIFY) {
          cout << "modifcs" << endl;
        }
        else if (event->mask & IN_DELETE) {
          cout << "Delete" << endl;
        }
        else if (event->mask & IN_MOVED_FROM) {
          cout << "mvd from" << endl;
        }
        else if (event->mask & IN_MOVED_TO) {
          std::cout << "movd t" << '\n';
        }
        else if (event->mask & IN_OPEN || event->mask & IN_ACCESS) {
          cout << "open" << endl;
        }

        i += EVENT_SIZE + event->len;

      }
    }
  }
}

vector<string> ClientUser::getCommandFromQueue(){
  this->commandMutex.lock();
  vector<string> c = this->commandQueue.front();
  this->commandQueue.pop();
  this->commandMutex.unlock();
  return c;
}

void ClientUser::addCommandToQueue(vector<string> command){
  this->commandMutex.lock();
  this->commandQueue.push(command);
  this->commandMutex.unlock();
}

string ClientUser::getUserId() {
  return this->userId;
}

Folder* ClientUser::getUserFolder() {
  return this->userFolder;
}
void ClientUser::setUserFolder(Folder* userFolder) {
  this->userFolder = userFolder;
}

int ClientUser::loginServer() {
  char* ip = this->ip;
  int port = this->port;
  ClientUser* user = this;
  int socketDesc;
  int status;
  unsigned int lenSckAddr;
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *host;
  string clientFolderPath;
  string serverFolderPath;
  UserInfo userInfo = {};

  char buffer[BUFFER_SIZE];
  fflush(stdin);
  // Get host
  host = gethostbyname(ip);
  if (host == NULL) {
    throwError("The host does not exist");
  }

  socketDesc = openSocket();

  // Address' configurations
  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  // Folder management after the user gets logged in
  clientFolderPath = getpwuid(getuid())->pw_dir; // Get user's home folder
  clientFolderPath = clientFolderPath + "/sync_dir_" + user->getUserId();
  serverFolderPath = "db/clients/sync_dir_" + user->getUserId();
  Folder* folder = new Folder("");
  folder->createFolder(clientFolderPath);
  folder->createFolder(serverFolderPath);

  string message = "[Client Login]: User " + user->getUserId()
      + " has logged in via the socket " + to_string(socketDesc);
  message.copy(userInfo.message, message.length(), 0);
  string userId = user->getUserId();
  userId.copy(userInfo.userId, userId.length(), 0);
  writeToSocket(userInfo, socketDesc, ip, port);

  return socketDesc;
}
