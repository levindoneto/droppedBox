#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <thread>

#include "../headers/clientUser.hpp"

using namespace std;

void ClientUser::startThreads(){
  thread inotifyThread = thread(&ClientUser::inotifyEvent, this);
  thread syncDirThread = thread(&ClientUser::syncDirLoop, this);
  thread commandLoopThread = thread(&ClientUser::commandLoop, this);
  thread userLoop = thread(&ClientUser::userLoop, this);
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
  while(TRUE);
}

void ClientUser::inotifyEvent() {
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
    i = 0;
    length = read(init, buffer, EVENT_BUF_LEN);

    while (i < length) {
      struct inotify_event *event = (struct inotify_event *) &buffer[i];
      if (event->len) {
        if (event->mask & IN_CREATE) {
            cout << "Arquivo criado" << '\n';
        }
        if (IN_MODIFY) {

        }
        if (event->mask & IN_DELETE) {

        }
        if (IN_MOVED_FROM) {

        }
        if (IN_MOVED_TO) {

        }
        if (IN_OPEN || IN_ACCESS) {

        }

        i += EVENT_SIZE + event->len;
      }
    }
  }
}


ClientUser::ClientUser(string userId, Folder *userFolder) {
  this->userId = userId;
  this->isSync = false;
  this->userFolder = userFolder;
  this->device = NULL;
  this->numberOfFiles = 0;
}

ClientUser::ClientUser(string userId, Device *device, Folder *userFolder) {
  this->userId = userId;
  this->isSync = false;
  this->userFolder = userFolder;
  this->device = device;
  this->numberOfFiles = 0;
}

ClientUser::ClientUser(string userId, Device *device, Folder *userFolder, int numberOfFiles) {
  this->userId = userId;
  this->isSync = false;
  this->userFolder = userFolder;
  this->device = device;
  this->numberOfFiles = numberOfFiles;
}

string ClientUser::getUserId() {
  return this->userId;
}

Folder* ClientUser::getUserFolder() {
  return this->userFolder;
}

string ClientUser::getUserConnectedDevicesToString() {
  return "TODO: parse device list";
}

int ClientUser::getNumberOfFiles() {
  return this->numberOfFiles;
}

void ClientUser::setUserFolder(Folder* userFolder) {
  this->userFolder = userFolder;
}

void ClientUser::sync() {
  cout << "Sync client " << this->userId << " for accessing";
  unique_lock<mutex> lck(this->accessSync);
  this->isSync = true;
}

bool ClientUser::isSynchronized() {
  unique_lock<mutex> lck(this->accessSync);
  return this->isSync;
}
