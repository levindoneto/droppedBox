#include <iostream>
#include <string>
#include "string.h"
#include "../headers/dropboxClient.hpp"
#include "../headers/clientUser.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"

using namespace std;

int main (int argc, char **argv) {
  cout << "Client" << endl;
  string userId;
  string host;
  int port;

  userId = argv[USER_CLIENT] != NULL ? argv[USER_CLIENT] : USER_ADMIN;
  host = argv[HOST_CLIENT] != NULL ? argv[HOST_CLIENT] : LOCALHOST;
  port = argv[PORT_CLIENT] != NULL ? atoi(argv[PORT_CLIENT]) : PORT;

  // string -> char*
  char *hostConn = new char[host.size()+1];
  strcpy(hostConn, host.c_str());

  cout << "Establishing connection with the user " << userId
    << " at the port " << port;
  // Test client communication
  ClientCommunication* c = new ClientCommunication();
  c->connectServer(hostConn, port, userId);
  delete[] hostConn;
  return 0;
}
