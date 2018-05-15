#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <thread>

#include "../headers/clientUser.hpp"

using namespace std;

ClientUser::ClientUser(string userId, Folder *userFolder) {
  this->userId = userId;
  this->userFolder = userFolder;
}

void ClientUser::startThreads(){
  thread inotifyThread = thread(&ClientUser::inotifyEvent, this);
  thread syncDirThread = thread(&ClientUser::syncDirLoop, this);
  thread userLoop = thread(&ClientUser::userLoop, this);
  thread commandLoopThread = thread(&ClientUser::commandLoop, this);
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
