#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H

#include "../../utils/headers/dropboxUtils.h"
#include "../headers/serverUser.hpp"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class ServerCommunication : public Thread {
  public:
    ServerCommunication(
      Process *processComm,
      map<string, ServerUser*> syncUserThreads
    );
    ~ServerCommunication();
    void *run();
    bool usingActive;
    map<string, ServerUser*> syncUserThreads;
    list<string> arqsSending;
  private:
    Process *processComm;
};

#endif
