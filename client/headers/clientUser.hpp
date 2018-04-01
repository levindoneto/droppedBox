#include <string>
#include "../../utils/headers/device.hpp"

using namespace std;

class ClientUser {
	private:
		string userid;
		Device* device;
	public:
		ClientUser () {}; // Default constructor
		ClientUser (string userid, Device* device);
};
