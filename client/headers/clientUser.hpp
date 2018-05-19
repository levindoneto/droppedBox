#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"
#include "../../utils/fileSystem/headers/file.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/udpUtils.hpp"
#include "../../settings/config.hpp"

#define DEBUG 0
#define TRUE 1
#define BYTE_IN_BITS 8

using namespace std;

class ClientUser {
  private:
    string userId;
    mutex commandMutex;
    queue<vector<string> > commandQueue;
    bool queueEmpty;

    int port;
    int socketDescriptor;
    char* ip;

  public:
    Folder *userFolder;

    ClientUser (string userId, Folder* userFolder, char* ip, int port);

    string getUserId();
    Folder* getUserFolder();

    void setUserFolder(Folder* userFolder);
    void sync();
    bool isSynchronized();
    vector<string> getUserCommand();

    vector<string> getCommandFromQueue();
    void addCommandToQueue(vector<string> command);

    int loginServer();

    void inotifyEvent();
    void syncDirLoop();
    void commandLoop();
    void startThreads();
    void userLoop();
};
