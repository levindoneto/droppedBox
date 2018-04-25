#include <iostream>
#include <string>

#include "../headers/process.hpp"

using namespace std;

Process::~Process() {
  cout << "Destrying process...";
}

//TODO: REturn id of the command being executed
void Process::managerCommands(
  string command,
  string parameter,
  ClientUser* user
) {
  if (command.compare(UPLOAD) == 0) {
    cout << "TODO: UPLOAD" << endl;
  } else if (command.compare(DOWNLOAD) == 0) {
    cout << "TODO: DOWNLOAD" << endl;
  } else if (command.compare(LIST_SERVER) == 0) {
    cout << "TODO: LIST_SERVER" << endl;
  } else if (command.compare(LIST_CLIENT) == 0) {
    cout << "TODO: LIST_CLIENT" << endl;
  } else if (command.compare(GET_SYNC_DIR) == 0) {
    cout << "TODO: GET_SYNC_DIR" << endl;
  } else if (command.compare(EXIT_COMMAND) == 0) {
    cout << "TODO: EXIT" << endl;
  } else {
    throwError("Invalid command");
  }
}

int Process::getProcessId() {
  return this->processId;
}
