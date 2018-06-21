#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdint.h>
#include <iomanip>

#include "../headers/dropboxClient.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/file.hpp"
#include "../../settings/config.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/data.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

void DropboxClient::newProcessCommunication(
  string userId,
  string host,
  int port
) {
  this->userId = userId;
  this->folderOfTheUser = getHome() + SYNC_DIR_PREFIX + userId;
  processComm = new Process(userId); // TODO: REMOVE THE PARAMETER (Redundancy)

  processComm->sock = new UDPUtils(port);
  processComm->sock->setIp(host);
  processComm->login();
  ClientCommunication clientComm(processComm);

  showMenu();
  cout << endl << "**** The user " << userId
    << " has successfully logged in ****" << endl;
  run(); // For getting the logged user's requests
}

int DropboxClient::verifyServerAlive() {
  FILE *fp;
  fp = fopen("Welcome.txt","w");
  fclose(fp);
  string pathOfTheFile = "Welcome.txt";
  printf("travei1");
  processComm->send(Data::T_UPLOAD, pathOfTheFile);
  printf("travei2");
  int workedProperly = processComm->rcvConfirmation();
  printf("im alive");
  return workedProperly;
}

int DropboxClient::run() {
  string commandToRun;
  string pathOfTheFile;
  while (true) {
    cout << PREFIX_BASH;
    cin >> commandToRun;
    // printf("if...\n");
    // cout << verifyServerAlive();
    // if (verifyServerAlive()) {
    //   printf("else...\n");

    if (commandToRun == UPLOAD || commandToRun == DOWNLOAD) {
      cin >> pathOfTheFile;
    }
    if (commandToRun == UPLOAD) {
      try {
        if (!ifstream(pathOfTheFile)) {
          continue;
        }

        processComm->send(Data::T_UPLOAD, pathOfTheFile);
        int workedProperly = processComm->rcvConfirmation();
        if (!workedProperly) {
          processComm->sendConfirmation();
          continue;
        }
        int timestamp = obtainTSofFile(pathOfTheFile);
        processComm->send(Data::T_SOF, to_string(timestamp));
        processComm->rcvConfirmation();

        if (processComm->sendArq(pathOfTheFile) != EQUAL) {
          char error[ERROR_MSG_SIZE] = "Error sending file";
          throwError(error);
        }
      }
      catch (exception &e) {
        processComm->sendConfirmation(false);
        processComm->rcvConfirmation();
        continue;
      }
    }
    else if (commandToRun == DOWNLOAD) {
      processComm->send(Data::T_DOWNLOAD, pathOfTheFile);
      bool workedProperly = processComm->rcvConfirmation();
      if (!workedProperly) {
        processComm->sendConfirmation();
        continue;
      }
      string filepath = getHome() + PATH_SEPARATOR + pathOfTheFile;
      if (processComm->getArq(filepath) == EQUAL)
        cout << pathOfTheFile << " was successfully downloaded into your home :)" << endl;
      else
        cout << pathOfTheFile << " was not downloaded into your home :(" << endl;
    }
    else if (commandToRun == LIST_SERVER) {
      processComm->send(Data::T_LS);
      string server_list = processComm->receive_string();
      formatListOfArqs(server_list);
      processComm->sendConfirmation();
    }
    else if (commandToRun == LIST_CLIENT) {
      listClient(this->userId);
    }
    else if (commandToRun == EXIT_APP) {
      processComm->send(Data::T_BYE);
      processComm->rcvConfirmation();
      processComm->sendConfirmation();
      break;
    }
    else if (commandToRun == HELP_L) {
      showHelp();
    }
    else {
      cout << "Invalid command" << endl;
    }
  //}
  }

  delete processComm;
  cout << "Successfully logged out!" << endl;
  return TRUE;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  string userName;
  string host;
  int port;
  userName = string(argv[USER_CLIENT]);
  host = string(argv[HOST_CLIENT]);
  port = atoi(argv[PORT_CLIENT]);

  if (
    argv[USER_CLIENT] != NULL &&
    argv[HOST_CLIENT] != NULL &&
    argv[PORT_CLIENT] != NULL
  ) {
    userName = argv[USER_CLIENT];
    host = argv[HOST_CLIENT];
    port = atoi(argv[PORT_CLIENT]);
  } else {
    cout << PREFIX_BASH << "Usage:" << endl
      << PREFIX_BASH << "./dropboxClient <idUser> <host> <port>" << endl;
    char error[ERROR_MSG_SIZE] = "[Client]: Invalid use of the application";
    throwError(error);
  }
  DropboxClient dropboxClient;
  dropboxClient.newProcessCommunication(userName, host, port); // Used for the frontend
}
