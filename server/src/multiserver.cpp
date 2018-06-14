#include "../headers/multiserver.hpp"
#include "../../utils/headers/dropboxUtils.h"

// Dynamic initialization for mutex with specified attributes
pthread_mutex_t startElection_mutex = PTHREAD_MUTEX_INITIALIZER;

// Start an election using the bully algorithm for all active processes
void MultiServer::startElectionProcesses() {
  pthread_mutex_lock(&startElection_mutex);
  printf("in - Start election%s\n");
  pthread_mutex_unlock(&startElection_mutex);
  printf("out%s\n");
}

void MultiServer::setProcessElected (int procElected) {
  this->procElected = procElected;
}

// Send down the elected info to all the other server (processes)
void MultiServer::broadcastElected () {
  printf("TODO\n");
}
