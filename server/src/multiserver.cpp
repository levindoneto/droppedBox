#include "../headers/multiserver.hpp"
#include "../../utils/headers/dropboxUtils.h"

// Dynamic initialization for mutex with specified attributes
pthread_mutex_t startElection_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize the structure with the process' information (ip, port, pid of the
// process running it). It's called in the constructor of this class.
void MultiServer::serverInfoInit() {
  string ipTest = "localhost";
  this->serverInfo.ip = ipTest;
  this->serverInfo.port = 3000;
  this->serverInfo.pid = (int) getpid();
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
