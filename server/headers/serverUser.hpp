#include <string>
#include <vector>
#include <mutex>
#include "../../utils/headers/device.hpp"

using namespace std;

class ServerUser {
	private:
		string userid;
        vector<Device*> devices; // Vector of devices
	public:
		ServerUser () {}; // Default constructor
		ServerUser (string userid); // TODO: Put devices in this init
};
