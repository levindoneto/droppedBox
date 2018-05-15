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
#include <sys/stat.h>

using namespace std;

#define TRUE 1

ServerCommunication::ServerCommunication() {
  setLoggedUser("");
}

void ServerCommunication::serverComm(int port) {
  int socketDesc, itr = 1, status, lastChunck = 0;
  socklen_t clilen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  Datagram receiveChunck;
  Datagram sendChunck;
  UserInfo userInfo = {};
  char buffer[CHUNCK_SIZE];
  fflush(stdin);
  char fname[20], ack[10], str[10];
  FILE *fp;
  unsigned int filesizeInt;
  string database = DATABASE;
  string userInfoId;
  string slash = "/";
  string filePathDB;
  const char *filePathDBChar;

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
        &userInfo,
        sizeof(userInfo),
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
  		if (status < 0) {
        throwError("[ServerCommunication::ServerCommunication]: Error on recvfrom");
      }

      if (strcmp(userInfo.message, UPLOAD) != 0 &&
      strcmp(userInfo.message, DOWNLOAD) != 0 &&
      strcmp(userInfo.message, LIST_SERVER) != 0
      && strcmp(userInfo.message, GET_SYNC_DIR) != 0) {
        cout << userInfo.message << endl;
      }

      const char *loggedUserIdChar = (getLoggedUser()).c_str();
      if (strcmp(loggedUserIdChar, "") == EQUAL) {
        setLoggedUser(userInfo.userId);
      }
      string userIdLogged;
      userIdLogged = userInfo.userId;
      string userPath = "db/clients/sync_dir_" + userIdLogged;
      folder->createFolder("db/clients/" + userIdLogged);
    } while (strcmp(userInfo.message, UPLOAD) != 0 &&
    strcmp(userInfo.message, DOWNLOAD) != 0 &&
    strcmp(userInfo.message, LIST_SERVER) != 0  &&
    strcmp(userInfo.message, GET_SYNC_DIR) != 0);

    if (strcmp(userInfo.message, UPLOAD) == EQUAL) {
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

    userInfoId = userInfo.userId;
    filePathDB = database + userInfoId + slash + buffer;
    filePathDBChar = filePathDB.c_str();
    fp = fopen(filePathDBChar, "wb");

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

  filesizeInt = atoi(buffer);

    itr = 1;
    memset(buffer, 0, CHUNCK_SIZE);

    while(itr * CHUNCK_SIZE < filesizeInt) {
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

      fflush(stdin);
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

    memset(receiveChunck.chunck, 0, (filesizeInt % CHUNCK_SIZE));
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

    fwrite(receiveChunck.chunck,(filesizeInt % CHUNCK_SIZE), 1, fp);
    memset(buffer, 0, CHUNCK_SIZE);
    fclose(fp);
    fflush(stdin);

    sprintf(buffer, "%s", fname);
    }
    else if (strcmp(userInfo.message, DOWNLOAD) == 0) {
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

    userInfoId = userInfo.userId;
    filePathDB = database + userInfoId + slash + buffer;
    filePathDBChar = filePathDB.c_str();
    fp = fopen(filePathDBChar, "rb");
    filesizeInt = fileSize(filePathDBChar);

    sprintf(str, "%d", filesizeInt);

    status = sendto(
      socketDesc,
      str,
      CHUNCK_SIZE,
      0,
      (const struct sockaddr *) &clientAddress,
      sizeof(struct sockaddr_in)
    );
    if (status < 0) {
      throwError("[Process::upload]: Error on sending message");
    }

    memset(sendChunck.chunck, 0, CHUNCK_SIZE);
    fread(sendChunck.chunck, CHUNCK_SIZE, 1, fp);
    sendChunck.chunckId = 1;

    while(itr * CHUNCK_SIZE < filesizeInt){
      status = sendto(
        socketDesc,
        &sendChunck,
        sizeof(sendChunck),
        0,
        (const struct sockaddr *) &clientAddress,
        sizeof(struct sockaddr_in)
      );
      if (status < 0) {
        throwError("[Process::upload]: Error on sending message");
      }
      fflush(stdin);
      status = recvfrom(
        socketDesc,
        ack,
        sizeof(int),
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
      if (status < 0) {
        throwError("[Process::upload]: Error on receive ack");
      }

      if(atoi(ack) == sendChunck.chunckId) {
        memset(sendChunck.chunck, 0, CHUNCK_SIZE);
        fread(sendChunck.chunck, CHUNCK_SIZE, 1, fp);
        itr++;
        sendChunck.chunckId = sendChunck.chunckId%1000 + 1;
      }
    }

    fread(sendChunck.chunck, (filesizeInt % CHUNCK_SIZE), 1, fp);

    status = sendto(
      socketDesc,
      &sendChunck,
      sizeof(sendChunck),
      0,
      (const struct sockaddr *) &clientAddress,
      sizeof(struct sockaddr_in)
    );
    if (status < 0) {
      throwError("[Process::upload]: Error on sending message");
    }

    memset(sendChunck.chunck, 0, CHUNCK_SIZE);
    fclose(fp);
    sprintf(userInfo.message, "%s", fname);
  }

    else if (strcmp(userInfo.message, LIST_SERVER) == EQUAL) {

      fflush(stdin);
      string listServerToClient = folder->listFiles(SERVER_LIST, userInfo.userId);
      char listServerToClientChar[CHUNCK_SIZE];
      listServerToClient.copy(listServerToClientChar, listServerToClient.length(), 0);

      status = sendto(
        socketDesc,
        listServerToClientChar,
        sizeof(listServerToClientChar),
        0,
        (struct sockaddr *) &clientAddress,
        sizeof(struct sockaddr)
      );
      if (status < 0) {
        throwError("[ServerCommunication::serverComm]: Error on sending the list of files");
      }
    }

    else if (strcmp(userInfo.message, GET_SYNC_DIR) == EQUAL) {
      char receiveTimes[CHUNCK_SIZE], ackGetSync[10];
      struct stat stats;

      status = recvfrom(
        socketDesc,
        receiveTimes,
        sizeof(receiveTimes),
        MSG_OOB,
        (struct sockaddr *) &clientAddress,
        &clilen
      );
      if (status < 0) {
        throwError("Error on recvfrom");
      }

      char* name;
      char* timeChar;
      name = strtok(receiveTimes, " ");
      timeChar = strtok(NULL, " ");

      time_t timeType1 = (time_t) atoll(timeChar);
      userInfoId = userInfo.userId;
      filePathDB = database + userInfoId + slash + name;
      filePathDBChar = filePathDB.c_str();

      if (stat(filePathDBChar, &stats) == ERROR) {
        throwError("[serverCommunication::getSyncDir]: Error on getting to the file");
      }

      time_t timeType2 = time_t(stats.st_mtim.tv_sec);

      int diff = difftime(timeType1, timeType2);

      // If time is bigger on the client
      if (diff > 0) {
        // The client has to upload the file
        sprintf(ackGetSync, "%d", ACK_UPLOAD);
        status = sendto(
          socketDesc,
          ackGetSync,
          sizeof(int),
          0,
          (const struct sockaddr *) &clientAddress,
          sizeof(struct sockaddr_in)
        );
        if (status < 0) {
          throwError("[ServerCommunication::serverComm]: Error on sending ack");
        }
      }
      // If time is bigger on the server
      else if (diff < 0) {
        // The client has to download the file
        sprintf(ackGetSync, "%d", ACK_DOWNLOAD);
        status = sendto(
          socketDesc,
          ackGetSync,
          sizeof(int),
          0,
          (const struct sockaddr *) &clientAddress,
          sizeof(struct sockaddr_in)
        );
        if (status < 0) {
          throwError("[ServerCommunication::serverComm]: Error on sending ack");
        }
      }
      // If time is the same on the client and the server
      else {
        // The client and the server have the same files
        sprintf(ackGetSync, "%d", ACK_SAME);
        status = sendto(
          socketDesc,
          ackGetSync,
          sizeof(int),
          0,
          (const struct sockaddr *) &clientAddress,
          sizeof(struct sockaddr_in)
        );
        if (status < 0) {
          throwError("[ServerCommunication::serverComm]: Error on sending ack");
        }
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
