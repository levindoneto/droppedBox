#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

using namespace std;

#define TRUE 1
#define BUFFER_SIZE 256

ServerCommunication::ServerCommunication(int port) {
  int socketDesc, itr, status;
  socklen_t clilen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  char buffer[BUFFER_SIZE];
  fflush(stdin);
  char fname[20];
  FILE *fp;
  unsigned int fileSize;

  // Open socket
	if ((socketDesc = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throwError("Error on opening socket");
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  if (
    bind(
      socketDesc,
      (struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr)
    ) < 0
  ) {
    throwError("Error on on binding");
  }

  clilen = sizeof(struct sockaddr_in);

  Folder* folder = new Folder("");
  folder->createFolder("db/");
  folder->createFolder("db/clients");

  while (TRUE) {
    /* Receive from socket */
		status = recvfrom(
      socketDesc,
      buffer,
      BUFFER_SIZE,
      0,
      (struct sockaddr *) &clientAddress,
      &clilen
    );
		if (status < 0) {
      throwError("Error on recvfrom");
    }

    strcpy(fname, buffer);

    status = recvfrom(
      socketDesc,
      buffer,
      BUFFER_SIZE,
      0,
      (struct sockaddr *) &clientAddress,
      &clilen
    );
		if (status < 0) {
      throwError("Error on recvfrom");
    }

    fileSize = atoi(buffer);

    fp = fopen(fname, "wb");
    itr=1;

    memset(buffer, 0, BUFFER_SIZE);
    while(itr * BUFFER_SIZE < fileSize)
    {
      status = recvfrom(
        socketDesc,
        buffer,
        BUFFER_SIZE,
        0,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
      if (status < 0) {
        throwError("Error on recvfrom");
      }
        fwrite(buffer, BUFFER_SIZE, 1, fp);
        memset(buffer, 0, BUFFER_SIZE);
        itr++;
    }

    status = recvfrom(
      socketDesc,
      buffer,
      (fileSize % BUFFER_SIZE),
      0,
      (struct sockaddr *) &clientAddress,
      &clilen
    );
    if (status < 0) {
      throwError("Error on recvfrom");
    }

    fwrite(buffer,(fileSize % BUFFER_SIZE), 1, fp);
    memset(buffer, 0, BUFFER_SIZE);
    fclose(fp);

    // Send datagram to the created socket
    status = sendto(
      socketDesc,
      "Got your datagram\n",
      17,
      0,
      (struct sockaddr *) &clientAddress,
      sizeof(struct sockaddr)
    );

    if (status  < 0) {
      throwError("Error on sending datagram to the created socket");
    }
  }
  fflush(stdin);
  close(socketDesc);
}
