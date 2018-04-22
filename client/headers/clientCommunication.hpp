#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;
// ClientCommunication extends Communication
class ClientCommunication : public Communication {
  private:
    int port;
    int socketDescriptor;

  public:
    ClientCommunication();
    ClientCommunication(int port);
    ClientCommunication(char* ip, int port);
    ClientCommunication(char* ip, int port, ClientUser* user);
    bool loginServer(char* ip, int port, ClientUser* user);
    bool closeSession ();
};
