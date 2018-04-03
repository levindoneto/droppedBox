#include <string>
#include "../headers/file.hpp"

using namespace std;

File::File() {
	this->fileName = EMPTY_FILENAME;
	this->whenAccessed = UNDEF_TIME;
	this->whenChanged = UNDEF_TIME;
	this->whenCreated = UNDEF_TIME;
}

File::File(string fileName) {
	this->fileName = fileName;
	this->whenAccessed = UNDEF_TIME;
	this->whenChanged = UNDEF_TIME;
	this->whenCreated = UNDEF_TIME;
}


File::File(string fileName, long whenAccessed, long whenChanged, long whenCreated) {
	// TODO: Handle folder for getting its path as a string if needed here
	// Warning: redefinition because of ClientCommunication
	this->fileName = fileName;
	this->whenAccessed = whenAccessed;
	this->whenChanged = whenChanged;
	this->whenCreated = whenCreated;
}

string File::getFileName() {
	return this->fileName;
}

long File::getWhenChanged() {
	return this->whenAccessed;
}

long File::getWhenAccessed() {
	return this->whenChanged;
}

long File::getWhenCreated() {
	return this->whenCreated;
}
