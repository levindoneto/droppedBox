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

DropboxClient::DropboxClient(string username,
  string hostname,
  int port
) : Thread() {
    this->username = username;
    this->hostname = hostname;
    this->port = port;
}

DropboxClient::~DropboxClient() {
    delete process;
}

void* DropboxClient::run() {
  cout << "Connecting to " << hostname << ":" << port << " ..." << endl;
  process = new Process(hostname, port);
  process->send(Data::T_LOGIN, userId);
  folderOfTheUser = getHome() + "/sync_dir_" + userId;
  File::createFolderForFiles(folderOfTheUser);

  ClientCommunication* clientCommunication = new ClientCommunication(this);
  clientCommunication->start();
  cout << "Successfully logged in as " << userId << "!" << endl;
  mainloop(); // For getting the logged user's requests
}

int DropboxClient::mainloop() {
  string commandToRun;
  string pathOfTheFile;
  while (true) {
    cout << PREFIX_BASH;
    cin >> commandToRun;
    if (commandToRun == UPLOAD || commandToRun == DOWNLOAD) {
      cin >> pathOfTheFile;
    }
    if (commandToRun == UPLOAD) {
      try {
        if (!ifstream(pathOfTheFile)) {
          continue;
        }

        process->send(Data::T_UPLOAD, pathOfTheFile);
        int workedProperly = process->rcvConfirmation();
        if (!workedProperly) {
          process->sendConfirmation();
          continue;
        }
        int timestamp = obtainTSofFile(pathOfTheFile);
        process->send(Data::T_SOF, to_string(timestamp));
        process->rcvConfirmation();

        if (process->sendArq(pathOfTheFile) != EQUAL) {
          char error[ERROR_MSG_SIZE] = "Error sending file";
          throwError(error);
        }
      }
      catch (exception &e) {
        process->sendConfirmation(false);
        process->rcvConfirmation();
        continue;
      }
    }
    else if (commandToRun == DOWNLOAD) {
      process->send(Data::T_DOWNLOAD, pathOfTheFile);
      bool workedProperly = process->rcvConfirmation();
      if (!workedProperly) {
        process->sendConfirmation();
        continue;
      }
      string filepath = getHome() + PATH_SEPARATOR + pathOfTheFile;
      if (process->getArq(filepath) == EQUAL)
        cout << pathOfTheFile << " was successfully downloaded into your home :)" << endl;
      else
        cout << pathOfTheFile << " was not downloaded into your home :(" << endl;
    }
    else if (commandToRun == LIST_SERVER) {
      process->send(Data::T_LS);
      string server_list = process->receive_string();
      formatListOfArqs(server_list);
      process->sendConfirmation();
    }
    else if (commandToRun == LIST_CLIENT) {
      listClient(this->userId);
    }
    else if (commandToRun == EXIT_APP) {
      process->send(Data::T_BYE);
      process->rcvConfirmation();
      process->sendConfirmation();
      break;
    }
    else if (commandToRun == HELP_L) {
      showHelp();
    }
    else {
      cout << "Invalid command" << endl;
    }
  }

  delete process;
  cout << "Successfully logged out!" << endl;
  return TRUE;
}

string listen_new_master(DropboxClient* client, UDPUtils* sock) {
  cout << "Listening new master process" << endl;
  string newPrimaryProcessServer = sock->receive();
  cout << "The new master: " << newPrimaryProcessServer << endl;
  delete client;
  return newPrimaryProcessServer;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  string username = string(argv[1]);
  string hostname = string(argv[2]);
  int port = atoi(argv[3]);
  string master = hostname;
  UDPUtils* sock = new UDPUtils(4002);
  sock->bindServer();
  while (true) {
    DropboxClient *client = new DropboxClient(username, master, port);
    client->start();
    master = listen_new_master(client, sock);
  }
}
