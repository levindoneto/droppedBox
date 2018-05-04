#include <string>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include "../headers/dropboxUtils.hpp"

using namespace std;



int writeToSocket(int socketDesc, char* buffer) {
  int status;
  int nBytes;
  bzero(buffer, sizeof(buffer));
  fgets(buffer, sizeof(buffer), stdin);
  nBytes = write(socketDesc, &buffer, sizeof(buffer));

  if (status < 0 || nBytes < 0) {
    throwError("[writeToSocket]: Error on sending message");
  }
  return SUCCESS;
}

int readFromSocket(int socketDesc, char* buffer) {
  int status;
  status = read(socketDesc, &buffer, sizeof(buffer));
  if (status < 0) {
    throwError("[readFromSocket]: Error on receiving message");
  }
  return SUCCESS;
}
