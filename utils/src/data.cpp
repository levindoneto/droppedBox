#include "../headers/data.hpp"

const string Data::MSG_SEPARATOR = "|";

const string Data::T_SYN = "SYN";
const string Data::T_ACK = "ACK";
const string Data::T_LS = "LS";
const string Data::T_DOWNLOAD = "DOWN";
const string Data::T_UPLOAD = "UP";
const string Data::T_BYE = "BYE";
const string Data::T_FILE = "FILE";
const string Data::T_SOF = "SOF";
const string Data::T_EOF = "EOF";
const string Data::T_ERROR = "ERRO";
const string Data::T_SYNC = "SYNC";
const string Data::T_STAT = "STAT";
const string Data::T_DONE = "DONE";
const string Data::T_EQUAL = "EQAL";

bool Data::pedindo() {
    return (type == Data::T_LS || type == Data::T_DOWNLOAD || type == Data::T_UPLOAD || type == Data::T_BYE);
}

Data::Data(string session, int sequence, string type, string content = "") {
    this->session = session;
    this->sequence = sequence;
    this->type = type;
    this->content = content;
}

string Data::to_string() {
    string msg = "";
    msg += session;
    msg += Data::MSG_SEPARATOR;
    msg += std::to_string(sequence);
    msg += Data::MSG_SEPARATOR;
    msg += type;
    msg += Data::MSG_SEPARATOR;
    msg += content;
    msg += Data::MSG_SEPARATOR;
    return msg;
}

Data Data::parse(string msg) {
    // Find position of separators
    int session_sep = msg.find('|');
    int sequence_sep = msg.find('|', session_sep + 1);
    int type_sep = msg.find('|', sequence_sep + 1);
    int content_sep = msg.find_last_of('|');

    int session_len = session_sep;
    int sequence_len = sequence_sep - session_sep - 1;
    int type_len = type_sep - sequence_sep - 1;
    int content_len = content_sep - type_sep - 1;

    string session = msg.substr(INIT, session_len);
    string sequence = msg.substr(session_sep + 1, sequence_len);
    string type = msg.substr(sequence_sep + 1, type_len);
    string content = msg.substr(type_sep + 1, content_len);

    return Data(session, stoi(sequence), type, content);
}
