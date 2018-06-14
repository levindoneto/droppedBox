#ifndef MULTISERVER_HPP
#define MULTISERVER_HPP

#include "../../utils/headers/dropboxUtils.h"

#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

class MultiServer {
  public:
    MultiServer();
    ~MultiServer();
    list<int> servers; // pid_t

  private:
    int procElected;
};

#endif
