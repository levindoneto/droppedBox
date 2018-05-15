#pragma once
#include <string>
#include "clientUser.hpp"

using namespace std;

class ClientCommunication{
  private:
    int port;
    int socketDescriptor;
    char* ip;

  public:
    ClientCommunication();
    ClientCommunication(int port);
    ClientCommunication(char* ip, int port);
    int loginServer(char* ip, int port, ClientUser* user);
    bool closeSession ();
};
