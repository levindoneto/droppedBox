#include "../headers/serverUser.hpp"
#include <iostream>

using namespace std;

ServerUser::ServerUser(string userid, Folder *folderPath) {
	this->userid = userid;
	this->folderPath = folderPath;
}
