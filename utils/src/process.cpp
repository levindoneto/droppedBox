#include "../headers/process.hpp"
#include "../headers/dropboxUtils.h"
#include <string>
#include <list>
#include <utime.h>
#include <stdio.h>
using namespace std;
Process::~Process() {}

Process::Process(string idUser, string session, UDPUtils *sock) {
  this->idUser = idUser;
  this->session = session;
  if (session.empty()) {
    this->session = to_string(rand() % RANDOM_ID_SESSION_SIZE);
  }
  this->sock = sock;
  init_sequences();
}

Process::Process(string hostname, int port) : Process() {
  this->sock = new UDPUtils(port);
  this->sock->setIp(hostname);
  connectProc();
}

Process::Process() {
    this->session = to_string(rand() % RANDOM_ID_SESSION_SIZE);
    theLastPartS = ERROR;
    theLastPartRCV = ERROR;
}

Process::Process(int port) {
  sock = new UDPUtils(port);
  sock->bindServer();
}

Process::Process(string session, UDPUtils* sock) : Process() {
    this->session = session;
    this->sock = sock;
}

Process* Process::createProcComm() {
    Process* newProcComm = new Process();
    newProcComm-> sock =  sock->get_answerer();
    newProcComm->connectProc();
    return newProcComm;
}

Process *Process::rcvProcComm() {
  Data *data = new Data();
  sock->turnOffTimeout();
  while (true) {
    string dataString = this-> sock->receive();
    Data dataMessage = data->parse(dataString);
    if (dataMessage.type == Data::T_SYN && dataMessage.session != this->session) {
      sock->turnOnTimeout();
/*
      if (DropboxServer::backupServers.count(dataMessage.content) <= INIT) {
        send(Data::T_NEW_USER, dataMessage.content);
      }
*/
      return new Process(dataMessage.session, sock->get_answerer());
    }
  }
}

void Process::connectProc() {
  send(Data::T_SYN);
  sock->set_to_answer( sock);
}

void Process::confirmComm() {
  theLastPartRCV = INIT;
  sendConfirmation();
}

void Process::confirmRcv(Data dataMessage) {
  theLastPartRCV = dataMessage.sequence;
  sendConfirmation();
}

void Process::closeProcess() {
  printf("Closing process of the user %s.\n", this->idUser);
  delete this;
}

void Process::login() {
  send(Data::T_SYN, idUser);
  rcvConfirmation();
  sock->set_dest_address(sock->get_sender_address());
  sendConfirmation();
  folderOfTheUser = HOME + SYNC_DIR_PREFIX + idUser;
  File *file = new File();
  file->createFolderForFiles(folderOfTheUser);
}

void Process::initProcessComm() {
  theLastPartRCV = INIT;
  sendConfirmation();
  rcvConfirmation();
  folderOfTheUser = idUser;
  File *file = new File();
  file->createFolderForFiles(folderOfTheUser);
}

void Process::send(string type, string content) {
  theLastPartS += 1;
  Data msg = Data(session, theLastPartS, type, content);
  sock->send(msg.to_string());
  messages_sent[theLastPartS] = msg.to_string();
}

void Process::sendConfirmation(bool workedProperly) {
  if (workedProperly) {
    send(Data::T_ACK, to_string(theLastPartRCV));
  }
  else {
    send(Data::T_ERROR, to_string(theLastPartRCV));
  }
}

int Process::sendArq(string pathOfTheFile) {
  ifstream file(pathOfTheFile, std::ifstream::binary);
  char chunk[DATAGRAM_LEN];
  do {
    file.read(chunk, DATAGRAM_LEN);
    send(Data::T_FILE, string(chunk, file.gcount()));
    rcvConfirmation();
  } while (!file.eof());
  send(Data::T_EOF);
  rcvConfirmation();
  return INIT;
}

void Process::sendText(string data) {
  string chunk;
  int pos = DATAGRAM_LEN;
  do {
    chunk = data.substr(INIT, pos);
    send(Data::T_LS, chunk);
    rcvConfirmation();
    data.erase(INIT, pos);
  } while (data.length() > INIT);
  send(Data::T_EOF);
  rcvConfirmation();
}

void Process::resend() {
  for (map<int, string>::iterator it = messages_sent.begin();
    it != messages_sent.end();
    it++
  ) {
    sock->send(string(it->second));
  }
}

bool Process::rcvConfirmation() {
  //printf("awaiting ack da ultima sequencia\n");
  while (true) {
    Data msg = receive();
    {
      if (stoi(msg.content) == theLastPartS) {
        if (msg.type == Data::T_ACK) {
          theLastPartRCV = msg.sequence;
          messages_sent.clear();
          return true;
        } else if (msg.type == Data::T_ERROR) {
            theLastPartRCV = msg.sequence;
            messages_sent.clear();
            return false;
        }
      }

    }
  }
}

