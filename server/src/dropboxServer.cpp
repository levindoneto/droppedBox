#include <string>
#include "../headers/serverUser.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../headers/multiserver.hpp"
#include "../../settings/config.hpp"

using namespace std;

DropboxServer::DropboxServer(int port) {
    sock = new Socket(port);
    this->port = port;
}

void DropboxServer::closeThreadsOpen() {
  auto it = (*threads).cbegin();
  while (it != (*threads).cend()) {
    if (it->second->usingActive) {
      ++it;
    } else {
        delete it->second;
        it = (*threads).erase(it);
    }
  }
}

void primaryServerRun(int port) {
  listener = new Process(port);
  cout << "Listening on port " << port << " for connections..." << endl;
  while (true) {
    Process *process = listener->rcvProcComm();
    closeThreadsOpen();
    // if backup server
    if (process->session.at(0) == 'b') {
      //cout << "Backup " << process->host << " connected" << endl;
      connection->confirmComm();
      serverBackups.push_back(process);
    } else if (!threads.count(connection->session)) { // New communication with client
        ServerUser *newUserThread = new ServerThread(this, process);
        newUserThread->start();
        threads[process->session] = newUserThread;
        for (Process *serverBackup : serverBackups) {
            serverBackup->send(Data::T_PROCCLIENT_ON, process->hostname);
        }
      }
  }
}

void Server::backup_loop(string master_ip, int port) {
  //TODO
}

int main(int argc, char *argv[]) {
  // Create a object for the server (with the method for cleaning server threads)
  DropboxServer *dropboxServer = new DropboxServer();
  int port;
  pid_t pid;
  string typeServer;
  string ipAddress;
  if (argc > 2) {
    port = atoi(argv[PORT_SERVER]);
    typeServer = string(argv[TYPE_SERVER]);
  } else {
      char error[ERROR_MSG_SIZE] = "[DropboxServer]: Invalid parameter";
      throwError(error);
  }
  map<string,ServerUser*> threads;
  UDPUtils listener(port);
  listener.bindServer();
  pid = getIdOfProcess();
  ipAddress = getipAddress();
  cout << "******* Server is running *******" << endl << endl;

  while (true) {
    printf("im alive");
    //MultiServer *ms = new MultiServer();
    //ms->startThreads();
    //ms->startElectionProcesses();
    MultiServer *ms = new MultiServer(port, pid, ipAddress);

    Data message = Data::parse(listener.receive());
    dropboxServer->closeThreadsOpen();
    if (message.type == Data::T_SYN) {
      if ((*dropboxServer->threads).count(message.session)) {
        char error[ERROR_MSG_SIZE] = "Session already exists";
        throwError(error);
      }
      else {
        Process* processComm = new Process(
          message.content,
          message.session,
          listener.get_answerer()
        );
        ServerUser* newUserThread = new ServerUser(
          processComm,
          dropboxServer->threads,
          dropboxServer->syncThreads
        );
        newUserThread->start();
        // Create logged user's session
        (*dropboxServer->threads)[message.session] = newUserThread;
      }

      // TODO Add regarding type of server (primary or backup)

    }
  }
}
