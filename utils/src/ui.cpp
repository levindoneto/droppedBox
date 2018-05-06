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
    << "  upload <path/filename.ext>" << endl
    << "  download <filename.ext>" << endl
    << "  list_server" << endl
    << "  list_client" << endl
    << "  get_sync_directory" << endl
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
    string token = input.substr(previous, position-previous);
    if (!token.empty()) commandWithParameter.push_back(token);
      previous = position + separator.length();
  } while (position < input.length() && previous < input.length());
  return commandWithParameter;
}


vector<string> getUserCommand() {
  vector<string> commandWithParameter;
  string wholeCommand;
  cout << PREFIX << " ";
  getline(cin, wholeCommand);
  commandWithParameter = parseUserCommand(wholeCommand, " ");
  return commandWithParameter;
}
