#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;

class ClientCommunication : public Communication {
  private:
    int port;
    int socketDescriptor;

  public:
    ClientCommunication();
    ClientCommunication(int port);
    ClientCommunication(char* ip, int port);
    bool connectServer(char* ip, int port, string userId);
};
