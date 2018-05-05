#include <string>
#include "../../utils/headers/communication.hpp"

#define BUFFER_SIZE sizeof(datagram)
#define CHUNCK_SIZE 1024

struct Datagrams {
   int  chunckId;
   char  chunck[CHUNCK_SIZE];
} datagram;

class ServerCommunication: public Communication {
  public:
    ServerCommunication(int socketDesc, int port);
    ServerCommunication(int port);
    ServerCommunication();
};
