#ifndef SERVERUSER_H
#define SERVERUSER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"
#include "../headers/serverCommunication.hpp"

class ServerUser : public Thread {
  private:
    Process *processComm;
  public:
    ServerUser(
      Process *processComm,
      map<string, ServerUser*> *syncUserThreads,
      map<string, ServerCommunication*> *syncCommunicationThreads
    );
    ~ServerUser();
    void *run();
    bool usingActive;
    map<string, ServerUser*> *threads;
    map<string, ServerCommunication*> *syncThreads;
};

#endif
