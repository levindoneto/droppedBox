#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"
#include "../../utils/headers/dropboxUtils.hpp"

using namespace std;

class ClientUser {
  private:
    string userId;
    bool isSync;
    mutex accessSync; // For more than one device
  public:
    Folder *userFolder;
    Device* device;

    ClientUser(string userId, Folder *userFolder); // Default constructor
    ClientUser (string userid, Device* device, Folder *userFolder);

    string getUserId();
    Folder* getUserFolder();

    void setUserFolder(Folder* userFolder);
    void sync();
    bool isSynchronized();
    vector<string> getUserCommand();
};
