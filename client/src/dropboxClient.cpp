#include <iostream>
#include <string>
#include "string.h"
#include "../headers/dropboxClient.hpp"
//#include "../headers/clientUser.hpp"

#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"
#include "../../utils/headers/dropboxUtils.hpp"

using namespace std;

int main (int argc, char **argv) {
  cout << "******* Client is running *******" << endl << endl;
  string username = USER_ADMIN; // Default user
  string host = LOCALHOST; // Default host
  int port = PORT; // Default port

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

  // string -> char*
  char *hostConn = new char[host.size()+1];
  strcpy(hostConn, host.c_str());

  cout << "Establishing connection with the user " << username
    << " at the port " << port << " on the host " << host << endl;

  // TODO: Use createUserFolder for this purpose
  Folder *userFolder = new Folder("../../db/" + username);
  ClientUser* user = new ClientUser(username, userFolder);
  ClientCommunication* c = new ClientCommunication();

  c->loginServer(hostConn, port, user);

  while(!EXIT) {
    // Show dropbox interface
  }

  delete[] hostConn;
  return 0;
}
