#include "../headers/file.hpp"
#include "../../headers/dropboxUtils.hpp"
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

bool fileExists (char *filename) {
  struct stat buffer;
  return (stat (filename, &buffer) == EQUAL);
}
