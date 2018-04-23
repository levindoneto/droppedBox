#include <iostream>
#include <string>
#include <vector>

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

/* Available options:
 * - upload <path/filename.ext>
 * - download <filename.ext>
 * - list_server
 * - list_client
 * - get_sync directory
 * - exit
 */

 vector<string> parseUserCommand(const string& input, const string& separator) {
     vector<string> commandWithParameter;
     size_t previous = 0, position = 0;
     do {
         position = input.find(separator, previous);
         if (position == string::npos) position = input.length();
         string token = input.substr(previous, position-previous);
         if (!token.empty()) commandWithParameter.push_back(token);
         previous = position + separator.length();
     } while (position < input.length() && previous < input.length());
     return commandWithParameter;
 }

vector<string> ClientUser::getCommand() {
  vector<string> commandWithParameter;
  string wholeCommand;
  cin >> wholeCommand;
  commandWithParameter = parseUserCommand(wholeCommand, " ");
  cout << commandWithParameter.front() << '\n';
  cout << commandWithParameter.back() << '\n';
  return commandWithParameter;
}
