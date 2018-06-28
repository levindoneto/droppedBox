#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/process.hpp"

class ServerUser;

class ServerCommunication : public Thread {
  public:
    ServerCommunication(ServerUser* parent);
    ~ServerCommunication();
    void *run();
    bool usingActive;
    list<string> arqsSending;
    //map<string, ServerCommunication*> *syncThreads;
  private:
    void sync_client_files();
    void sync_file(string filename);
    void delete_file(string filename);
    void send_files_to_client();
    Process *process;
    ServerUser* parent;
    list<string> files_not_synced;
    list<string> files_to_delete;
};

#endif
