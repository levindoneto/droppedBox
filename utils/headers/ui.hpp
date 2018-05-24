#include <string>
#include <vector>

#define PREFIX_BASH "\n$ "

using namespace std;

void showMenu ();
void showHelp();
vector<string> parseUserCommand(const string& input, const string& separator);
