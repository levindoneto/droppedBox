#include <string>
#include <iostream>
#include <sys/types.h> // bool impl
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../headers/clientCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../settings/config.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

#define DEBUG 0
#define TRUE 1
#define BYTE_IN_BITS 8
#define BUFFER_SIZE 256

using namespace std;

/* start socket
   while true:
     sendto
     recvfrom
   close socket */

// Create connectionm in localhost:8080 with IPv4
ClientCommunication::ClientCommunication() {
  /* According to the C standard, the address of a structure and its first
     member are the same, so you can cast the pointer to sockaddr_in(6) in a
     pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/

  // TODO UDP
  #ifdef DEBUG
  cout << "<Client Communication>: Connection with the socket "
    << this->socketDescriptor << " has been established"<< endl;
  #endif
}

// Create connectionm in localhost:port with IPv4
ClientCommunication::ClientCommunication(int port) {
  this->port =  port;
  // TODO - assume localhost
}

// Create connectionm in ip:port with IPv4
ClientCommunication::ClientCommunication(char* ip, int port) {
  this->port = port;
  // TODO - assume localhost
}

bool ClientCommunication::connectServer(char* ip, int port, string userId) {
  int socketDesc;
  int status;
  unsigned int lenSckAddr;
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *server;

  char buffer[BUFFER_SIZE];
  fflush(stdin);
  // Get host
  server = gethostbyname(ip);
  if (server == NULL) {
    server = gethostbyname(LOCALHOST);
    if (server == NULL) {
      throwError("The host does not exist\n");
    }
  }

  // Open udp socket using the defaul protocol
  if ((socketDesc = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throwError("Error on opening socket\n");
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);
  
  Folder* folder = new Folder();
  folder->createFolder(userId);

  printf(">>> ");
  bzero(buffer, BUFFER_SIZE);
  fgets(buffer, BUFFER_SIZE, stdin);

  status = sendto(
    socketDesc,
    buffer,
    strlen(buffer),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message\n");
  }

  lenSckAddr = sizeof(struct sockaddr_in);
  status = recvfrom(
    socketDesc,
    buffer,
    BUFFER_SIZE,
    0,
    (struct sockaddr *) &from,
    &lenSckAddr
  );

  if (status < 0) {
    throwError("Error on receive from");
  }

  cout << "Got an ack: " << buffer;

  close(socketDesc);

  #ifdef DEBUG
  cout << "Connecting to the server with ip = " << ip
    << " and port = " << port << endl;
  #endif
  return true;
}
