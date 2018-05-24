#include "../headers/clientCommunication.hpp"

#define stat _stat
#define TIME_DO_NOTHING 6
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

ClientCommunication::~ClientCommunication() {
  delete processComm;
}

ClientCommunication::ClientCommunication(Process *_processComm) {
  this->processComm = new Process(_processComm->idUser);
  this->processComm->sock = _processComm->sock->get_answerer();
  this->processComm->login();
  this->start();
}

void *ClientCommunication::run() {
  while (true) {
    processComm->send(Data::T_SYNC);
    processComm->rcvConfirmation();

    list<string> listFiles = File::listNamesOfFiles(processComm->folderOfTheUser);

    for (list<string>::iterator it = listFiles.begin(); it != listFiles.end(); ++it) {
      string idArq = *it;
      string filePath = processComm->folderOfTheUser + '/' + idArq;
      if (allowSending(idArq)) {
        int timeStamp = obtainTSofFile(filePath);
        if (timeStamp != -1) {
          string content = to_string(timeStamp) + '*' + idArq;
          processComm->send(Data::T_STAT, content);
          processComm->rcvConfirmation();
          list<string> expectedTypes;
          expectedTypes.push_back(Data::T_DOWNLOAD);
          expectedTypes.push_back(Data::T_UPLOAD);
          expectedTypes.push_back(Data::T_EQUAL);
          expectedTypes.push_back(Data::T_ERROR);

          Data message = processComm->receive(expectedTypes);

          if (message.type == Data::T_DOWNLOAD) {
            processComm->sendConfirmation();
            if (processComm->getArq(filePath) == 0)
              cout << "File received" << '\n';
            else {
              char error[ERROR_MSG_SIZE] = "Download failed";
              throwError(error);
            }
          }
          else if (message.type == Data::T_UPLOAD) {
            processComm->sendConfirmation();
            try {
              if (!ifstream(filePath)) {
                char error[ERROR_MSG_SIZE] = "Error on opening file";
                throwError(error);
                processComm->sendConfirmation(false);
                processComm->rcvConfirmation();
                unlock_file(idArq);
                continue;
              }

              int timeStamp = obtainTSofFile(filePath);

              processComm->send(Data::T_SOF, to_string(timeStamp));
              processComm->rcvConfirmation();

              if (processComm->sendArq(filePath) != 0) {
                char error[ERROR_MSG_SIZE] = "Error sending file";
                throwError(error);
              }
            }
            catch (exception &e) {
              cout << e.what() << endl;

              processComm->sendConfirmation(false);
              processComm->rcvConfirmation();
              unlock_file(idArq);
              continue;
            }
          }
          else if (message.type == Data::T_EQUAL) {
            processComm->sendConfirmation();
            unlock_file(idArq);
            continue;
          } else {
            processComm->sendConfirmation();
            unlock_file(idArq);
            continue;
          }
        } else {
          char error[ERROR_MSG_SIZE] = "Error on generating times";
          throwError(error);
          unlock_file(idArq);
          continue;
        }
        unlock_file(idArq);
      }
    }
    processComm->send(Data::T_DONE);
    processComm->rcvConfirmation();

    list<string> expectedTypes;
    expectedTypes.push_back(Data::T_DONE);
    expectedTypes.push_back(Data::T_DOWNLOAD);
    expectedTypes.push_back(Data::T_ERROR);

    // Receives for files that client doesn't have yet
    while (true) {
      Data message = processComm->receive(expectedTypes);
      if (message.type == Data::T_DONE) {
        processComm->sendConfirmation();
        break;
      }
      else if (message.type == Data::T_DOWNLOAD) {
        string idArq = message.content;
        string filePath = processComm->folderOfTheUser + '/' + idArq;
        if (allowSending(idArq)) {
          processComm->sendConfirmation();
          if (processComm->getArq(filePath) != 0) {
            char error[ERROR_MSG_SIZE] = "Error downloading files";
            throwError(error);
          }
          unlock_file(idArq);
        }
        else {
          processComm->sendConfirmation(false);
          processComm->rcvConfirmation();
          continue;
        }
      }
      else {
        char error[ERROR_MSG_SIZE] = "Error file not founded";
        throwError(error);
        processComm->sendConfirmation();
        continue;
      }
    }
    sleep(TIME_DO_NOTHING);
  }
}
