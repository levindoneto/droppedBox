#include <string>
#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

class ClientUser {
	private:
		string userId;
		Device* device;
	public:
		Folder *folderPath;
		ClientUser () {}; // Default constructor
		ClientUser (string userid, Device* device, Folder *folderPath);
		string getUserId();
};
