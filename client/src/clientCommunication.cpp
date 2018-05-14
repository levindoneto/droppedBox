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
#include "../../utils/headers/udpUtils.hpp"
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
  this->port =  PORT;
  this->ip = LOCALHOST;
}

// Create a communication in localhost:port with IPv4
ClientCommunication::ClientCommunication(int port) {
  this->port =  port;
  this->ip = LOCALHOST;
}

// Create a communication in ip:port with IPv4
ClientCommunication::ClientCommunication(char* ip, int port) {
  this->port = port;
  this->ip = ip;
}

/* Function that logs a user in and creates a socket. In addition, it returns
 * the socket descriptor for trading messages throughtout the application,
 * and for closingthe open socket afterward.
 */
int ClientCommunication::loginServer(char* ip, int port, ClientUser* user) {
  int socketDesc;
  int status;
  unsigned int lenSckAddr;
  /* According to the C standard, the address of a structure and its first
     member are the same, so you can cast the pointer to sockaddr_in(6) in a
     pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *host;
  string clientFolderPath;
  string serverFolderPath;
   UserInfo userInfo = {};

  // Thread for synchronization
  pthread_t syn_th;

  char buffer[BUFFER_SIZE];
  fflush(stdin);
  // Get host
  host = gethostbyname(ip);
  if (host == NULL) {
    throwError("The host does not exist");
  }

  socketDesc = openSocket();

  // Address' configurations
  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  // Folder management after the user gets logged in
  clientFolderPath = getpwuid(getuid())->pw_dir; // Get user's home folder
  clientFolderPath = clientFolderPath + "/sync_dir_" + user->getUserId();
  serverFolderPath = "db/clients/sync_dir_" + user->getUserId();
  Folder* folder = new Folder("");
  folder->createFolder(clientFolderPath);
  folder->createFolder(serverFolderPath);

  string message = "[Client Login]: User " + user->getUserId()
      + " has logged in via the socket " + to_string(socketDesc);
  message.copy(userInfo.message, message.length(), 0);
  string userId = user->getUserId();
  userId.copy(userInfo.userId, userId.length(), 0);
  writeToSocket(userInfo, socketDesc, ip, port);
  //getClientFolderPath(clientFolderPath); // TODO: change this in dropboxUtils

  // Create thread for monitoring synchronized user folder
  //pthread_create(&syn_th, NULL, inotifyEvent, NULL); // TODO: Fix inotify

  return socketDesc;
}

bool ClientCommunication::closeSession () {
  cout << "Bye user" << endl;
  // Close sync thread (pthread_t syn_th)

  return false; // For exiting the user interface
}
