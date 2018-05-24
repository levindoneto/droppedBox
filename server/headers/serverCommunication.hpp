#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H

#include "../../utils/headers/dropboxUtils.h"

#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class ServerCommunication : public Thread
{
  public:
    ServerCommunication(Process *processComm);
    ~ServerCommunication();
    void *run();

    bool usingActive;
    list<string> arqsSending;

  private:
    Process *processComm;
};

#endif
