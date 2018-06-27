#ifndef PROCESS_H
#define PROCESS_H

#include "udpUtils.hpp"
#include "data.hpp"
#include "dropboxUtils.h"
#include "../fileSystem/headers/file.hpp"
#include "../../server/headers/dropboxServer.hpp"

class Process {
  private:
    // Attr
    map<int, string> messages_sent;
    int theLastPartS;
    int theLastPartRCV;
    // Methods
    Data receive();
    void init_sequences();

  public:
    // Attr
    string idUser;
    string folderOfTheUser;
    string session;
    UDPUtils *sock;
    string hostname;
    // Methods
    Process(string idUser, string session = "", UDPUtils *new_socket = NULL);
    ~Process();
    void closeProcess();
    Process(string hostname, int port);
    Process();
    Process(int port);
    Process(string session, UDPUtils* sock);
    Process* createProcComm();
    Process *rcvProcComm();
    void connectProc();
    void confirmComm();
    void confirmRcv(Data dataMessage);
    void login();
    void initProcessComm();
    void send(string type, string content = "");
    void sendb(string stype, char *content = NULL);
    void sendConfirmation(bool workedProperly = true);
    int sendArq(string filepath);
    void sendText(string data);
    void resend();
    static Process *listener(int port);
    Data receive(string expected_type);
    Data receive(list<string> expected_types);
    Data receive_request();
    bool rcvConfirmation();
    int getArq(string filepath);
    string receive_string();
    string list_server_dir(string dirpath);
    int deleteFile(string filepath);
    static void *server_thread(void *void_this);
};

#endif
