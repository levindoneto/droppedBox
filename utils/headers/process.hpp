#include <string>
#include "../../client/headers/clientUser.hpp"
#include "../headers/dropboxUtils.hpp"

using namespace std;

class Process {
  private:
    int processId;
  public:
    Process() {};
    ~Process();
    void managerCommands(
      string command,
      string parameter,
      ClientUser* user
    );
    int getProcessId();

    // Functions which the user has access
    int upload(string filePath, ClientUser* user);
    int download(string filePath, ClientUser* user);
    int listServer(ClientUser* user);
    int listClient(ClientUser* user);
    int getSyncDir(ClientUser* user);
    int exitApp(ClientUser* user);
};
