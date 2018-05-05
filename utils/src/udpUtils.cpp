#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../headers/dropboxUtils.hpp"

using namespace std;

int writeToSocket(string request, int socketDesc, string host, int port) {
  int status;
  unsigned int lenSckAddr;
  const char *requestChar = request.c_str();
  const char *hostChar = host.c_str();
  int nBytes;
  struct sockaddr_in serverAddress;
  struct hostent *server;

  server = gethostbyname(hostChar);
  // Address' configurations
  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  status = sendto(
    socketDesc,
    requestChar,
    BUFFER_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );

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
