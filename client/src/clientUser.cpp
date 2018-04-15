#include <iostream>
#include "../headers/clientUser.hpp"

using namespace std;

ClientUser::ClientUser(string userId, Device *device, Folder *folderPath) {
	this->userId = userId;
	this->device = device;
	this->folderPath = folderPath;
}

string ClientUser::getUserId() {
	return this->userId;
}
