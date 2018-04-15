#include <iostream>
#include <string>
#include "string.h"
#include "../headers/dropboxClient.hpp"
#include "../headers/clientUser.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"

using namespace std;

int main (int argc, char **argv) {
    cout << "Client" << endl;
    int port = atoi(argv[2]);
    // Test client communication
    ClientCommunication* c = new ClientCommunication();
    c->connectServer(argv[1], port);
    return 0;
}
