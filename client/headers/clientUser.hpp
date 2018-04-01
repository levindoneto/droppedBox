#include <string>
#include "../../utils/headers/device.hpp"

using namespace std;

class ClientUser {
	private:
		string userId;
		Device* device;
	public:
		ClientUser () {}; // Default constructor
		ClientUser (string userid, Device* device);

		string getUserId();
};
