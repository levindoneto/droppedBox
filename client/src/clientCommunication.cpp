#include "../headers/clientCommunication.hpp"
#include "../../utils/headers/dropboxUtils.h"
#include "../../utils/fileSystem/headers/file.hpp"

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
  process = client->process->createProcComm();
  while (true) {
    mergeTheseFiles();
    getAllFilesFromServer();
    sleep(TIME_DO_NOTHING);
  }
}

void ClientCommunication::mergeTheseFiles() {
  /*
  File *file = new File();
  for (File &file : file->listNamesOfFiles(client->folderOfTheUser)) {
    mergeFile(file);
    filenames.remove(file.name());
  }
  for (string &nameOfTheFile : filenames) {
    bool delFine = false;
    while (!delFine) {
      process->send(Data::T_DELETE, nameOfTheFile);
      delFine = true;
      try {
        process->receive(Data::T_OK);
      } catch (ResponseException &e){
        delFine = false;
      }
    }
  }
  */
  process->send(Data::T_DONE);
  filenames = File::listNamesOfFiles(client->folderOfTheUser);
}

void ClientCommunication::getAllFilesFromServer() {
  list<string> syncExpectedTypes;
  syncExpectedTypes.push_back(Data::T_DONE);
  syncExpectedTypes.push_back(Data::T_SYNC);
  syncExpectedTypes.push_back(Data::T_DELETE);
  while (true) {
    Data processdataM = process->receive(syncExpectedTypes);
    if (processdataM.type == Data::T_SYNC) {
      string nameOfTheFile = processdataM.content;
      process->receive_file(client->folderOfTheUser + SLASH + nameOfTheFile);
    }
    else if (processdataM.type == Data::T_DELETE) {
      string filepath = client->folderOfTheUser + SLASH + processdataM.content;
      remove(filepath.c_str());
    } else if (processdataM.type == Data::T_DONE) break;
  }
}

void ClientCommunication::mergeFile(File file) {
  process->send(Data::T_SYNC, file.name());
  int modtime = stoi(process->receive_content(Data::T_MODTIME));
  if (modtime > file.modification_time())
    client->down(file.name(), client->folderOfTheUser, process);
  else if (modtime < file.modification_time())
    client->up(file.name(), client->folderOfTheUser, process);
  else
    process->send(Data::T_DONE);

}
