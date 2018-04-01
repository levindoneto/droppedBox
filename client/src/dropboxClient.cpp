#include <iostream>
#include "../headers/dropboxClient.hpp"
#include "../headers/clientUser.hpp"
#include "../headers/clientCommunication.hpp"
#include "../../settings/config.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    cout << "Client" << endl;
    string userid = "Taschetto";

    // Test client communication
    ClientCommunication* c = new ClientCommunication();
    cout << "Attempt to connect the user " << userid << endl;
    c->connectServer(HOST, PORT);
    return 0;
}
