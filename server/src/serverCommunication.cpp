#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/process.hpp"
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

ServerCommunication::ServerCommunication() {
  setLoggedUser("");
}

void ServerCommunication::serverComm(int port) {
  int socketDesc, itr, status, lastChunck = 0;
  socklen_t clilen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  Datagram receiveChunck;
  char buffer[CHUNCK_SIZE];
  fflush(stdin);
  char fname[20], ack[10];
  FILE *fp;
  unsigned int fileSize;

  // Open socket
	socketDesc = openSocket();

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  if (bind (
      socketDesc,
      (struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr)
    ) < 0
  ) {
    throwError("[ServerCommunication::ServerCommunication]: Error on on binding");
  }

  clilen = sizeof(struct sockaddr_in);
  //sprintf(buffer, "%s", "message");

  Folder* folder = new Folder("");
  folder->createFolder("db/");
  folder->createFolder("db/clients");

  // Receive clients' requests
  while (TRUE) {
    do {
      status = recvfrom(
        socketDesc,
        buffer,
        CHUNCK_SIZE,
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
  		if (status < 0) {
        throwError("[ServerCommunication::ServerCommunication]: Error on recvfrom");
      }
      const char *loggedUserIdChar = (getLoggedUser()).c_str();
      if (strcmp(loggedUserIdChar, "") == EQUAL) {
        setLoggedUser(buffer);
      }
    } while (strcmp(buffer, UPLOAD) != 0 && strcmp(buffer, LIST_SERVER) != 0);

    if (strcmp(buffer, UPLOAD) == EQUAL) {
      status = recvfrom(
        socketDesc,
        buffer,
        CHUNCK_SIZE,
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
      if (status < 0) {
        throwError("Error on recvfrom");
      }

      strcpy(fname, buffer);
      printf("%s\n", fname);

      status = recvfrom(
        socketDesc,
        buffer,
        CHUNCK_SIZE,
        0,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
      if (status < 0) {
        throwError("Error on recvfrom");
      }

      fileSize = atoi(buffer);
      //printf("%d\n", fileSize);

      fp = fopen(fname, "wb");
      itr = 1;
      memset(buffer, 0, CHUNCK_SIZE);

      while(itr * CHUNCK_SIZE < fileSize) {
        status = recvfrom(
          socketDesc,
          &receiveChunck,
          sizeof(receiveChunck),
          MSG_OOB,
          (struct sockaddr *) &clientAddress,
          &clilen
        );

        if (status < 0) {
          throwError("[ServerCommunication::ServerCommunication]: Error on receiving datagram");
        }

        printf("%d\n", receiveChunck.chunckId);
        sprintf(ack, "%d", receiveChunck.chunckId);

        status = sendto(
          socketDesc,
          ack,
          sizeof(int),
          0,
          (struct sockaddr *) &clientAddress,
          sizeof(struct sockaddr)
        );

        if (status < 0) {
          throwError("[ServerCommunication::ServerCommunication]: Error on sending ack");
        }
        if (lastChunck != receiveChunck.chunckId) {
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
        throwError("[ServerCommunication::ServerCommunication]: Error on sending ack");
      }

      fwrite(receiveChunck.chunck,(fileSize % CHUNCK_SIZE), 1, fp);
      memset(buffer, 0, CHUNCK_SIZE);
      fclose(fp);
      fflush(stdin);

      cout << fname << " has been received" << endl;

      sprintf(buffer, "%s", fname);
    }

    else if (strcmp(buffer, LIST_SERVER) == EQUAL) {
      string listServerToClient = folder->listFiles(SERVER_LIST, getLoggedUser());
      const char *listServerToClientChar = listServerToClient.c_str();

/*
      status = sendto(
        socketDesc,
        listServerToClientChar,
        CHUNCK_SIZE,
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        sizeof(struct sockaddr)
      );
*/
      if (status < 0) {
        throwError("[ServerCommunication::ServerCommunication]: Error on sending ack");
      }
    }
  }

  close(socketDesc);

}

void ServerCommunication::setLoggedUser(string loggedUserId) {
  this->loggedUserId = loggedUserId;
}

string ServerCommunication::getLoggedUser() {
  return this->loggedUserId;
}
