#include <string>
#include "../headers/dropboxServer.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../settings/config.hpp"

using namespace std;

void DropboxServer::backupServerRun(string master_ip, int port) {
  int timeoutToPut = 6; // timeout to generate timeout_exception

  // Get expected expected_types
  list<string> backupExpectedTypes;
  backupExpectedTypes.push_back(Data::T_UPLOAD);
  backupExpectedTypes.push_back(Data::T_DELETE);
  backupExpectedTypes.push_back(Data::T_CLIENT_CONNECT);
  backupExpectedTypes.push_back(Data::T_CLIENT_DISCONNECT);
  backupExpectedTypes.push_back(Data::T_LOGIN);

  list<string> clientIps;
  listener = new Process(master_ip, port, true);
  listener->sendAgainInCaseTO = false;
  bool serverIsAbackup = true;
  while (serverIsAbackup) {
    try {
      listener->sock->set_timeout(timeoutToPut);
      Data dataMessage = listener->receive(backupExpectedTypes);
      if (dataMessage.type == Data::T_UPLOAD) {
        string filepath = dataMessage.content;
        cout << "Backing up ";
        listener->receive_file(filepath);
        cout << "Done :)\n" << endl;      }
      else if (dataMessage.type == Data::T_DELETE) {
        string filepath = dataMessage.content;
        cout << "Delete file :)\n";
        remove(filepath.c_str());
        cout << "Done :)\n" << endl;
      }
      else if (dataMessage.type == Data::T_CLIENT_CONNECT) {
        cout << "Back up communication with all :)\n";
        clientIps.push_back(dataMessage.content);
      }
      else if (dataMessage.type == Data::T_CLIENT_DISCONNECT) {
        cout << dataMessage.content << " disconnected" << endl;
        clientIps.remove(dataMessage.content);
      }
      else if (dataMessage.type == Data::T_LOGIN) {
        File::createFolderForFiles(dataMessage.content);
        listener->rcvConfirmation();
      }
    }
    catch (timeout_exception &e) {
      string new_master = bullyAlgorithm();
      if (new_master.empty()) serverIsAbackup = false;
      else {
        delete listener;
        listener = new Process(new_master, port);
      }
    }
  }
  if (!serverIsAbackup) primaryServerRun(port);
}

string DropboxServer::bullyAlgorithm() {
  UDPUtils *sock = new UDPUtils(4001);
  sock->bindServer();
  string thisIp = getIpAddress();
  for (string &ip : getListOfHostnames()) {
    if (ip > thisIp) {
      sock->setIp(ip);
      sock->send("ELECTION");
    }
  }
  while (true) {
    string dataMessage = sock->receive();
    if (dataMessage == "ELECTION") {
      sock->set_to_answer(sock);
      sock->send("ANSWER");
    }
    else if (dataMessage == "ANSWER") continue;
    else return dataMessage;
  }
}

void DropboxServer::primaryServerRun(int port, list<string> clientIps) {
  listener = new Process(port);
  //sock->bindServer();
  process->sendAgainInCaseTO = false; // init
  giveInfoToClients(clientIps);
  cout << "Listening on port " << port << ", awaiting communications...\n";
  while (true) {
    Process *process = listener->rcvProcComm();
    closeThreadsOpen();
    try {
      Process *process = listener->rcvProcComm();
      closeThreadsOpen();
      if (process->session.at(INIT) == BACKUP_TAG_C) {
        cout << "Backup " << process->ip << " was connected :) \n";
        process->confirmComm();
        backupServers.push_back(process);
      }
      else if (!threads.count(process->session)) {
        ServerUser *new_thread = new ServerUser(this, process);
        new_thread->start();
        threads[process->session] = new_thread;
        for (Process *backup : backupServers) {
          backup->send(Data::T_CLIENT_CONNECT, process->ip);
        }
      }
    } catch (timeout_exception &e) {
      cout << "deu timeout";
        for (Process *backup : backupServers) {
          backup->send(Data::T_SERVER_ALIVE);
        }
    }
  }
}

void DropboxServer::giveInfoToClients(list<string> clientIps) {
  UDPUtils *sock = new UDPUtils(4001);
  string thisIp = getIpAddress();
  for (string &clientHostname : clientIps) {
    sock->setIp(clientHostname);
    sock->send(thisIp);
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