Data Process::receive(string expected_type) {
  //printf("awaiting por tipo::: %s\n", expected_type);
  int erro = INIT;
  while (true) {
    Data msg = receive();
    if (msg.type == expected_type) {
      theLastPartRCV = msg.sequence;
      return msg;
    } else {
      erro = TRUE;
      //printf("error on type recebido\n");
    }
  }
}

Data Process::receive(list<string> expected_types) {
  int erro = INIT;
  while (true) {
    Data msg = receive();
    for (list<string>::iterator it = expected_types.begin();
      it != expected_types.end();
      it++
    ) {
      if (msg.type == *it) {
        theLastPartRCV = msg.sequence;
        return msg;
      } else {
        erro = TRUE;
        //printf("error on type recebido\n");
      }
    }
  }
}

string Process::receive_content(string expected_type) {
  Data dataMessage = receive(expected_type);
  string contentReceived = dataMessage.content;
  return contentReceived;
}


Data Process::receive_request() {
  this->sock->set_timeout(INIT); // Never timeout
  while (true) {
    Data msg = receive();
    if (msg.pedindo()) {
      theLastPartRCV = msg.sequence;
      this->sock->set_timeout(TIMEO);
      return msg;
    }
  }
}

Data Process::receive() {
  int erro = INIT;
  while (true) {
    try {
      Data msg = Data::parse(sock->receive());
      if (msg.session == session) {
        if (msg.sequence == theLastPartRCV + 1) {
          return msg;
        } else {
          resend();
        }
      }
      else {
        printf("Data received from wrong session");
      }
    }
    catch (timeout_exception &e) {
      resend();
    }
    catch (runtime_error &e) {
      cout << e.what() << endl;
    }
  }
}

string Process::receive_string() {
  string received_data;
  while(true)
  {
    Data msg = receive();
    {
      if (msg.type == Data::T_EOF) {
        theLastPartRCV = msg.sequence;
        sendConfirmation();
        return received_data;
      } else if (msg.type == Data::T_LS) {
        theLastPartRCV = msg.sequence;
        received_data += msg.content;
        sendConfirmation();
      }
    }
  }
}

int Process::getArq(string pathOfTheFile) {
  int longTime;
  ofstream file;
  while (true) {
    Data msg = receive();
    {
      if (msg.type == Data::T_FILE) {
        theLastPartRCV = msg.sequence;
        file.write(msg.content.data(), msg.content.length());
        sendConfirmation();
      } else if (msg.type == Data::T_SOF) {
          theLastPartRCV = msg.sequence;
          longTime = stoi(msg.content);
          file.open(pathOfTheFile, ofstream::binary | ofstream::trunc);
          sendConfirmation();
      } else if (msg.type == Data::T_EOF) {
          theLastPartRCV = msg.sequence;
          sendConfirmation();
          file.close();

          // Set modification time
          struct utimbuf ubuf;
          ubuf.modtime = longTime;
          struct stat info;
          stat(pathOfTheFile.c_str(), &info);
          if (utime(pathOfTheFile.c_str(), &ubuf) != EQUAL) {
            throwError("Timing error");
          } else {
            stat(pathOfTheFile.c_str(), &info);
          }
        return EQUAL;
      }
      else if (msg.type == Data::T_ERROR) {
        theLastPartRCV = msg.sequence;
        sendConfirmation();
        return ERROR;
      }
    }
  }
}

void Process::receive_file(string pathOfTheFile) {
  list<string> fileExpextedTypes = {Data::T_END, Data::T_FILE};

  ofstream fileS;

  // ofstream::binary for solving bug with non text files
  fileS.open(pathOfTheFile, ofstream::binary | ofstream::trunc);
  while (true) {
    Data msg = receive(fileExpextedTypes);
    if (msg.type == Data::T_FILE)
      fileS.write(msg.content.data(), msg.content.length());
    else break;
  }
  fileS.close();
  int modificsTime = stoi(receive_content(Data::T_MODTIME));
  File file(pathOfTheFile);
  file.set_modification_time(modificsTime);
}

void Process::init_sequences() {
  theLastPartS = ERROR;
  theLastPartRCV = ERROR;
}

string Process::list_server_dir(string dirpath) {
  return File::ll(dirpath);
}

int Process::deleteFile(string pathOfTheFile) {
  const char *cstr = pathOfTheFile.c_str();
  int status = remove(cstr);
  if (status != OK) {
    return ERROR;
  }
  return INIT;
}
