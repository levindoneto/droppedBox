#include "../headers/data.hpp"
#include <string>

const string Data::MSG_SEPARATOR = "|";

const string Data::T_SYN = "SYN";
const string Data::T_ACK = "ACK";
const string Data::T_LS = "LS";
const string Data::T_DOWNLOAD = "DOWN";
const string Data::T_UPLOAD = "UP";
const string Data::T_BYE = "EXITAPP";
const string Data::T_FILE = "FILE";
const string Data::T_END = "END";
const string Data::T_SOF = "SOF";
const string Data::T_EOF = "EOF";
const string Data::T_ERROR = "ERRO";
const string Data::T_SYNC = "SYNC";
const string Data::T_STAT = "STAT";
const string Data::T_DONE = "DONE";
const string Data::T_EQUAL = "EQUAL";
const string Data::T_DELETE = "DELETE";

// Multiserver stuff
const string Data::T_SERVER_ELECTION = "SERVER_ELECTION";
const string Data::T_WORKING = "WORKING";
const string Data::T_COORDINATOR = "COORDINATOR";
const string Data::T_NEW_USER = "NEW_USER";
const string Data::T_IP = "IP";
const string Data::T_BACKUP = "BACKUP";
const string Data::T_OK = "OK";
const string Data::T_MODTIME = "MODTIME";
const string Data::T_LOGIN = "LOGIN";

bool Data::pedindo() {
  return (
    type == Data::T_LS
    || type == Data::T_DOWNLOAD
    || type == Data::T_UPLOAD
    || type == Data::T_BYE
    || type == Data::T_NEW_USER
  );
}

Data::Data(string session, int sequence, string type, string content = "") {
  this->session = session;
  this->sequence = sequence;
  this->type = type;
  this->content = content;
}

string Data::to_string() {
  string msgData = "";
  msgData += session;
  msgData += Data::MSG_SEPARATOR;
  msgData += std::to_string(sequence);
  msgData += Data::MSG_SEPARATOR;
  msgData += type;
  msgData += Data::MSG_SEPARATOR;
  msgData += content;
  msgData += Data::MSG_SEPARATOR;
  return msgData;
}

Data Data::parse(string msgData) {
  // Find position of separators
  int session_sep = msgData.find('|');
  int sequence_sep = msgData.find('|', session_sep + 1);
  int type_sep = msgData.find('|', sequence_sep + 1);
  int content_sep = msgData.find_last_of('|');
  int session_len = session_sep;
  int sequence_len = sequence_sep - session_sep - 1;
  int type_len = type_sep - sequence_sep - 1;
  int content_len = content_sep - type_sep - 1;
  string session = msgData.substr(INIT, session_len);
  string sequence = msgData.substr(session_sep + 1, sequence_len);
  string type = msgData.substr(sequence_sep + 1, type_len);
  string content = msgData.substr(type_sep + 1, content_len);

  return Data(session, stoi(sequence), type, content);
}

string Data::stringify() {
  string msgData = "";
  msgData += session;
  msgData += Data::MSG_SEPARATOR;
  msgData += std::to_string(sequence);
  msgData += Data::MSG_SEPARATOR;
  msgData += type;
  msgData += Data::MSG_SEPARATOR;
  msgData += content;
  msgData += Data::MSG_SEPARATOR;
  return msgData;
}
