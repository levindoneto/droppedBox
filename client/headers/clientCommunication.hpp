#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;

class ClientCommunication : public Communication {
    private:
    	int port;
    	int socketDescriptor;

    public:
    	ClientCommunication();
    	ClientCommunication(char* port);
    	bool connectServer(char* ip, char* port);
};
