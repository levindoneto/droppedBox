#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/process.hpp"

class DropboxClient {
  private:
    // Where the commands are in (link with the server via UDP functions)
    int run();
    bool verifyServerAlive();
    string userId;
    string folderOfTheUser;
    Process *processComm;
  public:
    void newProcessCommunication(string userId, string host, int port);
};

#endif
