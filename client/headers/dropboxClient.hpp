#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/process.hpp"

class DropboxClient : public Thread {
  private:
    // Where the commands are in (link with the server via UDP functions)
    int mainloop();
    string userId;
  public:
    DropboxClient(string username, string hostname, int port);
    ~DropboxClient();
    void *run();
    int port;
    string folderOfTheUser;
    Process *process;
    string username;
    string hostname;
};

#endif
