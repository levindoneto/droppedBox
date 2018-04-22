#include <iostream>
#include "../headers/clientUser.hpp"

using namespace std;

ClientUser::ClientUser(string userId, Folder *userFolder) {
  this->userId = userId;
  this->isSync = false;
  this->userFolder = userFolder;
  this->device = NULL;
}


ClientUser::ClientUser(string userId, Device *device, Folder *userFolder) {
  this->userId = userId;
  this->isSync = false;
  this->userFolder = userFolder;
  this->device = device;
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

void ClientUser::sync() {
  cout << "Sync client " << this->userId << " for accessing";
  unique_lock<mutex> lck(this->accessSync);
  this->isSync = true;
}

bool ClientUser::isSynchronized() {
  unique_lock<mutex> lck(this->accessSync);
  return this->isSync;
}
