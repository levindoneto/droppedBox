#include <string>

using namespace std;

class ServerCommunication{
  private:
    string loggedUserId;
  public:
    ServerCommunication(int port);
    ServerCommunication();
    void serverComm(int port);
    void setLoggedUser(string loggedUserId);
    string getLoggedUser();
};
