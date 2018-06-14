#include "../headers/multiserver.hpp"
#include "../../utils/headers/dropboxUtils.h"

// Start an election using the bully algorithm for all active processes
void startElectionProcesses() {
  printf("start election%s\n");
}

void setProcessElected (int procElected) {
  this->procElected = procElected;
}
