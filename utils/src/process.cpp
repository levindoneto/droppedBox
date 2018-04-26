#include <iostream>
#include <string>

#include "../headers/process.hpp"
#include "../headers/ui.hpp"

using namespace std;

Process::~Process() {
  cout << "Destrying process..." << endl;
}

//TODO: REturn id of the command being executed
void Process::managerCommands(
  string command,
  string parameter,
  ClientUser* user
) {
  if (command.compare(UPLOAD) == 0) {
    upload(parameter, user);
  } else if (command.compare(DOWNLOAD) == 0) {
    download(parameter, user);
  } else if (command.compare(LIST_SERVER) == 0) {
    listServer(user);
  } else if (command.compare(LIST_CLIENT) == 0) {
    listClient(user);
  } else if (command.compare(GET_SYNC_DIR) == 0) {
    getSyncDir(user);
  } else if (command.compare(EXIT_APP) == 0) {
    exitApp(user);
  } else if (command.compare(HELP_C) == 0 || command.compare(HELP_L) == 0) {
    showHelp();
  }else {
    throwError("Invalid command");
  }
}

int Process::getProcessId() {
  return this->processId;
}

int Process::upload(string filePath, ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::download(string filePath, ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::listServer(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::listClient(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::getSyncDir(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::exitApp(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}
