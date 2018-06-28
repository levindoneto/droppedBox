#include <string>
#include "../headers/serverUser.hpp"
#include "../headers/dropboxServer.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../headers/multiserver.hpp"
#include "../../settings/config.hpp"

using namespace std;

DropboxServer::DropboxServer(int port) {
  sock = new UDPUtils(port);
  this->port = port;
}

void DropboxServer::start() {
  listener = new Process(port);
  sock->bindServer();
  cout << "Listening on port " << port << ", awaiting communications...\n";
  while (true) {
    Process *process = listener->rcvProcComm();
    Data r_msg = Data::parse(sock->receive());
    if (r_msg.type == Data::T_BACKUP) {
      UDPUtils backup_socket = UDPUtils(port);
      backup_socket.setIp(r_msg.content);
      DropboxServer::backupServers[r_msg.content] = backup_socket;
    }

    closeThreadsOpen();
    // If session doesn't exist
    if (!threads.count(process->session)) {
      ServerUser *new_thread = new ServerUser(this, process);
      new_thread->start();
      threads[process->session] = new_thread;
    }
  }
}

void DropboxServer::backup(string host_master) {
  sock->setIp(host_master);

  Data msg = Data(NULL, NULL, Data::T_BACKUP, "meu ip");
  sock->send(msg.stringify());

  sock->bindServer();
  while(true) {
    Data r_msg = Data::parse(sock->receive());

    if (r_msg.type == Data::T_UPLOAD) {
      listener = new Process(host_master, port);
      listener->send(Data::T_DOWNLOAD, r_msg.content);
      listener->receive(Data::T_OK);
      // dirpath + "/" + filename
      listener->receive_file("file.txt");  // FIXME dirpath AND filenameeeeeeeeeeeeeeeee
      delete listener;
    }
    else if (r_msg.type == Data::T_IP) {
      DropboxServer::clientIps.push_back(r_msg.content);
    }
    else if (r_msg.type == Data::T_DELETE) {
      cout << "delete";
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
  if (argc > 1) {
    port = atoi(argv[1]);
  }
  DropboxServer* server = new DropboxServer(port);
  server->start();
}
