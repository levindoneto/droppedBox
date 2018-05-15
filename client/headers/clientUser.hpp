#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <sys/inotify.h>

#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/ui.hpp"

using namespace std;

class ClientUser {
  private:
    string userId;
    mutex commandMutex;
    queue<vector<string> > commandQueue;
    bool queueEmpty;
  public:
    Folder *userFolder;
    Device* device;

    ClientUser (string userId, Folder* userFolder);

    string getUserId();
    Folder* getUserFolder();
    string getUserConnectedDevicesToString();
    int getNumberOfFiles();

    void setUserFolder(Folder* userFolder);
    void sync();
    bool isSynchronized();
    vector<string> getUserCommand();

    vector<string> getCommandFromQueue();
    void addCommandToQueue(vector<string> command);

    void inotifyEvent();
    void syncDirLoop();
    void commandLoop();
    void startThreads();
    void userLoop();
};
