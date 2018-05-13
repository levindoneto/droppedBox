#include <iostream>
#include <string>
#include <unistd.h>
#include "string.h"
#include "../headers/dropboxClient.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/process.hpp"
#include "../../utils/headers/ui.hpp"

using namespace std;

int main (int argc, char **argv) {
  string username = USER_ADMIN; // Default user
  string host = LOCALHOST; // Default host
  string command;
  string parameter;
  vector<string> commandToRun;
  bool resp = true;
  int port = PORT; // Default port
  int socket;

  if (
    argv[USER_CLIENT] != NULL &&
    argv[HOST_CLIENT] != NULL &&
    argv[PORT_CLIENT] != NULL
  ) {
      username = argv[USER_CLIENT];
      host = argv[HOST_CLIENT];
      port = atoi(argv[PORT_CLIENT]);
  } else {
    cout << "Usage:" << endl
      << "./dropboxClient <username> <host> <port>" << endl;
    throwError("[Client]: Invalid use of the application");
  }

  cout << endl << "**** The user " << username << " has successfully logged in ****" << endl;
  // string -> char*
  char *hostConn = new char[host.size()+1];
  strcpy(hostConn, host.c_str());

  Folder *userFolder = new Folder("../../db/" + username);
  ClientUser* user = new ClientUser(username, userFolder);
  ClientCommunication* c = new ClientCommunication();
  Process* proc = new Process();

  socket = c->loginServer(hostConn, port, user);
  //proc->setLoggedUser(user);

  showMenu();
  while(resp) {
    commandToRun = getUserCommand();
    command = commandToRun.front();
    parameter = commandToRun.back();
    resp = proc->managerCommands(command, parameter, user, port, hostConn, socket);
  }
  delete[] hostConn;

  exit(TRUE);

  return 0;
}
