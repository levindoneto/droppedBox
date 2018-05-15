#pragma once
#include <string>
#include "../../utils/headers/communication.hpp"
#include "clientUser.hpp"

using namespace std;
// ClientCommunication extends Communication
class ClientCommunication : public Communication {
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
