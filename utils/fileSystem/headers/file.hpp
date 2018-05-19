#include <string>

#define EMPTY_FILENAME ""
#define UNDEF_TIME 0

using namespace std;

class File {
  private:
    string fileName;
    string whenAccessed;
    string whenChanged;
    string whenCreated;
  public:
    //~File();
    File();
    File(string fileName);
    File(string fileName, string whenAccessed, string whenChanged, string whenCreated);
    string getFileName();
    string getWhenChanged();
    string getWhenAccessed();
    string getWhenCreated();
    bool fileExists (char *filename)
};
