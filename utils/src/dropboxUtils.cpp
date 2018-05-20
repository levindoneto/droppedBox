#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include "../headers/dropboxUtils.hpp"

using namespace std;
string clientFolderPath;

void throwError (char* errorMessage) {
  perror(errorMessage);
  exit(TRUE);
}

// TODO: Remake this function to make it OO

void getClientFolderPath(string folderPath) {
  clientFolderPath = folderPath;
}

unsigned int fileSize(string filePath) {
  const char *filePathChar = filePath.c_str();
  FILE * f = fopen(filePathChar, "r");
  fseek(f, 0, SEEK_END);
  unsigned long len = (unsigned long)ftell(f);
  fclose(f);
  return len;
}

Semaphore::Semaphore() {
  if (initialized) {
    sem_destroy(&this->internalSemaphore);
  }
  sem_init(&internalSemaphore, 0, 0);
  initialized = true;
}

void Semaphore::init(int initValue) {
  if (initialized) {
    sem_destroy(&this->internalSemaphore);
  }
  sem_init(&internalSemaphore, 0, initValue);
  initialized = true;
}

Semaphore::~Semaphore(void) {
  int status = sem_destroy(&this->internalSemaphore);
  if (status) {
      std::cout << "[Error] Could not destroy semaphore." << std::endl;
    } else {
    this->initialized = false;
  }
}

int Semaphore::wait() {
  int status = sem_wait(&this->internalSemaphore);
  if (status) {
    std::cout << "[Error] Could not destroy semaphore." << std::endl;
  }
  return status;
}

int Semaphore::post() {
  int status = sem_post(&this->internalSemaphore);
  if (status) {
    std::cout << "[Error] Could not post semaphore." << std::endl;
  }
  return status;
}
