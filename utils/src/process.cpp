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

#include "../headers/process.hpp"
#include "../headers/ui.hpp"
#include "../headers/dropboxUtils.hpp"

#define BUFFER_SIZE 256

using namespace std;

Process::~Process() {
  cout << "Destroying process..." << endl;
}

bool Process::managerCommands(
  string command,
  string parameter,
  ClientUser* user,
  int port,
  char* host
) {
  int resp;
  if (command.compare(UPLOAD) == 0) {
    resp = upload(parameter, user, port, host);
  } else if (command.compare(DOWNLOAD) == 0) {
      resp = download(parameter, user);
  } else if (command.compare(LIST_SERVER) == 0) {
      resp = listServer(user);
  } else if (command.compare(LIST_CLIENT) == 0) {
      resp = listClient(user);
  } else if (command.compare(GET_SYNC_DIR) == 0) {
      resp = getSyncDir(user);
  } else if (command.compare(EXIT_APP) == 0) {
      resp = exitApp(user);
      if (resp == EXIT_OPT_YES) {
        return EXIT;
      } else if (resp == EXIT_OPT_WRONG) {
          exitApp(user);
      } else {
        return !EXIT;
      }
  } else if (command.compare(HELP_C) == 0 || command.compare(HELP_L) == 0) {
    showHelp();
  }else {
    throwError("Invalid command");
  }
}

int Process::getProcessId() {
  return this->processId;
}

int Process::upload(string fileName, ClientUser* user, int port, char* host) {
  int socketDesc;
  int status;
  unsigned int lenSckAddr;
  const char *fileNameChar = fileName.c_str();
  /* According to the C standard, the address of a structure and its first
     member are the same, so you can cast the pointer to sockaddr_in(6) in a
     pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *server;
  string homePath = getpwuid(getuid())->pw_dir;
  string filePath = homePath + "/sync_dir_" + user->getUserId() + "/" + fileName;
  unsigned int size;
  char str[10], message[BUFFER_SIZE];;
  FILE *file;
  int itr = 1;

  struct Books {
   int  title;
} book;

book.title = 29;


  char buffer[BUFFER_SIZE];
  fflush(stdin);
  // Get host
  server = gethostbyname(host);
  if (server == NULL) {
    throwError("The host does not exist");
  }

  // Open udp socket using the defaul protocol
  if ((socketDesc = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throwError("Error on opening socket");
  }

  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  status = sendto(
    socketDesc,
    fileNameChar,
    BUFFER_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message");
  }

  size = fileSize(filePath);
  sprintf(str, "%d", size);

  status = sendto(
    socketDesc,
    str,
    BUFFER_SIZE,
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message");
  }
  const char *filePathChar = filePath.c_str();
  file = fopen(filePathChar, "rb");
  memset(message, 0, BUFFER_SIZE);
  fread(message, BUFFER_SIZE, 1, file);

  while(itr * BUFFER_SIZE < size){

    status = sendto(
      socketDesc,
      message,
      BUFFER_SIZE,
      0,
      (const struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr_in)
    );
    if (status < 0) {
      throwError("Error on sending message");
    }
        memset(message, 0, BUFFER_SIZE);
        fread(message, BUFFER_SIZE, 1, file);
        itr++;
    }
    fread(message, (size % BUFFER_SIZE), 1, file);
    status = sendto(
      socketDesc,
      message,
      (size % BUFFER_SIZE),
      0,
      (const struct sockaddr *) &serverAddress,
      sizeof(struct sockaddr_in)
    );
    if (status < 0) {
      throwError("Error on sending message");
    }

    memset(message, 0, BUFFER_SIZE);
    fclose(file);
    close(socketDesc);
}

int Process::download(string filePath, ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::listServer(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::listClient(ClientUser* user) {
  cout << "It has to be implemented" << endl;
}

int Process::getSyncDir(ClientUser* user) {
  cout << "It has to be implemented" << endl;
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
      return EXIT_OPT_NO;
  } else {
      cout << INVALID_OPTION << endl;
      return EXIT_OPT_WRONG;
  }
}
