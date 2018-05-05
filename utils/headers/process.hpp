#include <string>
#include "../../client/headers/clientUser.hpp"
#include "../headers/dropboxUtils.hpp"

using namespace std;
#define BUFFER_SIZE sizeof(datagram)
#define CHUNCK_SIZE 1024

struct Datagrams {
   int  chunckId;
   char  chunck[CHUNCK_SIZE];
} datagram;

class Process {
  private:
    int processId;
  public:
    Process() {};
    ~Process();
    bool managerCommands(
      string command,
      string parameter,
      ClientUser* user,
      int port,
      char* host
    );
    int getProcessId();

    // Functions which the user has access
    int upload(string filePath, ClientUser* user, int port, char* host);
    int download(string filePath, ClientUser* user);
    int listServer(ClientUser* user);
    int listClient(ClientUser* user);
    int getSyncDir(ClientUser* user);
    int exitApp(ClientUser* user);
};
