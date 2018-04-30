#include <string>
#include "../headers/file.hpp"

using namespace std;

File::File(string fileName, string whenAccessed, string whenChanged, string whenCreated) {
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

string File::getWhenChanged() {
  return this->whenAccessed;
}

string File::getWhenAccessed() {
  return this->whenChanged;
}

string File::getWhenCreated() {
  return this->whenCreated;
}
