#include <string>
#include "../headers/serverUser.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../headers/multiserver.hpp"
#include "../../settings/config.hpp"

using namespace std;

class DropboxServer {
  public:
    DropboxServer() {
      syncThreads = new map<string, ServerCommunication *>();
      threads = new map<string, ServerUser*>();
    };
    ~DropboxServer() {}; // Destroy server
    map<string, ServerCommunication*> *syncThreads;
    map<string, ServerUser*> *threads;
    void closeThreadsOpen() {
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
};

int main(int argc, char *argv[]) {
  // Create a object for the server (with the method for cleaning server threads)
  DropboxServer *dropboxServer = new DropboxServer();

  int port;
  string typeServer;
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
  cout << "******* Server is running *******" << endl << endl;

  while (true) {
    printf("im alive");
    //MultiServer *ms = new MultiServer();
    //ms->startThreads();
    //ms->startElectionProcesses();

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
    }
  }
}
