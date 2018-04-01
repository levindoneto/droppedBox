#include <iostream>
#include "../headers/clientUser.hpp"

using namespace std;

ClientUser::ClientUser(string userid, Device* device) {
	this->userid = userid;
	this->device = device;
}
