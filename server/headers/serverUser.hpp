#include <string>
#include <vector>
#include <mutex>
#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

class ServerUser {
	private:
		string userid;
        vector<Device*> devices; // Vector of devices
	public:
		Folder *folderPath;
		ServerUser () {}; // Default constructor
		ServerUser (string userid, Folder *folderPath); // TODO: Put devices in this init
};
