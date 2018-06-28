#ifndef DROPBOXSERVER_H
#define DROPBOXSERVER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../headers/serverUser.hpp"

class DropboxServer {
  private:
    void rcvProcComm(UDPUtils listener);
    void closeThreadsOpen();
  public:
    int port;
    Process* listener;
    UDPUtils* sock;
    static map<string, UDPUtils> backupServers;
    static list<string> clientIps;
    map<string, ServerUser*> threads;
    DropboxServer(int port);
    void start();
    void backup(string hostname);
};

#endif
