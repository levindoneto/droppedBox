DropboxServer#include "../headers/serverUser.hpp"
#include "../headers/serverCommunication.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/folder.hpp"

ServerUser::ServerUser(DropboxServer* server, Process *process) {
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
      for (pair<string, UDPUtils> backup : DropboxServer::backupServers) {
        Data msg = Data(NULL,
          NULL,
          request.type,
          username + SLASH + request.content
        );
        backup.second.send(msg.stringify());
      }
    }
    else if (request.type == Data::T_DOWNLOAD) {
      send_download(request.content);
    }
    else if (request.type == Data::T_LS) {
      list_server();
    }
    else if (request.type == Data::T_NEW_USER) {
      for (pair<string, UDPUtils> backup : Server::backupServers) {
        Data msg = Data(NULL, NULL, Data::T_IP, request.content);
        backup.second.send(msg.stringify());
      }
    }
    else if (request.type == Data::T_BYE) {
      break;
    }
  }
  cout << "User " << username << " logged out." << endl;
  usingActive = false;
}

void ServerUser::receive_upload(string filename, Process* process) {
  if (!process) {
    process = this->process;
  }
  string filepath = username + '/' + filename;
  if (allowSending(filename)) {
    process->send(Data::T_OK);
    cout << username << " is uploading " << filename << "..." << endl;
    process->receive_file(filepath);
    unlock_file(filename);
    cout << username << " uploaded " << filename << endl;
  }
  else {
    cout << "Error: File " << filename << " currently syncing" << endl;
    process->send(Data::T_ERROR, "File currently syncing");
    return;
  }
}

void ServerUser::send_download(string filename, Process* process) {
  if (!process) {
    process = this->process;
  }
  string filepath = username + '/' + filename;

  if (!fileInFolder(filepath)) {
    cout << "Error opening file " << filename << " at " << username << endl;
    process->send(Data::T_ERROR, "File not found");
    return;
  }
  if (allowSending(filename)) {
    process->send(Data::T_OK);
    cout << username << " is downloading " << filename << "..." << endl;
    process->send_file(filepath);
    cout << username << " downloaded " << filename << endl;
    unlock_file(filename);
  } else {
      cout << "Error: File " << filename << " currently syncing" << endl;
      process->send(Data::T_ERROR, "File currently syncing");
  }
}

void ServerUser::list_server() {
  string file_list = File::list_directory_str(username);
  process->send_long_content(Data::T_LS, file_list);
}
