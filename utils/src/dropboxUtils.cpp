#include <iostream>
#include <string>

#include "../headers/dropboxUtils.hpp"

using namespace std;

void throwError (char* errorMessage) {
  perror(errorMessage);
  exit(1);
}
