#include "../headers/multiserver.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"
#include <thread>

// Dynamic initialization for mutex with specified attributes
pthread_mutex_t startElection_mutex = PTHREAD_MUTEX_INITIALIZER;

MultiServer::MultiServer(int port, pid_t pid, string ipAddress) {
  serverInfoInit(port, pid, ipAddress);
}

void MultiServer::startThreads() {
  thread threadSendAlive = thread(&MultiServer::sendAliveMessage, this);
  threadSendAlive.join();
}

// Initialize the structure with the process' information (ip, port, pid of the
// process running it). It's called in the constructor of this class.
void MultiServer::serverInfoInit(int port, pid_t pid, string ipAddress) {
  this->serverInfo.ip = ipAddress;
  this->serverInfo.port = 3000;
  this->serverInfo.pid = pid;
}

// Return the information about the process running the server
SERVER_INFO MultiServer::getServerInfo() {
  return this->serverInfo;
}

// Start an election using the bully algorithm for all active processes
void MultiServer::startElectionProcesses() {
  pthread_mutex_lock(&startElection_mutex);
  printf("Start election\n");
  this->inElectionProcesses.push_back(this->serverInfo);
  for (int i = 0; i < this->listActiveProcesses.size(); i++) {
    if (this->serverInfo.pid < this->listActiveProcesses.at(i).pid) {
      this->inElectionProcesses.push_back(this->listActiveProcesses.at(i));
    }
  }
  printf("End election\n");
  pthread_mutex_unlock(&startElection_mutex);
}

// TODO: Change type, call broadcast afterward
void MultiServer::setProcessElected (int procElected) {
  this->procElected = procElected;
}

// Send down the elected info to all the other server (processes)
void MultiServer::broadcastElected () {
  printf("TODO\n");
}

// Send ALIVE(0) message to all backup servers every 3 seconds in order to keep
// them aware about the execution of the primary server
void MultiServer::sendAliveMessage() {
  while(true) {
    printf("alive\n");
    usleep(3000000); // 3 seconds
  }
}
