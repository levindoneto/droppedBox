#ifndef SERVERUSER_H
#define SERVERUSER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class ServerUser : public Thread {
  private:
    Process *processComm;
  public:
    bool usingActive;
    ServerUser(Process *processComm);
    ~ServerUser(); // DESTROY USER COMM OBJ
    void *run();
};

#endif
