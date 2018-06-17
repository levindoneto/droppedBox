#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class ServerCommunication : public Thread {
  public:
    ServerCommunication(
      Process *processComm,
      map<string, ServerCommunication*> *syncCommunicationThreads
    );
    ~ServerCommunication();
    void *run();
    bool usingActive;
    map<string, ServerCommunication*> *syncThreads;
    list<string> arqsSending;
  private:
    Process *processComm;
};

#endif
