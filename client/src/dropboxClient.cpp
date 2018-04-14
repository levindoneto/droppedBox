#include <iostream>
#include "../headers/dropboxClient.hpp"
#include "../headers/clientUser.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    cout << "Client" << endl;

    // Test client communication
    ClientCommunication* c = new ClientCommunication();
    cout << "Attempt to connect the user " << argv[1] << endl;
    c->connectServer(argv[2], argv[3]);
    return 0;
}
