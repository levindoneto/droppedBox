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
#include <pwd.h>
#include <pthread.h>
#include "../headers/clientCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../settings/config.hpp"

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

ClientCommunication::ClientCommunication() {
  #ifdef DEBUG
  cout << endl << "<Client Communication>: Connection with the socket "
    << this->socketDescriptor << " has been established" << endl;
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

bool ClientCommunication::loginServer(char* ip, int port, ClientUser* user) {
  string clientFolderPath, serverFolderPath;
  pthread_t syn_th;

  clientFolderPath = getpwuid(getuid())->pw_dir;
  clientFolderPath = clientFolderPath + "/sync_dir_" + user->getUserId();
  serverFolderPath = "db/clients/sync_dir_" + user->getUserId();
  Folder* folder = new Folder("");
  folder->createFolder(clientFolderPath);
  folder->createFolder(serverFolderPath);

  getClientFolderPath(clientFolderPath);
  pthread_create(&syn_th, NULL, inotifyEvent, NULL);

/*  printf(">>> ");
  bzero(buffer, BUFFER_SIZE);
  fgets(buffer, BUFFER_SIZE, stdin);

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
    throwError("Error on receive ack");
  }

  cout << "Got an ack: " << buffer << endl;

  close(socketDesc);*/

  return true;
}

bool ClientCommunication::closeSession () {
  cout << "Bye user" << endl;
  // Close sync thread

  return false; // For exiting the user interface
}
