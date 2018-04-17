#include <string>
#include <vector>
#include <mutex>
#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

class ServerUser {
  private:
    string userid;
    int userReaders; // Users who are reading from the server
    mutex aReader;
    mutex aWriterReader;
    vector<Device*> devices; // Vector of devices
  public:
    Folder *folderPath;
    ServerUser () {}; // Default constructor
    ServerUser (string userid, Folder *folderPath);
    ServerUser (string userid, Folder *folderPath, Device* device);

    string getUserId();
    Folder* getUserFolder();

    bool thereAreNoDevices();
    void delDevice(Device* device);
    void addDevice(Device* device);
};
