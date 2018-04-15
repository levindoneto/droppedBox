#include <string>
#include <mutex>
#include "../../utils/headers/device.hpp"
#include "../../utils/fileSystem/headers/folder.hpp"

using namespace std;

class ClientUser {
	private:
		string userId;
		bool isSync;
		mutex accessSync; // For more than one device
	public:
		Folder *userFolder;
		Device* device;

		ClientUser () {}; // Default constructor
		ClientUser (string userid, Device* device, Folder *userFolder);

		string getUserId();
		Folder* getUserFolder();

		void setUserFolder(Folder* userFolder);
		void sync();
		bool isSynchronized();
};
