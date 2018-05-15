#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <dirent.h>

#include "../headers/process.hpp"
#include "../headers/ui.hpp"
#include "../headers/udpUtils.hpp"
#include <unistd.h>
using namespace std;

Process::~Process() {
  cout << "Destroying process..." << endl;
}

int Process::managerCommands(
  string command,
  string parameter,
  ClientUser* user,
  int port,
  string host,
  int socketDesc
) {
  int status;
  int resp;
  if (command.compare(UPLOAD) == EQUAL) {
    resp = upload(parameter, user, port, host, socketDesc);
  } else if (command.compare(DOWNLOAD) == EQUAL) {
      resp = download(parameter, user, host, port, socketDesc);
  } else if (command.compare(LIST_SERVER) == EQUAL) {
      resp = listServer(user, port, host, socketDesc);
  } else if (command.compare(LIST_CLIENT) == EQUAL) {
      resp = listClient(user, port, host, socketDesc);
  } else if (command.compare(GET_SYNC_DIR) == EQUAL) {
      resp = getSyncDir(user, port, host, socketDesc);
  } else if (command.compare(EXIT_APP) == EQUAL) {
      resp = exitApp(user);
      if (resp == EXIT_OPT_YES) {
        //string clientRequest = "[Client Request]: Log off user";
        //writeToSocket(clientRequest, socketDesc, host, port);
        closeSocket(socketDesc);
        return EXIT;
      } else if (resp == EXIT_OPT_WRONG) {
          resp = exitApp(user);
      } else {
        return !EXIT;
      }
  } else if (command.compare(HELP_C) == 0 || command.compare(HELP_L) == 0) {
    showHelp();
  }else {
    throwError("[managerCommands]: Invalid command");
  }
}

int Process::getProcessId() {
  return this->processId;
}

/*
void Process::setLoggedUserId(ClientUser* user) {
  this->loggedUserId = "user->getUserId()";
}
*/

