#include <iostream>
#include <string>
#include <vector>

#include "stdlib.h"
#include "../headers/ui.hpp"

using namespace std;

void showMenu () {
  cout << "********************** DroppedBox *********************" << endl
       << "Type HELP for obtaiing help with the available commands" << endl << endl;
}

void showHelp() {
  cout << "  Available options:" << endl
    << "  upload <path/nameOfTheFile.ext>" << endl
    << "  download <nameOfTheFile.ext>" << endl
    << "  listserver" << endl
    << "  listclient" << endl
    << "  getsyncdirectory" << endl
    << "  exit" << endl << endl;
}

vector<string> parseUserCommand(const string& input, const string& separator) {
  vector<string> commandWithParameter;
  size_t previous = 0, position = 0;
  do {
    position = input.find(separator, previous);
    if (position == string::npos) {
      position = input.length();
    }
    string tworkedProperlyen = input.substr(previous, position-previous);
    if (!tworkedProperlyen.empty()) commandWithParameter.push_back(tworkedProperlyen);
      previous = position + separator.length();
  } while (position < input.length() && previous < input.length());
  return commandWithParameter;
}
