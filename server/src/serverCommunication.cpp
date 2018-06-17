#include "../headers/serverCommunication.hpp"
#include "../headers/serverUser.hpp"
#include "../../utils/headers/dropboxUtils.h"

ServerCommunication::~ServerCommunication() {
  delete processComm;
}

ServerCommunication::ServerCommunication(
  Process *processComm,
  map<string, ServerUser*> syncUserThreads
) {
  string newUserDeviceSession; // Each device gets one in the beggining regardless
  while (true) {
    string dataMessage = processComm->sock->receive();
    Data msg = Data::parse(dataMessage);
    if (msg.type == Data::T_SYN && msg.session != processComm->session) {
      newUserDeviceSession = msg.session;
      break;
    }
  }
  this->processComm = new Process(processComm->idUser,
                    newUserDeviceSession,
                    processComm->sock->get_answerer());
}

void *ServerCommunication::run() {
  processComm->initProcessComm();
  while (true) {
    list<string> filesToBePosted = File::listNamesOfFiles(
      processComm->folderOfTheUser
    );
    Data msg = processComm->receive(Data::T_SYNC);
    list<string> expected_types;
    bool receiving_stats = true;
    processComm->sendConfirmation();
    expected_types.push_back(Data::T_STAT);
    expected_types.push_back(Data::T_DONE);
    expected_types.push_back(Data::T_DELETE);

    while (receiving_stats) {
      Data msg = processComm->receive(expected_types);
      // Check file timestamp
      if (msg.type == Data::T_STAT) {
        processComm->sendConfirmation();
        int timestamp_sep = msg.content.find('|');

        int content_len = msg.content.size();
        int timestamp_len = timestamp_sep;
        int nameOfTheFile_len = content_len - timestamp_len - 1;

        int timestamp_remote = stoi(msg.content.substr(0, timestamp_len));
        string nameOfTheFile = msg.content.substr(timestamp_len + 1, content_len);
        string filepath = processComm->folderOfTheUser + '/' + nameOfTheFile;

        if (!allowSending(nameOfTheFile)) {
          printf("to nmabsdfs\n");
          // ta mandando arq
          processComm->sendConfirmation(false);
          processComm->rcvConfirmation();
          continue;
        }

        // pega tempos do server
        int timestamp_local = obtainTSofFile(filepath);

        // SYNC POR TEMPOS
        if (timestamp_remote < timestamp_local) {
          // server manda arq then
          try {
            processComm->send(Data::T_DOWNLOAD);
            processComm->rcvConfirmation();
            int timestamp = obtainTSofFile(filepath);
            processComm->send(Data::T_SOF, to_string(timestamp));
            processComm->rcvConfirmation();
            if (processComm->sendArq(filepath) == 0)
              printf("download ok.");
            else
              printf("download not ok.");
          }
          catch (exception &e) {
            unlock_file(nameOfTheFile);
            continue;
          }
        }
        else if (timestamp_remote > timestamp_local || timestamp_local == ERROR) {
          // server gets
          printf("upasdasd\n");
          cout << "test" << endl;
          processComm->send(Data::T_UPLOAD);
          processComm->rcvConfirmation();
          // Upload on the client side
          if (processComm->getArq(filepath) == 0)
            printf("up ok.");
          else
            printf("up not ok.");
        }
        else {
          processComm->send(Data::T_EQUAL);
          processComm->rcvConfirmation();
        }

        filesToBePosted.remove(nameOfTheFile);
        unlock_file(nameOfTheFile);
      }



      else if (msg.type == Data::T_DELETE) {
        string nameOfTheFile = msg.content;
        string filepath = processComm->folderOfTheUser
          + PATH_SEPARATOR
          + nameOfTheFile;
        if (processComm->deleteFile(filepath) == OK) {
          cout << "The file " << nameOfTheFile
            << " has been successfully removed :)" << endl;
        } else {
            cout << "The file " << nameOfTheFile
              << " has been successfully removed :)" << endl;
        }
        filesToBePosted.remove(nameOfTheFile);
        processComm->sendConfirmation(); // About the attempt of deleting the file
      }




      else if (msg.type == Data::T_DONE) {
        processComm->sendConfirmation();
        if (filesToBePosted.size() == EQUAL) {
          processComm->send(Data::T_DONE);
          processComm->rcvConfirmation();
          break;
        }
        else {
          for (list<string>::iterator fname = filesToBePosted.begin();
            fname != filesToBePosted.end();
            ++fname
          ) {
            try {
              string nameOfTheFile = *fname;
              string filepath = processComm->folderOfTheUser + '/' + nameOfTheFile;
              // user baixa o arq
              if (!ifstream(filepath)) {
                continue;
              }
              // Download on the client side
              processComm->send(Data::T_DOWNLOAD, nameOfTheFile);
              bool ok = processComm->rcvConfirmation();
              if (ok) {
                int timestamp = obtainTSofFile(filepath);
                processComm->send(Data::T_SOF, to_string(timestamp));
                processComm->rcvConfirmation();
                if (processComm->sendArq(filepath) == EQUAL)
                  printf("download ok.");
                else
                  printf("download not ok.");
              }
              else {
                // PROB NO USER
                processComm->sendConfirmation();
              }
            }
            catch (exception &e) {
              cout << e.what() << endl;
              continue;
            }
          }

          processComm->send(Data::T_DONE);
          processComm->rcvConfirmation();
          break;
        }
      }
    }
  }
}
