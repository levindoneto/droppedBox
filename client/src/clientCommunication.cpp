#include "../headers/clientCommunication.hpp"
#include "../../utils/headers/dropboxUtils.h"

#define stat _stat
#define TIME_DO_NOTHING 6 // Sync thread (merging files)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

ClientCommunication::~ClientCommunication() {
  delete process;
}

ClientCommunication::ClientCommunication(DropboxClient *client) : Thread() {
    this->client = client;
    filenames = File::listNamesOfFiles(client->folderOfTheUser);
}

void *ClientCommunication::run() {
  list<string> previousListOfFiles;
  list<string> currentListOfFiles;
  while (true) {
    process->send(Data::T_SYNC);
    process->rcvConfirmation();
    list<string> currentListOfFiles = File::listNamesOfFiles(
      process->folderOfTheUser
    );

    for (
      list<string>::iterator fname = currentListOfFiles.begin();
      fname != currentListOfFiles.end();
      ++fname
    ) {
      string idArq = *fname;
      string filePath = process->folderOfTheUser + PATH_SEPARATOR + idArq;
      previousListOfFiles.remove(idArq);

      if (allowSending(idArq)) {
        int timeStamp = obtainTSofFile(filePath);
        if (timeStamp != ERROR) {
          string content = to_string(timeStamp) + SEPARATOR_FILENAME + idArq;
          process->send(Data::T_STAT, content);
          process->rcvConfirmation();
          list<string> expectedTypes;
          expectedTypes.push_back(Data::T_DOWNLOAD);
          expectedTypes.push_back(Data::T_UPLOAD);
          expectedTypes.push_back(Data::T_EQUAL);
          expectedTypes.push_back(Data::T_ERROR);
          expectedTypes.push_back(Data::T_DELETE);

          Data message = process->receive(expectedTypes);

          if (message.type == Data::T_DOWNLOAD) {
            process->sendConfirmation();
            if (process->getArq(filePath) == EQUAL)
              cout << "File received" << '\n';
            else {
              char error[ERROR_MSG_SIZE] = "Download failed";
              throwError(error);
            }
          }
          else if (message.type == Data::T_UPLOAD) {
            process->sendConfirmation();
            try {
              if (!ifstream(filePath)) {
                char error[ERROR_MSG_SIZE] = "Error on opening file";
                throwError(error);
                process->sendConfirmation(false);
                process->rcvConfirmation();
                unlock_file(idArq);
                continue;
              }

              int timeStamp = obtainTSofFile(filePath);

              process->send(Data::T_SOF, to_string(timeStamp));
              process->rcvConfirmation();

              if (process->sendArq(filePath) != 0) {
                char error[ERROR_MSG_SIZE] = "Error sending file";
                throwError(error);
              }
            }
            catch (exception &e) {
              cout << e.what() << endl;
              process->sendConfirmation(false);
              process->rcvConfirmation();
              unlock_file(idArq);
              continue;
            }
          }
          else if (message.type == Data::T_EQUAL) {
            process->sendConfirmation();
            unlock_file(idArq);
            continue;
          } else {
            process->sendConfirmation();
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
        process->send(Data::T_DELETE, nameOfTheFile);
        process->rcvConfirmation();
      }
    }

    // Send confirmation
    process->send(Data::T_DONE);
    process->rcvConfirmation();

    // Receive files which the client does not have yet
    while (true) {
      Data message = process->receive(expectedTypes);
      if (message.type == Data::T_DONE) {
        process->sendConfirmation();
        break;
      }
      else if (message.type == Data::T_DOWNLOAD) {
        string idArq = message.content;
        string filePath = process->folderOfTheUser + PATH_SEPARATOR + idArq;
        if (allowSending(idArq)) {
          process->sendConfirmation();
          if (process->getArq(filePath) != OK) {
            char error[ERROR_MSG_SIZE] = "Error on downloading files";
            throwError(error);
          }
          unlock_file(idArq);
        }
        else {
          process->sendConfirmation(false);
          process->rcvConfirmation();
          continue;
        }
      }
      else {
        char error[ERROR_MSG_SIZE] = "Error of not found file";
        throwError(error);
        process->sendConfirmation();
        continue;
      }
    }

    previousListOfFiles = File::listNamesOfFiles(process->folderOfTheUser);
    sleep(TIME_DO_NOTHING);
  }
}
