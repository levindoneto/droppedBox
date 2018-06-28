#include <string>
#include "../headers/dropboxServer.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../settings/config.hpp"

using namespace std;

void DropboxServer::backupServerRun(string hostnamePrimary, int port) {
  list<string> backupExpectedTypes = {
    Data::T_UPLOAD,
    Data::T_DELETE,
    Data::T_CLIENT_CONNECT,
    Data::T_CLIENT_DISCONNECT
  };
  listener = new Process(hostnamePrimary, port, true); // true: server is a backup
  while (true) {
    Data msg = listener->receive(backupExpectedTypes);
    if (msg.type == Data::T_UPLOAD) {
      string pathOfTheFile = msg.content;
      listener->receive_file(pathOfTheFile);
    }
    else if (msg.type == Data::T_DELETE) {
      string pathOfTheFile = msg.content;
      remove(pathOfTheFile.c_str());
    }
    else if (msg.type == Data::T_CLIENT_CONNECT) {
      threads[msg.content] = NULL;
    }
    else if (msg.type == Data::T_CLIENT_DISCONNECT) {
      threads.erase(msg.content);
    }
  }
}

void DropboxServer::primaryServerRun(int port) {
  listener = new Process(port);
  //sock->bindServer();
  cout << "Listening on port " << port << ", awaiting communications...\n";
  while (true) {
    Process *process = listener->rcvProcComm();
    closeThreadsOpen();
    if (process->session.at(INIT) == BACKUP_TAG_C) {
      backupServers.push_back(process); // Add new server back process to the list
    } else if (!threads.count(process->session)) {
       // If new client process communication
       ServerUser *newUserThread = new ServerUser(this, process);
       newUserThread->start();
       threads[process->session] = newUserThread;
       for (Process* backup : backupServers) {
         backup->send(Data::T_CLIENT_CONNECT, process->ip);
       }
    }
  }
}

void DropboxServer::closeThreadsOpen() {
  auto it = threads.cbegin();
  while (it != threads.cend()) {
    ServerUser *serverUserThread = it->second;
    if (!serverUserThread->usingActive) {
      delete serverUserThread;
      it = threads.erase(it);
    }
    else ++it;
  }
}

int main(int argc, char *argv[]) {
  int port;
  if (argc > DEF) port = atoi(argv[PORT_SERVER]);
  string thisProcessHostname = getIpAddress();
  string primaryServer = "";
  for (string &hostname : getListOfHostnames()) {
    if (thisProcessHostname < hostname) {
      primaryServer = hostname;
      break;
    }
  }
  DropboxServer *server = new DropboxServer();
  if (primaryServer.empty()) server->primaryServerRun(port);
  else server->backupServerRun(primaryServer, port);
}
