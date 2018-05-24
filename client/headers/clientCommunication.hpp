#ifndef ClientCommunication_H
#define ClientCommunication_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class ClientCommunication: public Thread {
private:
  Process* processComm;
public:
  ClientCommunication(Process* processComm);
  ~ClientCommunication();
  bool usingActive;
  void* run();
};

#endif
