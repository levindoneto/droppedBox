#include "../headers/serverUser.hpp"
#include "../headers/serverCommunication.hpp"

ServerUser::ServerUser(Process *processComm) {
  usingActive = true;
  this->processComm = processComm;
}

ServerUser::~ServerUser() {
  delete processComm;
}

void *ServerUser::run() {
  processComm->accept_processComm();
  ServerCommunication server_sync(processComm);
  server_sync.start();
  cout << processComm->idUser << " logged in" << endl;
  while (true) {
    Data request = processComm->receive_request();
    if (request.type == Data::T_LS) {
      processComm->send_string(processComm->list_server_dir(processComm->folderOfTheUser));
      processComm->receive_ack();
    }
    else if (request.type == Data::T_UPLOAD) {
      string nameOfTheFile = request.content;
      string pathOfTheFile = processComm->folderOfTheUser + '/' + nameOfTheFile;
      if (!allowSending(request.content)) {
        processComm->send_ack(false);
        processComm->receive_ack();
        continue;
      }
      processComm->send_ack();
      processComm->getArq(pathOfTheFile);
      unlock_file(nameOfTheFile);
    }
    else if (request.type == Data::T_DOWNLOAD) {
      string nameOfTheFile = request.content;
      string pathOfTheFile = processComm->folderOfTheUser + '/' + nameOfTheFile;
      if (!allowSending(nameOfTheFile)) {
        processComm->send_ack(false);
        processComm->receive_ack();
        continue;
      }
      processComm->send_ack();
      try {
        if (!ifstream(pathOfTheFile)) {
          char error[ERROR_MSG_SIZE] = "Error opening file";
          throwError(error);

          processComm->send_ack(false);
          processComm->receive_ack();
          unlock_file(nameOfTheFile);
          continue;
        }
        int timeStamp = obtainTSofFile(pathOfTheFile);

        processComm->send(Data::T_SOF, to_string(timeStamp));
        processComm->receive_ack();

        processComm->sendArq(pathOfTheFile);
        unlock_file(nameOfTheFile);
      }
      catch (exception &e) {
        processComm->send_ack(false);
        processComm->receive_ack();

        cout << e.what() << endl;
        unlock_file(nameOfTheFile);
        continue;
      }
      unlock_file(nameOfTheFile);
    }
    else if (request.type == Data::T_BYE) {
      processComm->send_ack();
      processComm->receive_ack();
      break;
    }
  }
  cout << "User " << processComm->idUser << " logged out." << endl;
  usingActive = false;
  return NULL;
}
