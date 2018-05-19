#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <vector>
#include <string>
#include<stdio.h>

#include "../headers/clientUser.hpp"
#include "../../utils/headers/ui.hpp"
#include "../../utils/headers/dropboxUtils.hpp"
#include "../../utils/headers/process.hpp"

using namespace std;

ClientUser::ClientUser(string userId, Folder *userFolder, char* ip, int port) {
  this->userId = userId;
  this->userFolder = userFolder;
  this->ip = ip;
  this->port = port;
  this->socketDescriptor = this->loginServer();
}

void ClientUser::startThreads(){
  //inotifyEvent();
  thread inotifyThread = thread(&ClientUser::inotifyEvent, this);
  thread syncDirThread = thread(&ClientUser::syncDirLoop, this);
  thread userLoop = thread(&ClientUser::userLoop, this);
  thread commandLoopThread = thread(&ClientUser::commandLoop, this);
  userLoop.join();
  //inotifyThread.join();
}

void ClientUser::syncDirLoop() {
  usleep(10000000); //10 seconds
  //cout << "ITS ALIVE! sync" << endl;
  while(TRUE);
}

void ClientUser::commandLoop() {
  //cout << "ITS ALIVE! command" << endl;
  while(TRUE);
}

void ClientUser::userLoop() {
  int resp = !EXIT;
  string command, parameter, ip = this->ip;
  vector<string> commandToRun, wholeCommand;
  Process* proc = new Process();
  showMenu();
  while(resp != EXIT) {
    commandToRun = getUserCommand();
    addCommandToQueue(commandToRun);
    wholeCommand = getCommandFromQueue();
    command = wholeCommand.front();
    parameter = wholeCommand.back();
    //cout << command << endl;
    //cout << parameter << endl;

    resp = proc->managerCommands(command, parameter, this, this->port, ip, this->socketDescriptor);
  };
}

void ClientUser::inotifyEvent() {
  int fd, wd, wd1, i = 0, len = 0;
  char pathname[100],buf[1024];
  struct inotify_event *event;
  string folderStr = this->userFolder->getHome();
  folderStr += "/sync_dir_" + this->userId;
  //cout << folderStr << endl;
  char watchedPath[100];
  folderStr.copy(watchedPath, folderStr.length(), 0);
  fd = inotify_init1(IN_NONBLOCK);
  bool notTempFile;
  bool threIsThisFile;
  wd = inotify_add_watch(fd, watchedPath, IN_ALL_EVENTS);
  string command;
  string parameter;
  vector<string> commandToRun;

  while(true) {
    i = 0;
    len = read(fd,buf,1024);

    while(i < len) {
      event = (struct inotify_event *) &buf[i];
      sprintf(pathname, "%s/%s", watchedPath, event->name);
      notTempFile = (event->name[0] != '.') && (event->name[strlen(event->name) - 1] != '~');
      threIsThisFile = fileExists(pathname);
      if(!fileExists(pathname) && notTempFile) {
        command = DELETE_FILE;
        parameter = event->name;
        vector<string> commandToRun;
        commandToRun.push_back(command);
        commandToRun.push_back(parameter);
        addCommandToQueue(commandToRun);
        command.clear();
        parameter.clear();
        commandToRun.clear();
        //printf("%s : deleted\n",event->name);
      }
      if(event->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
        notTempFile = (event->name[0] != '.') && (event->name[strlen(event->name) - 1] != '~');
        threIsThisFile = fileExists(pathname);
        if (notTempFile && threIsThisFile) {
          command = GET_SYNC_DIR;
          parameter = "";
          vector<string> commandToRun;
          commandToRun.push_back(command);
          commandToRun.push_back(parameter);
          addCommandToQueue(commandToRun);
          command.clear();
          parameter.clear();
          commandToRun.clear();
        }
        else if (!threIsThisFile)
          int c = 0;
          //printf("%s : deleted\n",event->name);
      }
      // update index to start of next event
      i += sizeof(struct inotify_event) + event->len;
    }
  }
}

vector<string> ClientUser::getCommandFromQueue() {
  this->commandMutex.lock();
  vector<string> c = this->commandQueue.front();
  this->commandQueue.pop();
  this->commandMutex.unlock();
  return c;
}

void ClientUser::addCommandToQueue(vector<string> command) {
  this->commandMutex.lock();
  this->commandQueue.push(command);
  this->commandMutex.unlock();
}

string ClientUser::getUserId() {
  return this->userId;
}

Folder* ClientUser::getUserFolder() {
  return this->userFolder;
}
void ClientUser::setUserFolder(Folder* userFolder) {
  this->userFolder = userFolder;
}

int ClientUser::loginServer() {
  char* ip = this->ip;
  int port = this->port;
  ClientUser* user = this;
  int socketDesc;
  int status;
  unsigned int lenSckAddr;
  struct sockaddr_in serverAddress;
  struct sockaddr_in from;
  struct hostent *host;
  string clientFolderPath;
  string serverFolderPath;
  UserInfo userInfo = {};

  char buffer[BUFFER_SIZE];
  fflush(stdin);
  // Get host
  host = gethostbyname(ip);
  if (host == NULL) {
    throwError("The host does not exist");
  }

  socketDesc = openSocket();

  // Address' configurations
  serverAddress.sin_family = AF_INET; // IPv4
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(serverAddress.sin_zero), BYTE_IN_BITS);

  // Folder management after the user gets logged in
  clientFolderPath = getpwuid(getuid())->pw_dir; // Get user's home folder
  clientFolderPath = clientFolderPath + "/sync_dir_" + user->getUserId();
  serverFolderPath = "db/clients/sync_dir_" + user->getUserId();
  Folder* folder = new Folder("");
  folder->createFolder(clientFolderPath);
  folder->createFolder(serverFolderPath);

  string message = "[Client Login]: User " + user->getUserId()
      + " has logged in via the socket " + to_string(socketDesc);
  message.copy(userInfo.message, message.length(), 0);
  string userId = user->getUserId();
  userId.copy(userInfo.userId, userId.length(), 0);
  writeToSocket(userInfo, socketDesc, ip, port);

  return socketDesc;
}

vector<string> ClientUser::getUserCommand() {
  vector<string> commandWithParameter;
  string wholeCommand;
  char string[256];
  cout << endl << PREFIX << " ";
  fflush(stdin);
  //scanf("%s\n", string);
  cin.clear();
  cin.sync();
  getline(cin, wholeCommand);
  //wholeCommand = string;
  commandWithParameter = parseUserCommand(wholeCommand, " ");
  return commandWithParameter;
}
