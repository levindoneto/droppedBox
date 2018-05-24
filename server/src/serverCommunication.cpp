#include "../headers/serverCommunication.hpp"

ServerCommunication::~ServerCommunication() {
  delete processComm;
}

ServerCommunication::ServerCommunication(Process *processComm) {
  string new_session;
  while (true) {
    string msg_s = processComm->sock->receive();
    Data msg = Data::parse(msg_s);
    if (msg.type == Data::T_SYN && msg.session != processComm->session) {
      new_session = msg.session;
      break;
    }
  }
  this->processComm = new Process(processComm->idUser,
                    new_session,
                    processComm->sock->get_answerer());
}

void *ServerCommunication::run() {
  processComm->accept_processComm();
  while (true) {
    list<string> files_to_update = File::listNamesOfFiles(processComm->folderOfTheUser);
    Data msg = processComm->receive(Data::T_SYNC);
    list<string> expected_types;
    bool receiving_stats = true;
    processComm->send_ack();
    expected_types.push_back(Data::T_STAT);
    expected_types.push_back(Data::T_DONE);

    while (receiving_stats) {
      Data msg = processComm->receive(expected_types);

      // Check file timestamp
      if (msg.type == Data::T_STAT) {
        processComm->send_ack();
        int timestamp_sep = msg.content.find('|');

        int content_len = msg.content.size();
        int timestamp_len = timestamp_sep;
        int nameOfTheFile_len = content_len - timestamp_len - 1;

        int timestamp_remote = stoi(msg.content.substr(0, timestamp_len));
        string nameOfTheFile = msg.content.substr(timestamp_len + 1, content_len);
        string filepath = processComm->folderOfTheUser + '/' + nameOfTheFile;

        if (!allowSending(nameOfTheFile)) {
          // ta mandando arq
          processComm->send_ack(false);
          processComm->receive_ack();
          continue;
        }

        // pega tempos do server
        int timestamp_local = obtainTSofFile(filepath);

        // SYNC POR TEMPOS
        if (timestamp_remote < timestamp_local) {
          // server manda arq then
          try   {
            processComm->send(Data::T_DOWNLOAD);
            processComm->receive_ack();
            int timestamp = obtainTSofFile(filepath);
            processComm->send(Data::T_SOF, to_string(timestamp));
            processComm->receive_ack();

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
          processComm->send(Data::T_UPLOAD);
          processComm->receive_ack();

          if (processComm->getArq(filepath) == 0)
            printf("up ok.");
          else
            printf("up not ok.");
        }
        else {
          processComm->send(Data::T_EQUAL);
          processComm->receive_ack();
        }

        files_to_update.remove(nameOfTheFile);
        unlock_file(nameOfTheFile);
      }
      else if (msg.type == Data::T_DONE) {
        processComm->send_ack();
        if (files_to_update.size() == EQUAL) {
          processComm->send(Data::T_DONE);
          processComm->receive_ack();
          break;
        }
        else {
          for (list<string>::iterator it = files_to_update.begin();
            it != files_to_update.end();
            ++it
          ) {
            try {
              string nameOfTheFile = *it;
              string filepath = processComm->folderOfTheUser + '/' + nameOfTheFile;
              // user baixa o arq
              if (!ifstream(filepath)) {
                continue;
              }
              processComm->send(Data::T_DOWNLOAD, nameOfTheFile);
              bool ok = processComm->receive_ack();
              if (ok) {
                int timestamp = obtainTSofFile(filepath);
                processComm->send(Data::T_SOF, to_string(timestamp));
                processComm->receive_ack();

                if (processComm->sendArq(filepath) == EQUAL)
                  printf("download ok.");
                else
                  printf("download not ok.");
              }
              else {
                // PROB NO USER
                processComm->send_ack();
              }
            }
            catch (exception &e) {
              cout << e.what() << endl;
              continue;
            }
          }

          processComm->send(Data::T_DONE);
          processComm->receive_ack();
          break;
        }
      }
    }
  }
}
