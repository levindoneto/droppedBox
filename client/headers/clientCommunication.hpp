#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;

class ClientCommunication : public Communication {
    private:
    	int port;
    	int socketDescriptor;

    public:
    	ClientCommunication();
    	ClientCommunication(int port);
    	bool connectServer(string ip, int port);
};
