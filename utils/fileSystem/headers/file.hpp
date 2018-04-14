#include <string>

#define EMPTY_FILENAME ""
#define UNDEF_TIME 0

using namespace std;

class File {
	private:
		string fileName;
		long whenAccessed;
		long whenChanged;
		long whenCreated;
	public:
		//~File();
		File();
		File(string fileName);
		File(string fileName, long whenAccessed, long whenChanged, long whenCreated);
		string getFileName();
		long getWhenChanged();
		long getWhenAccessed();
		long getWhenCreated();
};