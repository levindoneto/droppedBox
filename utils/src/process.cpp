#include "../headers/process.hpp"
#include "../headers/dropboxUtils.h"

#include <utime.h>
#include <stdio.h>

Process::~Process() {}

Process::Process(string idUser, string session, UDPUtils *sock) {
  this->idUser = idUser;
  this->session = session;
  if (session.empty()) {
    this->session = to_string(rand() % 10000);
  }
  this->sock = sock;
  init_sequences();
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
  folderOfTheUser = HOME + "/sync_dir_" + idUser;
  File *file = new File();
  file->createFolderForFiles(folderOfTheUser);
}

void Process::initProcessComm() {
  theLastPartRCV = 0;
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

int Process::sendArq(string filepath) {
  ifstream file(filepath, std::ifstream::binary);
  char chunk[DATAGRAM_LEN];
  do {
    file.read(chunk, DATAGRAM_LEN);
    send(Data::T_FILE, string(chunk, file.gcount()));
    rcvConfirmation();
  } while (!file.eof());
  send(Data::T_EOF);
  rcvConfirmation();
  return 0;
}

void Process::sendText(string data) {
  string chunk;
  int pos = DATAGRAM_LEN;
  do {
    chunk = data.substr(0, pos);
    send(Data::T_LS, chunk);
    rcvConfirmation();
    data.erase(0, pos);
  } while (data.length() > 0);
  send(Data::T_EOF);
  rcvConfirmation();
}

void Process::resend() {
  for (map<int, string>::iterator it = messages_sent.begin();
    it != messages_sent.end();
    it++
  ) {
    //printf("manda msg again\n");
    sock->send(string(it->second));
  }
}

bool Process::rcvConfirmation() {
  //printf("awaiting ack da ultima sequencia\n");
  while (true) {
    Data msg = receive();
    {
      // ERRO DO DELETE TODO
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
      erro = 1;
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
        erro = 1;
        //printf("error on type recebido\n");
      }
    }
  }
}

Data Process::receive_request() {
  this->sock->set_timeout(INIT); // Never timeout
  while (true) {
    Data msg = receive();
    if (msg.pedindo())
    {
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

int Process::getArq(string filepath) {
  int longTime;
  ofstream file;
  while (true) {
    Data msg = receive();
    {
      if (msg.type == Data::T_FILE)
      {
        theLastPartRCV = msg.sequence;
        file.write(msg.content.data(), msg.content.length());
        sendConfirmation();
      } else if (msg.type == Data::T_SOF) {
        theLastPartRCV = msg.sequence;
        longTime = stoi(msg.content);
        file.open(filepath, ofstream::binary | ofstream::trunc);

        sendConfirmation();
      } else if (msg.type == Data::T_EOF) {
        theLastPartRCV = msg.sequence;
        sendConfirmation();
        file.close();

        // Sets modification time
        struct utimbuf ubuf;
        ubuf.modtime = longTime;
        struct stat info;
        stat(filepath.c_str(), &info);
        if (utime(filepath.c_str(), &ubuf) != EQUAL) {
          throwError("error de tempo");
        } else {
          stat(filepath.c_str(), &info);
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

void Process::init_sequences() {
  theLastPartS = ERROR;
  theLastPartRCV = ERROR;
}

string Process::list_server_dir(string dirpath) {
  return File::ll(dirpath);
}

int Process::deleteFile(string filepath) {
  const char *cstr = filepath.c_str();
  int status = remove(cstr);
  if (status != OK) {
    return ERROR;
  }
  return 0;
}
