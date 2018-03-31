#include <mutex>
#define CLOSE 0 // Closing state
#define EXEC 1 // Executing state

using namespace std;

class Device {
	private:
		mutex accessDev;
		int state; // Executing, closing, ...

	public:
		Device () {};
};
