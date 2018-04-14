#include <iostream>
#include "../headers/dropboxServer.hpp"
#include "../headers/serverCommunication.hpp"

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
    int port = atoi(argv[1]);
    ServerCommunication* s = new ServerCommunication(port);
    return 0;
}
