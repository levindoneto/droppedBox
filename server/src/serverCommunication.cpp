#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/dropboxUtils.hpp"

ServerCommunication::ServerCommunication() {
	this->scktDesc = INIT;
	this->port = INIT;
}