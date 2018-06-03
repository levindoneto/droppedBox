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

#include "../headers/dropboxClient.h"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/file.hpp"
#include "../../settings/config.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../utils/headers/data.hpp"
#include "../../utils/headers/process.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

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

  Process *processComm = new Process(userName);
  processComm->sock = new UDPUtils(port);
  processComm->sock->setIp(host);
  processComm->login();
  ClientCommunication clientComm(processComm);
  showMenu();
  cout << endl << "**** The user " << userName << " has successfully logged in ****" << endl;

  // Main loop
  string commandToRun, pathOfTheFile;
  while (true) {
    cout << PREFIX_BASH;
    //getline(cin, command);
    cin >> commandToRun;
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

        if (processComm->sendArq(pathOfTheFile) != 0) {
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
      string filepath = processComm->folderOfTheUser + '/' + pathOfTheFile;
      if (processComm->getArq(filepath) == 0)
        cout << pathOfTheFile << " downloaded successfully!" << endl;
      else
        cout << pathOfTheFile << " download failed!" << endl;
    }
    else if (commandToRun == LIST_SERVER) {
      processComm->send(Data::T_LS);
      string server_list = processComm->receive_string();
      formatListOfArqs(server_list);
      processComm->sendConfirmation();
    }
    else if (commandToRun == LIST_CLIENT) {
      listClient(userName);
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
  }
  delete processComm;
  cout << "Successfully logged out!" << endl;
  return 0;
}

void listClient(string userId) {
  listFiles(CLIENT_LIST, userId);
}

void printElement(string data, int width, char separator) {
  cout << left << setw(width) << setfill(separator) << data;
}

void formatListOfArqs(string filelist) {
  char separator = ' ';
  int fieldWidth = 26;
  string delimiter = "|";
  string tworkedProperlyen;
  int pos = 0;
  while ((pos = filelist.find(delimiter)) != std::string::npos) {
    tworkedProperlyen = filelist.substr(0, pos);
    printElement(tworkedProperlyen, fieldWidth, separator);
    filelist.erase(0, pos + delimiter.length());
  }
  cout << endl;
}
