#include "../headers/serverCommunication.hpp"
#include "../headers/serverUser.hpp"
#include "../headers/dropboxServer.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/folder.hpp"

ServerSync::ServerSync(ServerUser *parent) : Thread() {
  this->parent = parent;
  log_file = parent->username + "/sync_log";
}

ServerCommunication::~ServerCommunication() {
  delete processComm;
}

void *ServerCommunication::run() {
  this->process = parent->process->rcvProcComm();
  process->confirmComm();
  while (true) {
    sync_client_files();
    cout << "sending left out files";
    send_files_to_client();
    cout << "sent left out files";
  }
}

void ServerCommunication::sync_client_files() {
  // TODO: Check username vs folderOfTheUser
  // Create list of sync expected types
  list<string> syncTypes;
  syncTypes.push(Data::T_DONE);
  syncTypes.push(Data::T_SYNC);
  syncTypes.push(Data::T_DELETE);

  files_not_synced = File::listNamesOfFiles(parent->username);
  while (true) {
    Data msg = process->receive(syncTypes);
    if (msg.type == Data::T_DONE) {
      break;
    }
    else if (msg.type == Data::T_SYNC) {
      sync_file(msg.content);
    }
    else if (msg.type == Data::T_DELETE) {
      delete_file(msg.content);
    }
  }
}

void ServerCommunication::sync_file(string nameOfTheFile) {
  list<string> syncTypes;
  syncTypes.push(Data::T_UPLOAD);
  syncTypes.push(Data::T_DOWNLOAD);
  File file(parent->username + SLASH + nameOfTheFile);
  process->send(Data::T_MODTIME, to_string(file.modification_time()));
  Data msg_action = process->receive(actionTypes);
  if (msg_action.type == Data::T_UPLOAD) {
    cout << "Receiving " << nameOfTheFile << endl;
    parent->receive_upload(nameOfTheFile, process);
    cout << "Done receiving file\n";
  }
  else if (msg_action.type == Data::T_DOWNLOAD) {
    cout << "send file";
    parent->send_download(nameOfTheFile, process);
    cout << "done sending file";
  }
  files_not_synced.remove(nameOfTheFile);
}

void ServerCommunication::delete_file(string nameOfTheFile) {
  cout << "Delete " <<  nameOfTheFile << endl;
  if (allowSending(nameOfTheFile)) {
    process->send(Data::T_OK);
    string filepath = parent->username + SLASH + nameOfTheFile;
    remove(filepath.c_str());
    for (auto const &thread_map : parent->server->threads) {
      ServerUser *thread = thread_map.second;
      if (thread->username != parent->username) {
        thread->server_sync->files_to_delete.push_back(nameOfTheFile);
      }
    }
    cout << "Done deleting file\n";
    unlock_file(nameOfTheFile);
    files_not_synced.remove(nameOfTheFile);
    for (Process *backup : parent->server->backupServers) {
      backup->send(Data::T_DELETE, parent->username + SLASH + nameOfTheFile);
    }
  } else {
    cout << "File cannot be deleted rn\n";
    process->send(Data::T_ERROR);
  }
}

void ServerCommunication::send_files_to_client() {
  for (string &nameOfTheFile : files_to_delete) {
    cout << "Delete " <<  nameOfTheFile << endl;
    remove(string(nameOfTheFile + SLASH + parent->username).c_str());
    process->send(Data::T_DELETE, nameOfTheFile);
    cout << "Done deleting file\n";
  }
  files_to_delete.clear();
  for (string &nameOfTheFile : files_not_synced) {
    cout << "send file";
    process->send(Data::T_SYNC, nameOfTheFile);
    process->send_file(parent->username + PATH + nameOfTheFile);
    cout << "done sending file";
  }
    process->send(Data::T_DONE);
}
