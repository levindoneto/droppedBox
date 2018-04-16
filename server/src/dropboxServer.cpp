#include <iostream>
#include "../headers/dropboxServer.hpp"
#include "../headers/serverCommunication.hpp"
#include "../../settings/config.hpp"

using namespace std;

/*
  start socket
  bind
  while true:
    sendto
    recvfrom
  close socket
*/

int main (int argc, char* argv[]) {
    cout << "Server" << endl;
    int port;
    port = argv[PORT_SERVER] != NULL ? atoi(argv[1]) : PORT;
    ServerCommunication* s = new ServerCommunication(port);
    return 0;
}
