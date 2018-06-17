#include "../headers/clientCommunication.hpp"
#include "../../utils/headers/dropboxUtils.h"

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
  list<string> previousListOfFiles;
  list<string> currentListOfFiles;
  while (true) {
    processComm->send(Data::T_SYNC);
    processComm->rcvConfirmation();

    list<string> currentListOfFiles = File::listNamesOfFiles(
      processComm->folderOfTheUser
    );

    for (
      list<string>::iterator fname = currentListOfFiles.begin();
      fname != currentListOfFiles.end();
      ++fname
    ) {
      string idArq = *fname;
      string filePath = processComm->folderOfTheUser + '/' + idArq;
      previousListOfFiles.remove(idArq);

      if (allowSending(idArq)) {
        int timeStamp = obtainTSofFile(filePath);
        if (timeStamp != ERROR) {
          string content = to_string(timeStamp) + SEPARATOR_FILENAME + idArq;
          processComm->send(Data::T_STAT, content);
          processComm->rcvConfirmation();
          list<string> expectedTypes;
          expectedTypes.push_back(Data::T_DOWNLOAD);
          expectedTypes.push_back(Data::T_UPLOAD);
          expectedTypes.push_back(Data::T_EQUAL);
          expectedTypes.push_back(Data::T_ERROR);
          expectedTypes.push_back(Data::T_DELETE);

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
    //processComm->send(Data::T_DONE);
    //processComm->rcvConfirmation();

    list<string> expectedTypes;
    expectedTypes.push_back(Data::T_DONE);
    expectedTypes.push_back(Data::T_DOWNLOAD);
    expectedTypes.push_back(Data::T_ERROR);

    // Treat removed files from a client
    if (previousListOfFiles.size() >= 1) {
      for (
        list<string>::iterator fname = previousListOfFiles.begin();
        fname != previousListOfFiles.end();
        ++fname
      ) {
        string nameOfTheFile = *fname;
        processComm->send(Data::T_DELETE, nameOfTheFile);
        processComm->rcvConfirmation();
      }
    }

    // Receive files which the client does not have yet
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

    previousListOfFiles = File::listNamesOfFiles(processComm->folderOfTheUser);
    sleep(TIME_DO_NOTHING);
  }
}
