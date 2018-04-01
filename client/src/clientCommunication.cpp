#include <sys/socket.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../headers/clientCommunication.hpp"

#define TRUE 1
using namespace std;

ClientCommunication::ClientCommunication(void) {
    /* According to the C standard, the address of a structure and its first
    member are the same, so you can cast the pointer to sockaddr_in(6) in a
    pointer to sockaddr. (source: https://stackoverflow.com/questions/18609397)*/
    struct sockaddr_in clientAddress;
    this->port = 0;

	if ((this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "<Client Communication>: Error for opening the socket" << endl;
		exit(TRUE);
	}
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddress.sin_port = this->port;

	if ((bind(this->socketDescriptor, (struct sockaddr *) &clientAddress, sizeof(clientAddress))) != 0) {
        cout << "<Client Communication>: Error for binding the socket"
            << "at the port " << this->port << endl;
		exit(TRUE);
	}
}

ClientCommunication::ClientCommunication(int port) {
    this->port = 0;
    // TODO
}

bool ClientCommunication::connectServer(string ip, int port) {
    // TODO
    cout << "Connecting to the server with ip = " << ip
        << " and port = " << port << endl;
    return true;
}