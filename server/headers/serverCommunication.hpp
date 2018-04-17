#include <string>
#include "../../utils/headers/communication.hpp"

class ServerCommunication: public Communication {
  public:
    ServerCommunication(int socketDesc, int port);
    ServerCommunication(int port);
    ServerCommunication();
};
