#include <iostream>
#include <string>
#include <vector>

#include "../headers/clientUser.hpp"
#include "../../utils/headers/ui.hpp"

using namespace std;

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
