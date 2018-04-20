#include <iostream>
#include "../headers/dropboxServer.hpp"
#include "../headers/serverCommunication.hpp"
#include "../../settings/config.hpp"
#include "../../utils/headers/dropboxUtils.hpp"

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
  cout << "******* Server is running *******" << endl << endl;
  int port;

  if (argv[PORT_SERVER] != NULL) {
      port = atoi(argv[PORT_SERVER]);
  } else {
    cout << "Usage:" << endl
      << "./dropboxServer <port>" << endl;
    throwError("[Server]: Invalid use of the application");
  }

  ServerCommunication* s = new ServerCommunication(port);
  return 0;
}
