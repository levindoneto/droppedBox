#ifndef DROPBOXSERVER_H
#define DROPBOXSERVER_H

#include "../../utils/headers/dropboxUtils.h"
#include "../headers/serverUser.hpp"

class DropboxServer {
  private:
    // Methods
    void closeThreadsOpen();
    void rcvProcComm(UDPUtils listener); // receive a communication via socket listener
  public:
      // Attr
      DropboxServer(int port);
      void primaryServerRun(int port);
      void backupServerRun(string mainServerHost, int port); // IT HAS TO BE IMPLEMENTED TODO
      // Methods
      Process* listener;
      UDPUtils* sock;
      int port;
      map<string, ServerUser*> threads;
      map<string, ServerCommunication*> *syncThreads;
      static list<string> clientHosts;
      static map<string, UDPUtils> serverBackups;
  };

  #endif
