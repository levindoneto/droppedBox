#ifndef MULTISERVER_HPP
#define MULTISERVER_HPP

#include <list>
#include <iostream>
#include <string>
#include <mutex>

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

#include "../../utils/headers/dropboxUtils.h"

#include "../../utils/headers/parUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"

using namespace std;

typedef struct serverInfo {
  string ip;
  int port;
  int pid;
} SERVER_INFO;

class MultiServer {
  public:
    MultiServer() {};
    ~MultiServer();
    void serverInfoInit();
    SERVER_INFO getServerInfo();
    void startElectionProcesses();
    void setProcessElected(int procElected);
    void broadcastElected();
    list<int> servers;

  private:
    int procElected;
    SERVER_INFO serverInfo;
    vector<SERVER_INFO> listActiveProcesses;
    vector<SERVER_INFO> inElectionProcesses;
};

#endif
