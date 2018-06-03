#include "../headers/serverUser.hpp"
#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/ui.hpp"

ServerUser::ServerUser(Process *processComm) {
  usingActive = true;
  this->processComm = processComm;
}

ServerUser::~ServerUser() {
  delete processComm;
}

void *ServerUser::run() {
  processComm->initProcessComm();
  ServerCommunication server_sync(processComm);
  server_sync.start();
  cout << processComm->idUser << " logged in" << endl;
  while (true) {
    Data request = processComm->receive_request();
    if (request.type == Data::T_LS) {
      processComm->sendText(processComm->list_server_dir(processComm->folderOfTheUser));
      processComm->rcvConfirmation();
    }
    else if (request.type == Data::T_UPLOAD) {
      string content = request.content;
      string nameOfTheFile = parsePath(content, "/").back();
      string pathOfTheFile = processComm->folderOfTheUser + '/' + nameOfTheFile;
      if (!allowSending(request.content)) {
        processComm->sendConfirmation(false);
        processComm->rcvConfirmation();
        continue;
      }
      processComm->sendConfirmation();
      processComm->getArq(pathOfTheFile);
      unlock_file(pathOfTheFile);
    }
    else if (request.type == Data::T_DOWNLOAD) {
      string nameOfTheFile = request.content;
      string pathOfTheFile = processComm->folderOfTheUser + '/' + nameOfTheFile;
      if (!allowSending(nameOfTheFile)) {
        processComm->sendConfirmation(false);
        processComm->rcvConfirmation();
        continue;
      }
      processComm->sendConfirmation();
      try {
        if (!ifstream(pathOfTheFile)) {
          char error[ERROR_MSG_SIZE] = "Error opening file";
          throwError(error);

          processComm->sendConfirmation(false);
          processComm->rcvConfirmation();
          unlock_file(nameOfTheFile);
          continue;
        }
        int timeStamp = obtainTSofFile(pathOfTheFile);

        processComm->send(Data::T_SOF, to_string(timeStamp));
        processComm->rcvConfirmation();

        processComm->sendArq(pathOfTheFile);
        unlock_file(nameOfTheFile);
      }
      catch (exception &e) {
        processComm->sendConfirmation(false);
        processComm->rcvConfirmation();

        cout << e.what() << endl;
        unlock_file(nameOfTheFile);
        continue;
      }
      unlock_file(nameOfTheFile);
    }
    else if (request.type == Data::T_BYE) {
      processComm->sendConfirmation();
      processComm->rcvConfirmation();
      break;
    }
  }
  cout << "User " << processComm->idUser << " logged out." << endl;
  usingActive = false;
  return NULL;
}
