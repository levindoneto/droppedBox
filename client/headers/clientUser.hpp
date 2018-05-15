#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <sys/inotify.h>

#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/ui.hpp"

using namespace std;

class ClientUser {
  private:
    string userId;
    bool isSync;
    mutex accessSync; // For more than one device
    int numberOfFiles;
  public:
    Folder *userFolder;
    Device* device;

    ClientUser (string userId, Folder* userFolder);
    ClientUser (string userId, Device* device, Folder *userFolder);
    ClientUser (string userId, Device *device, Folder *userFolder, int numberOfFiles);

    string getUserId();
    Folder* getUserFolder();
    string getUserConnectedDevicesToString();
    int getNumberOfFiles();

    void setUserFolder(Folder* userFolder);
    void sync();
    bool isSynchronized();
    vector<string> getUserCommand();

    void inotifyEvent();
    void syncDirLoop();
    void commandLoop();
    void startThreads();
    void userLoop();
};
