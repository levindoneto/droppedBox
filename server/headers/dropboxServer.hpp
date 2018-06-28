#ifndef DROPBOXSERVER_H
#define DROPBOXSERVER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../headers/serverUser.hpp"

class DropboxServer {
  public:
    // Attributes
    int port;
    Process* listener;
    list<Process*> backupServers;
    map<string, ServerUser*> threads;
    // Methods
    //DropboxServer(int port);
    void backupServerRun(string hostnamePrimary, int port);
    void primaryServerRun(int port);

  private:
    // Methods
    void rcvProcComm(UDPUtils listener);
    void closeThreadsOpen();

};

#endif
