#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

using namespace std;

#define TRUE 1
#define BUFFER_SIZE 256

ServerCommunication::ServerCommunication(int port) {
	int socketDesc;
  int n;
	socklen_t clilen;
	struct sockaddr_in serverAddress, clientAddress;
	char buffer[BUFFER_SIZE];
	fflush(stdin);
	if ((socketDesc = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	throwError("Error on opening socket");

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serverAddress.sin_zero), 8);

	if (
		bind(
			socketDesc,
			(struct sockaddr *) &serverAddress,
			sizeof(struct sockaddr)
		) < 0
	) {
		throwError("Error on on binding");
	}

	clilen = sizeof(struct sockaddr_in);

	while (TRUE) {
		/* receive from socket */
		n = recvfrom(socketDesc, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientAddress, &clilen);
		if (n < 0) {
			throwError("ERROR on recvfrom");
		}
		cout << "Received a datagram: " << endl << buffer << endl;

		// Send datagram to the created socket
		n = sendto(
			socketDesc,
			"Got your message\n",
			17,
			0,
			(struct sockaddr *) &clientAddress,
			sizeof(struct sockaddr)
		);

		if (n  < 0) {
			throwError("Error on sending datagram to the created socket");
		}
	}
	fflush(stdin);
	close(socketDesc);
}
