#include "../headers/serverUser.hpp"
#include <string>
#include <iostream>

using namespace std;

ServerUser::ServerUser(string userid, Folder *folderPath) {
	this->userid = userid;
	this->folderPath = folderPath;
}
