#ifndef ClientCommunication_H
#define ClientCommunication_H

#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/process.hpp"
#include "dropboxClient.hpp"

class ClientCommunication: public Thread {
private:
  // Attributes
  Process* process;
  DropboxClient *client;
  list<string> filenames;
  

public:
  ClientCommunication(DropboxClient *client);
  ~ClientCommunication();
  bool usingActive;
  void* run();
};

#endif
