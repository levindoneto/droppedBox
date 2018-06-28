DropboxServer#include "../headers/serverUser.hpp"
#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/folder.hpp"

ServerUser::ServerUser(DropboxServer *server, Process *process) {
  usingActive = true;
  this->server = server;
  this->process = process;
}

ServerUser::~ServerUser() {
  delete process;
}

void *ServerUser::run() {
  process->confirmComm();
  username = process->receive_content(Data::T_LOGIN);
  File::createFolderForFiles(username);

  server_sync = new ServerCommunication(this);
  server_sync->start();

  cout << username << " logged in" << endl;

  mainloop();

  return NULL;
}

void ServerUser::mainloop() {
  while (true) {
    Data request = process->receive_request();
    if (request.type == Data::T_UPLOAD) {
      receive_upload(request.content);
    }
    else if (request.type == Data::T_DOWNLOAD) {
      send_download(request.content);
    }
    else if (request.type == Data::T_LS) {
      list_server();
    }
    else if (request.type == Data::T_BYE) break;
  }
  for (Process *backup : server->backupServers) {
    backup->send(Data::T_CLIENT_DISCONNECT, process->ip);
  }
  cout << "User " << username << " logged out." << endl;
  usingActive = false;
}

void ServerUser::receive_upload(string nameOfTheFile, Process *process) {
  if (!process) {
    process = this->process;
  }
  string filepath = username + '/' + nameOfTheFile;
  if (allowSending(nameOfTheFile)) {
    process->send(Data::T_OK);
    cout << "The user " << username << " is uploading " << nameOfTheFile << endl;
    process->receive_file(filepath);
    for (Process *backup : server->backupServers) {
      backup->send(Data::T_UPLOAD, filepath);
      backup->send_file(filepath); // TODO: Add to process.cpp
    }
    unlock_file(nameOfTheFile);
    cout << "The user " << username << " successfully uploaded the file"
      << nameOfTheFile << endl;
  }
  else {
    cout << "Error: File " << nameOfTheFile << " currently syncing" << endl;
    process->send(Data::T_ERROR, "File currently syncing");
    return;
  }
}

void ServerUser::send_download(string nameOfTheFile, Process *process) {
  if (!process) {
    process = this->process;
  }
  string filepath = username + '/' + nameOfTheFile;

  if (!fileInFolder(filepath)) {
    cout << "Error opening file " << nameOfTheFile << " at " << username << endl;
    process->send(Data::T_ERROR, "File not found");
    return;
  }
  if (allowSending(nameOfTheFile)) {
    process->send(Data::T_OK);
    cout << username << " is downloading " << nameOfTheFile << "..." << endl;
    process->send_file(filepath);
    cout << username << " downloaded " << nameOfTheFile << endl;
    unlock_file(nameOfTheFile);
  } else {
      cout << "Error: File " << nameOfTheFile << " currently syncing" << endl;
      process->send(Data::T_ERROR, "File currently syncing");
  }
}

void ServerUser::list_server() {
  string file_list = File::list_directory_str(username);
  process->send_long_content(Data::T_LS, file_list);
}
