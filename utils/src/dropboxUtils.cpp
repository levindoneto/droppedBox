#include <iostream>
#include <string>

using namespace std;

void error (string errorMessage) {
  perror(errorMessage);
  exit(1);
}
