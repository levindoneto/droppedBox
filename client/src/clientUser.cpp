#include <iostream>
#include "../headers/clientUser.hpp"

using namespace std;

ClientUser::ClientUser(string userId, Device* device) {
	this->userId = userId;
	this->device = device;
}

string ClientUser::getUserId() {
	return this->userId;
}