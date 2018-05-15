#pragma once
#include <string>
#include "../../client/headers/clientUser.hpp"
#include "../headers/dropboxUtils.hpp"

using namespace std;

class Process {
  private:
    int processId;
    string loggedUserId;
  public:
    Process() {};
    ~Process();
    int managerCommands(
      string command,
      string parameter,
      ClientUser* user,
      int port,
      string host,
      int socketDesc
    );
    int getProcessId();

    // Functions which the user has access
    int upload(string filePath, ClientUser* user, int port, string host, int socketDesc);
    int download(string fileName, ClientUser* user, string host, int port, int socketDesc);
    int listServer(ClientUser* user, int port, string host, int socketDesc);
    int listClient(ClientUser* user, int port, string host, int socketDesc);
    int getSyncDir(ClientUser* user, int port, string host, int socketDesc);
    int exitApp(ClientUser* user, int port, string host, int socketDesc);
    int deleteFile(string fileName, ClientUser* user, int port, string host, int socketDesc);
};
