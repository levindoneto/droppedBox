#include <string>
#include <mutex>
#include "../../utils/device.hpp"

using namespace std;

class ServerUser {
	private:
		string userid;
        vector<Device*> devices; // Vector of devices
	public:
		ServerUser () {}; // Default constructor
};