int Process::upload(string fileName, ClientUser* user, int port, string host, int socketDesc) {
  int status;
  unsigned int lenSckAddr;
  const char *fileNameChar = fileName.c_str();
  const char *hostChar = host.c_str();
  /* According to the C standard, the address of a structure and its first
     member are the same, so you can cast the pointer to sockaddr_in(6) in a
     pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *server;
  Datagram sendChunck;
  UserInfo userInfo = {};
  string homePath = getpwuid(getuid())->pw_dir;
  string filePath = homePath + "/sync_dir_" + user->getUserId() + "/" + fileName;
  unsigned int size;
  char str[10], ack[10];
  FILE *file;
  int itr = 1;
  //struct datagram sendChunck;

  fflush(stdin);
  // Get host
  server = gethostbyname(hostChar);
  if (server == NULL) {
    throwError("[Process::upload]: The host does not exist");
  }

  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  strcpy(userInfo.message, UPLOAD);
  user->getUserId().copy(userInfo.userId, user->getUserId().length(), 0);

  status = sendto(
    socketDesc,
    &userInfo,
    sizeof(userInfo),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message");
  }

  status = sendto(
    socketDesc,
    fileNameChar,
    CHUNCK_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::upload]: Error on sending message");
  }

  size = fileSize(filePath);
  //printf("%d\n", size);
  sprintf(str, "%d", size);

  status = sendto(
    socketDesc,
    str,
    CHUNCK_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::upload]: Error on sending message");
  }
  const char *filePathChar = filePath.c_str();
  file = fopen(filePathChar, "rb");
  memset(sendChunck.chunck, 0, CHUNCK_SIZE);
  fread(sendChunck.chunck, CHUNCK_SIZE, 1, file);
  sendChunck.chunckId = 1;

  while(itr * CHUNCK_SIZE < size){
    status = sendto(
      socketDesc,
      &sendChunck,
      sizeof(sendChunck),
      0,
      (const struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr_in)
    );
    if (status < 0) {
      throwError("[Process::upload]: Error on sending message");
    }

    status = recvfrom(
      socketDesc,
      ack,
      sizeof(int),
      MSG_OOB,
      (struct sockaddr *) &from,
      &lenSckAddr
    );
    if (status < 0) {
      throwError("[Process::upload]: Error on receive ack");
    }

    //printf("%s\n", ack);
    //printf("%d\n", atoi(ack));


    if(atoi(ack) == sendChunck.chunckId) {
      memset(sendChunck.chunck, 0, CHUNCK_SIZE);
      fread(sendChunck.chunck, CHUNCK_SIZE, 1, file);
      itr++;
      sendChunck.chunckId = sendChunck.chunckId%1000 + 1;
    }
  }

  fread(sendChunck.chunck, (size % CHUNCK_SIZE), 1, file);

  status = sendto(
    socketDesc,
    &sendChunck,
    sizeof(sendChunck),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::upload]: Error on sending message");
  }

  memset(sendChunck.chunck, 0, CHUNCK_SIZE);
  fclose(file);
  //close(socketDesc);
}

int Process::download(string fileName, ClientUser* user, string host, int port, int socketDesc) {
  int status, lastChunck = 0, sizeInt;
  unsigned int lenSckAddr;
  const char *fileNameChar = fileName.c_str();
  const char *hostChar = host.c_str();
  /* According to the C standard, the address of a structure and its first
     member are the same, so you can cast the pointer to sockaddr_in(6) in a
     pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *server;
  Datagram receiveChunck;
  UserInfo userInfo = {};
  string homePath = getpwuid(getuid())->pw_dir;
  string filePath = homePath + "/sync_dir_" + user->getUserId() + "/" + fileName;
  char str[10], ack[10], size[CHUNCK_SIZE];
  FILE *file;
  int itr = 1;
  //struct datagram sendChunck;

  fflush(stdin);
  // Get host
  server = gethostbyname(hostChar);
  if (server == NULL) {
    throwError("[Process::download]: The host does not exist");
  }

  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  strcpy(userInfo.message, DOWNLOAD);
  user->getUserId().copy(userInfo.userId, user->getUserId().length(), 0);

  status = sendto(
    socketDesc,
    &userInfo,
    sizeof(userInfo),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::download]: Error on sending message");
  }

  status = sendto(
    socketDesc,
    fileNameChar,
    CHUNCK_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::download]: Error on sending message");
  }

  status = recvfrom(
    socketDesc,
    size,
    CHUNCK_SIZE,
    MSG_OOB,
    (struct sockaddr *) &from,
    &lenSckAddr
  );
  if (status < 0) {
    throwError("[Process::download]: Error on receive ack");
  }

  sizeInt = atoi(size);
  const char *filePathChar = filePath.c_str();
  file = fopen(filePathChar, "wb");

  while(itr * CHUNCK_SIZE < sizeInt) {
    status = recvfrom(
      socketDesc,
      &receiveChunck,
      sizeof(receiveChunck),
      MSG_OOB,
      (struct sockaddr *) &from,
      &lenSckAddr
    );

    if (status < 0) {
      throwError("[Process::download]: Error on receiving datagram");
    }

    sprintf(ack, "%d", receiveChunck.chunckId);

    status = sendto(
      socketDesc,
      ack,
      sizeof(int),
      0,
      (struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr)
    );

    if (status < 0) {
      throwError("[Process::download]: Error on sending ack");
    }
    if (lastChunck != receiveChunck.chunckId) {
      fwrite(receiveChunck.chunck, CHUNCK_SIZE, 1, file);
      memset(receiveChunck.chunck, 0, CHUNCK_SIZE);
      itr++;
    }
    lastChunck = receiveChunck.chunckId;
  }

  memset(receiveChunck.chunck, 0, (sizeInt % CHUNCK_SIZE));
  status = recvfrom(
    socketDesc,
    &receiveChunck,
    sizeof(receiveChunck),
    0,
    (struct sockaddr *) &from,
    &lenSckAddr
  );
  if (status < 0) {
    throwError("[Process::download]: Error on sending ack");
  }

  fwrite(receiveChunck.chunck,(sizeInt % CHUNCK_SIZE), 1, file);
  memset(receiveChunck.chunck, 0, CHUNCK_SIZE);
  fclose(file);
  fflush(stdin);

  cout << fileName << " has been received" << endl;
}

int Process::listServer(ClientUser* user, int port, string host, int socketDesc) {
  const char *hostChar = host.c_str();
  struct hostent *server;
  server = gethostbyname(hostChar);
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  char listFromServer[CHUNCK_SIZE];
  unsigned int lenSckAddr;
  UserInfo userInfo;
  char ack[ACK_SIZE];
  int status;

  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  // Send the request for getting the server list
  strcpy(userInfo.message, LIST_SERVER);
  user->getUserId().copy(userInfo.userId, user->getUserId().length(), 0);

  status = sendto(
    socketDesc,
    &userInfo,
    sizeof(userInfo),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message");
  }

  while (TRUE) {
    status = recvfrom(
      socketDesc,
      listFromServer,
      CHUNCK_SIZE,
      0,
      (struct sockaddr *) &serverAddress,
      &lenSckAddr
    );
    if (status < 0) {
      throwError("[Process::upload]: Error on receive ack");
    }
    fflush(stdout);
    printf("%s\n", listFromServer);
    return !EXIT;
  }
}

int Process::listClient(ClientUser* user, int port, string host, int socketDesc) {
  //string clientRequest = "[Client Request]: List files on the client side";
  //writeToSocket(clientRequest, socketDesc, host, port);
  Folder* procFolder = new Folder();
  procFolder->listFiles(CLIENT_LIST, user->getUserId());
  return !EXIT;
}

int Process::getSyncDir(ClientUser* user, int port, string host, int socketDesc) {
  Folder* procFolder = new Folder();
  string folderListStr;
  string userFolderPath;
  userFolderPath = procFolder->getHome() + "/sync_dir_" + user->getUserId();
  const char *hostChar = host.c_str();
  struct hostent *server;
  server = gethostbyname(hostChar);
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  char listFromServer[CHUNCK_SIZE];
  unsigned int lenSckAddr;
  UserInfo userInfo;
  char ack[ACK_SIZE];
  int status;

  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  // Send the request for start the sync between client and server
  strcpy(userInfo.message, GET_SYNC_DIR);
  user->getUserId().copy(userInfo.userId, user->getUserId().length(), 0);
  status = sendto(
    socketDesc,
    &userInfo,
    sizeof(userInfo),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("[Process::getSyncDir]: Error on sending the request");
  }

  const char *folder = userFolderPath.c_str();
  DIR * dir = opendir(folder);
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != EQUAL) {
      if (entry->d_type != DT_DIR) { // If entry is a file
        string filenameClient = entry->d_name;
        string slash = SLASH;
        string filePath = userFolderPath + slash + filenameClient;
        string requestNameTime;
        requestNameTime = procFolder->getFileWithModificationTime(filenameClient, filePath);
        char requestNameTimeChar[CHUNCK_SIZE];
        requestNameTime.copy(requestNameTimeChar, requestNameTime.length(), 0);

        // Send the the name and modification time for comparison on the server side
        status = sendto(
          socketDesc,
          requestNameTimeChar,
          sizeof(requestNameTimeChar),
          0,
          (const struct sockaddr *) &serverAddress,
          sizeof(struct sockaddr_in)
        );
        if (status < 0) {
          throwError("[Process::getSyncDir]: Error on sending the filename with modification time");
        }
        //cout << requestNameTime << endl;
        // up files test
        //upload(filenameClient, user, port, host, socketDesc);
      }
    }
  }

  /* GET FILE BY FILE
     SEND THE FILE TO CLIENT (NAME, TIMES)
             SERVER PARSES IT (get name and times, compare times and returns an ack:
                ACK_DOWNLOAD:0,
                ACK_UPLOAD:1,
                ACK:SAME:2
             )
    CLIENT TREATS THE ACK

  */

  return !EXIT;
}

int Process::exitApp(ClientUser* user) {
  string userAnswer;
  cout << "$ Do you really want to log off, " << user->getUserId()
    << "? [yes or no]" << endl;
    cout << "$ ";
  cin >> userAnswer;

  if (userAnswer.compare("yes") == 0 || userAnswer.compare("YES") == 0) {
    cout << "$ Have a good one and take care, " << user->getUserId() << " !" << endl;
    return EXIT_OPT_YES;
  } else if (userAnswer.compare("no") == 0 || userAnswer.compare("NO") == 0) {
      cout << "$ You can stay logged in then!" << endl;
  } else {
      cout << INVALID_OPTION << endl;
      return EXIT_OPT_WRONG;
  }
}
