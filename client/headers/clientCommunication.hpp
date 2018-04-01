#include <string>
#include "../../utils/headers/communication.hpp"

using namespace std;

class ClientCommunication { // TODO: extend communication from utils
    private:
    	int port;
    	int socketDescriptor;

    public:
    	ClientCommunication(void);
    	ClientCommunication(int port);
    	bool connectServer(string ip, int port);
};
