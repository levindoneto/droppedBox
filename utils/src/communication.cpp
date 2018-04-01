#include <unistd.h> // For closing socket by its descriptor
#include <sys/socket.h>

#include "../headers/communication.hpp"
#include "../headers/dropboxUtils.hpp"

// Destructor ~
Communication::~Communication() {
	close(this->scktDesc);
}

Communication::Communication() {
	this->scktDesc = UNDEF;
	this->port = UNDEF;
}

Communication::Communication(int scktDesc) {
	this->scktDesc = scktDesc;
	this->port = UNDEF;
}

// Return the descriptor of the socket which is open at the current communication
int Communication::getSocketDesc() {
	return this->scktDesc;
}