#ifndef UDPUTILS_H
#define UDPUTILS_H
#include "dropboxUtils.h"

#define SOCKET_BUFFER_SIZE 1200
#define DATAGRAM_LEN 1024

class UDPUtils {
  private:
    int total_send;
    int total_received;
    int id;
    int port;
    struct sockaddr_in dest_address;
    struct sockaddr_in server_address;
    struct sockaddr_in sender_address;
    struct hostent *host;
    char receive_buffer[SOCKET_BUFFER_SIZE];
    const char *send_buffer;
    struct timeval timeout;
    socklen_t socklen;
    struct timeval zero_timeout;
  public:
    UDPUtils(int port);
    ~UDPUtils();
    void bindServer();
    void setIp(string hostname);
    void send(string msg);
    void sendb(char *content);
    string receive();
    sockaddr_in get_sender_address();
    void set_dest_address(sockaddr_in new_dest_address);
    void turnOnTimeout();
    void turnOffTimeout();
    void set_timeout(int seconds);
    void set_to_answer(UDPUtils* sock);
    UDPUtils* get_answerer();
    static int DEFAULT_TIMEOUT;
};

#endif
