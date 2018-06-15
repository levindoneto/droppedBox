#include <string>
#include "../headers/serverUser.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../headers/multiserver.hpp"
#include "../../settings/config.hpp"

using namespace std;

void closeThreadsOpen(map<string,ServerUser*> threads) {
  auto it = threads.cbegin();
  while (it != threads.cend()) {
    if (!it->second->usingActive) {
      delete it->second;
      it = threads.erase(it);
    } else {
      ++it;
    }
  }
}

int main(int argc, char *argv[]) {
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
    MultiServer *ms = new MultiServer();
    ms->startThreads();
    ms->startElectionProcesses();

    Data message = Data::parse(listener.receive());
    closeThreadsOpen(threads);
    if (message.type == Data::T_SYN) {
      if (!threads.count(message.session)) {
        Process* processComm = new Process(
          message.content,
          message.session,
          listener.get_answerer()
        );
        ServerUser* new_thread = new ServerUser(processComm);
        new_thread->start();
        threads[message.session] = new_thread; // Create logged user's sessiom
      }
      else {
        char error[ERROR_MSG_SIZE] = "Session already exists";
        throwError(error);
      }
    }
  }
}
