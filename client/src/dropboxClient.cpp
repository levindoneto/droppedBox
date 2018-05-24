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
  userName = string(argv[1]);
  host = string(argv[2]);
  port = atoi(argv[3]);

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
  processComm->sock->set_host(host);
  processComm->connect();
  ClientCommunication clientComm(processComm);
  showMenu();
  cout << endl << "**** The user " << userName << " has successfully logged in ****" << endl;

  // Main loop
  string commandToRun, nameOfTheFile;
  while (true) {
    cout << PREFIX_BASH;
    //getline(cin, command);
    cin >> commandToRun;
    if (commandToRun == UPLOAD || commandToRun == DOWNLOAD) {
      cin >> nameOfTheFile;
    }
    if (commandToRun == UPLOAD) {
      try {
        string homePath = getHome() + "/sync_dir_" + userName;
        if (!ifstream(homePath + "/" + nameOfTheFile)) {
          continue;
        }

        processComm->send(Data::T_UPLOAD, nameOfTheFile);
        int workedProperly = processComm->receive_ack();
        if (!workedProperly) {
          processComm->send_ack();
          continue;
        }
        int timestamp = obtainTSofFile(nameOfTheFile);
        processComm->send(Data::T_SOF, to_string(timestamp));
        processComm->receive_ack();
      }
      catch (exception &e) {
        processComm->send_ack(false);
        processComm->receive_ack();

        cout << e.what() << endl;
        continue;
      }
    }
    else if (commandToRun == DOWNLOAD) {
      processComm->send(Data::T_DOWNLOAD, nameOfTheFile);
      bool workedProperly = processComm->receive_ack();
      if (!workedProperly) {
        processComm->send_ack();
        continue;
      }
      string filepath = processComm->folderOfTheUser + '/' + nameOfTheFile;
      if (processComm->getArq(filepath) == 0)
        cout << nameOfTheFile << " downloaded successfully!" << endl;
      else
        cout << nameOfTheFile << " download failed!" << endl;
    }
    else if (commandToRun == LIST_SERVER) {
      processComm->send(Data::T_LS);
      string server_list = processComm->receive_string();
      formatListOfArqs(server_list);
      processComm->send_ack();
    }
    else if (commandToRun == LIST_CLIENT) {
      listClient(userName);
    }
    else if (commandToRun == EXIT_APP) {
      processComm->send(Data::T_BYE);
      processComm->receive_ack();
      processComm->send_ack();
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
