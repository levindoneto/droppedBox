#ifndef DATA_H
#define DATA_H

#include "dropboxUtils.h"

#define HEADER_SEPARATOR '|'

class Data {
  public:
    Data() {};
    Data(string session, int sequence, string type, string content);
    string to_string();
    void print(char direction = '\0', string idUser = "");
    bool pedindo();
    string stringify();

    static Data parse(string msg);
    static Data parseb(char *msg);

    string type;
    string session;
    int sequence;
    string content;

    static const string MSG_SEPARATOR;
    static const string MSG_END;
    static const string T_BYE;
    static const string T_FILE;
    static const string T_END;
    static const string T_SOF;
    static const string T_EOF;
    static const string T_SYN;
    static const string T_ACK;
    static const string T_LS;
    static const string T_DOWNLOAD;
    static const string T_ERROR;
    static const string T_SYNC;
    static const string T_STAT;
    static const string T_DONE;
    static const string T_EQUAL;
    static const string T_UPLOAD;
    static const string T_DELETE;

    // Backup stuff
    static const string T_CLIENT_CONNECT;
    static const string T_CLIENT_DISCONNECT;
    static const string T_SERVER_ALIVE;

    // Multiserver stuff
    static const string T_WORKING;
    static const string T_SERVER_ELECTION;
    static const string T_COORDINATOR;
    static const string T_LOGIN;
    static const string T_BACKUP;
    static const string T_OK;
    static const string T_MODTIME;
};

#endif
