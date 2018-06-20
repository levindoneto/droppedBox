#include "../headers/serverCommunication.hpp"
#include "../headers/serverUser.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/folder.hpp"

ServerCommunication::~ServerCommunication() {
  delete processComm;
}

ServerCommunication::ServerCommunication(
  Process *processComm,
  map<string, ServerCommunication*> *syncCommunicationThreads
) {
  syncThreads = syncCommunicationThreads;
  // Each device gets one in the beggining regardless
  string newUserDeviceSession;
  while (true) {
    string dataMessage = processComm->sock->receive();
    Data msg = Data::parse(dataMessage);
    if (msg.type == Data::T_SYN && msg.session != processComm->session) {
      newUserDeviceSession = msg.session;
      break;
    }
  }
  this->processComm = new Process(
    processComm->idUser,
    newUserDeviceSession,
    processComm->sock->get_answerer()
  );
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

    // Merge files
    while (receiving_stats) {
      Data msg = processComm->receive(expected_types);
      // Check file timestamp
      if (msg.type == Data::T_STAT) {
        processComm->sendConfirmation();
        int timestamp_sep = msg.content.find(SEPARATOR_FILENAME);
        int content_len = msg.content.size();
        int timestamp_len = timestamp_sep;
        int nameOfTheFile_len = content_len - timestamp_len - 1;
        int timestamp_remote = stoi(msg.content.substr(INIT, timestamp_len));
        string nameOfTheFile = msg.content.substr(timestamp_len + 1, content_len);
        string filepath = processComm->folderOfTheUser + PATH_SEPARATOR + nameOfTheFile;

        if (!allowSending(nameOfTheFile)) {
          // The file is being sent
          processComm->sendConfirmation(false);
          processComm->rcvConfirmation();
          continue;
        }
        // Get timestamps from server
        int timestamp_local = obtainTSofFile(filepath);
        // Sync via timestamps
        if (timestamp_remote < timestamp_local) {
          // Server sends the file to the user
          try {
            processComm->send(Data::T_DOWNLOAD);
            processComm->rcvConfirmation();
            int timestamp = obtainTSofFile(filepath);
            processComm->send(Data::T_SOF, to_string(timestamp));
            processComm->rcvConfirmation();
            if (processComm->sendArq(filepath) == OK) {
              printf("Down ok.\n");
            } else {
                printf("Down not ok.\n");
            }
          }
          catch (exception &e) {
            unlock_file(nameOfTheFile);
            continue;
          }
        }
        else if (timestamp_remote > timestamp_local || timestamp_local == ERROR) {
          // server gets
          processComm->send(Data::T_UPLOAD);
          processComm->rcvConfirmation();
          // Upload on the client side
          if (processComm->getArq(filepath) == OK) {
            printf("Up ok.\n");
          } else {
              printf("Up not ok.\n");
            }
        }
        else {
          processComm->send(Data::T_EQUAL);
          processComm->rcvConfirmation();
        }

        filesToBePosted.remove(nameOfTheFile);
        unlock_file(nameOfTheFile);
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
              string filepath = processComm->folderOfTheUser + PATH_SEPARATOR + nameOfTheFile;
              // User gets the file via download from this server
              if (!fileInFolder(filepath)) {
                continue;
              }
              // Download on the client side
              processComm->send(Data::T_DOWNLOAD, nameOfTheFile);
              bool ok = processComm->rcvConfirmation();
              if (ok) {
                int timestamp = obtainTSofFile(filepath);
                processComm->send(Data::T_SOF, to_string(timestamp));
                processComm->rcvConfirmation();
                if (processComm->sendArq(filepath) == OK) {
                  printf("Down ok.\n");
                } else {
                    printf("Down not ok.\n");
                }
              }
              else {
                processComm->sendConfirmation();
              }
            }
            catch (exception &e) {
              cout << e.what() << endl;
              continue;
            }
          }
          // Send confirmation after finishing any operation
          processComm->send(Data::T_DONE);
          processComm->rcvConfirmation();
          break;
        }
      }
      else if (msg.type == Data::T_DELETE) {
        string nameOfTheFile = msg.content;
        string filepath = processComm->folderOfTheUser
          + PATH_SEPARATOR
          + nameOfTheFile;
        if (processComm->deleteFile(filepath) == OK) {
          cout << "The file " << nameOfTheFile
            << " has been successfully removed from one session. "
            << "Now it will be removed from the other ones" << endl;
          // Delete the file for the other sessions (other devices) of the user who deleted the file
          for (
            map<string, ServerCommunication *>::iterator fname = (*syncThreads).begin();
            fname != (*syncThreads).end();
            ++fname
          ) {
            ServerCommunication *thread = fname->second;
            if (
              thread->processComm->idUser != this->processComm->idUser &&
              thread->processComm->session != this->processComm->session
            ) {
              thread->processComm->send(Data::T_DELETE, nameOfTheFile);
            }
          }
        } else {
            cout << "The file " << nameOfTheFile
              << " has been successfully removed :)" << endl;
        }
        filesToBePosted.remove(nameOfTheFile);
        processComm->sendConfirmation(); // About the attempt of deleting the file
      }
    }
  }
}
