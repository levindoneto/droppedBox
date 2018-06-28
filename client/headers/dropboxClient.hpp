#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/process.hpp"

class DropboxClient {
  private:
    // Where the commands are in (link with the server via UDP functions)
    int run();
    string userId;
  public:
    ~DropboxClient();
    string folderOfTheUser;
    Process *process;
    void newProcessCommunication(string userId, string host, int port);
};

#endif
