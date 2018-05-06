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

ServerCommunication::ServerCommunication(int port) {
  int socketDesc, itr, status, lastChunck = 1;
  socklen_t clilen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  Datagram receiveChunck;
  char buffer[BUFFER_SIZE];
  fflush(stdin);
  char fname[20], ack[10];
  FILE *fp;
  unsigned int fileSize;
  //struct datagram receiveChunck;

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
  itr = 1;
  memset(buffer, 0, CHUNCK_SIZE);

  while(itr * CHUNCK_SIZE < fileSize) {
    status = recvfrom(
      socketDesc,
      &receiveChunck,
      sizeof(receiveChunck),
      0,
      (struct sockaddr *) &clientAddress,
      &clilen
    );
    if (status < 0) {
      throwError("Error on recvfrom");
    }

    //printf("%d\n", receiveChunck.chunckId);
    sprintf(ack, "%d", receiveChunck.chunckId);

    status = sendto(
      socketDesc,
      ack,
      sizeof(int),
      0,
      (struct sockaddr *) &clientAddress,
      sizeof(struct sockaddr)
    );

    if (status  < 0) {
      throwError("Error on sending receiveChunck to the created socket");
    }
    if(lastChunck != receiveChunck.chunckId) {
      fwrite(receiveChunck.chunck, CHUNCK_SIZE, 1, fp);
      memset(receiveChunck.chunck, 0, CHUNCK_SIZE);
      itr++;
    }
    lastChunck = receiveChunck.chunckId;
  }

  memset(receiveChunck.chunck, 0, (fileSize % CHUNCK_SIZE));
  status = recvfrom(
    socketDesc,
    &receiveChunck,
    sizeof(receiveChunck),
    0,
    (struct sockaddr *) &clientAddress,
    &clilen
  );
  if (status < 0) {
    throwError("Error on recvfrom");
  }

  fwrite(receiveChunck.chunck,(fileSize % CHUNCK_SIZE), 1, fp);
  memset(buffer, 0, CHUNCK_SIZE);
  fclose(fp);
  fflush(stdin);
  close(socketDesc);
}
