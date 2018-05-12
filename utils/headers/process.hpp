#include <string>
#include "../../client/headers/clientUser.hpp"
#include "../headers/dropboxUtils.hpp"

using namespace std;

class Process {
  private:
    int processId;
    ClientUser* user;
  public:
    Process() {};
    ~Process();
    bool managerCommands(
      string command,
      string parameter,
      ClientUser* user,
      int port,
      string host,
      int socketDesc
    );
    int getProcessId();

    // Functions which the user has access
    int upload(string filePath, ClientUser* user, int port, string host);
    int download(string filePath, ClientUser* user);
    int listServer();
    int listClient(ClientUser* user, int port, string host, int socketDesc);
    int getSyncDir(ClientUser* user);
    int exitApp(ClientUser* user);
    void setLoggedUser(ClientUser* user);
};
