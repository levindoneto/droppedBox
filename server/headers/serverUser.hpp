#ifndef SERVERUSER_H
#define SERVERUSER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"
#include "../headers/serverCommunication.hpp"

class DropboxServer;

class ServerUser : public Thread {
  public:
    ServerUser(DropboxServer* server, Process *process);
    ~ServerUser();
    void *run();

    void mainloop();
    void receive_upload(string filename, Process* process = NULL);
    void send_download(string filename, Process* process = NULL);
    void list_server();
    void close_session();

    bool usingActive;
    string username;
    Process *process;
    DropboxServer* server;
    ServerCommunication* server_sync;

};

#endif
