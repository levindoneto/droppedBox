#include <string>

using namespace std;

int writeToSocket(UserInfo user, int socketDesc, string host, int port);
int readFromSocket(int socketDesc, char* buffer);
int openSocket();
void closeSocket(int socketDesc);
