#include "../headers/serverUser.hpp"
#include <string>
#include <iostream>

using namespace std;

ServerUser::ServerUser(string userid, Folder *folderPath) {
	this->userid = userid;
	this->folderPath = folderPath;
}

ServerUser::ServerUser(string userid, Folder *folderPath, Device* device) {
	this->userid = userid;
	this->folderPath = folderPath;
	this->addDevice(device);
}

string ServerUser::getUserId() {
	return this->userid;
}

Folder* ServerUser::getUserFolder() {
	return this->folderPath;
}

// Return true if the vector of devices does not contain any element
bool ServerUser::thereAreNoDevices() {
	return this->devices.empty();
}

void ServerUser::delDevice(Device* device) {
	// TODO Delete a device from the devices vector
}

// Add a new element of Device at the end of the vector of devices
void ServerUser::addDevice(Device* device) {
	this->devices.push_back(device);
}
