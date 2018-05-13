#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;

class ServerCommunication: public Communication {
  private:
    string loggedUserId;
  public:
    ServerCommunication(int port);
    ServerCommunication();
    void serverComm(int port);
    void setLoggedUser(string loggedUserId);
    string getLoggedUser();
};
