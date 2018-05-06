#include <iostream>
#include <string>

#include "../headers/process.hpp"
#include "../headers/ui.hpp"
#include "../headers/udpUtils.hpp"
#include <unistd.h>
using namespace std;

Process::~Process() {
  cout << "Destroying process..." << endl;
}

bool Process::managerCommands(
  string command,
  string parameter,
  ClientUser* user,
  int port,
  string host,
  int socketDesc
) {
  int resp;
  if (command.compare(UPLOAD) == 0) {
    resp = upload(parameter, user);
  } else if (command.compare(DOWNLOAD) == 0) {
      resp = download(parameter, user);
  } else if (command.compare(LIST_SERVER) == 0) {
      resp = listServer(user, port, host, socketDesc);
  } else if (command.compare(LIST_CLIENT) == 0) {
      resp = listClient(user, port, host, socketDesc);
  } else if (command.compare(GET_SYNC_DIR) == 0) {
      resp = getSyncDir(user);
  } else if (command.compare(EXIT_APP) == 0) {
      resp = exitApp(user);
      if (resp == EXIT_OPT_YES) {
        string clientRequest = "[Client Request]: The user " + user->getUserId()
          + " has logged off from the DroppedBox";
        writeToSocket(clientRequest, socketDesc, host, port);
        closeSocket(socketDesc);
        return EXIT;
      } else if (resp == EXIT_OPT_WRONG) {
          exitApp(user);
      } else {
        return !EXIT;
      }
  } else if (command.compare(HELP_C) == 0 || command.compare(HELP_L) == 0) {
    showHelp();
  }else {
    throwError("[managerCommands]: Invalid command");
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

int Process::listServer(ClientUser* user, int port, string host, int socketDesc) {
  string clientRequest = "[Client Request]: List all the files on the server side for the user "
    + user->getUserId() + " via the socket " + to_string(socketDesc);
  writeToSocket(clientRequest, socketDesc, host, port);
  Folder* procFolder = new Folder();
  procFolder->listFiles(SERVER_LIST, user->getUserId());
}

int Process::listClient(ClientUser* user, int port, string host, int socketDesc) {
  string clientRequest = "[Client Request]: List all the files on the client side for the user "
     + user->getUserId() + " via the socket " + to_string(socketDesc);
  writeToSocket(clientRequest, socketDesc, host, port);
  Folder* procFolder = new Folder();
  procFolder->listFiles(CLIENT_LIST, user->getUserId());
}

int Process::getSyncDir(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::exitApp(ClientUser* user) {
  string userAnswer;
  cout << "$ Do you really want to log off, " << user->getUserId()
    << "? [yes or no]" << endl;
    cout << "$ ";
  cin >> userAnswer;

  if (userAnswer.compare("yes") == 0 || userAnswer.compare("YES") == 0) {
    cout << "$ Have a good one and take care, " << user->getUserId() << " !" << endl;
    return EXIT_OPT_YES;
  } else if (userAnswer.compare("no") == 0 || userAnswer.compare("NO") == 0) {
      cout << "$ You can stay logged in then!" << endl;
      return EXIT_OPT_NO;
  } else {
      cout << INVALID_OPTION << endl;
      return EXIT_OPT_WRONG;
  }
}
