#include <iostream>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "../headers/dropboxUtils.hpp"

using namespace std;
string clientFolderPath;

void throwError (char* errorMessage) {
  perror(errorMessage);
  exit(TRUE);
}

// TODO: Remake this function to make it OO
/*
void getClientFolderPath(string folderPath) {
  clientFolderPath = folderPath;
}
*/

void *inotifyEvent(void*) {
  int init;
  int watchedFolder;
  int i;
  int length;
  char buffer[EVENT_BUF_LEN];
  cout << "sync_dir_lgtneto" << '\n'; // TODO: Change to the user folder

  //const char *folder = "sync_dir_lgtneto".c_str();
  char *folder = "~sync_dir_lgtneto"; // TODO: Change to the user folder
  init = inotify_init();
  if (init == ERROR) {
    throwError("Could not initialize inotify");
  }

  watchedFolder = inotify_add_watch(init, folder, INOTIFY_EVENTS);

  if (watchedFolder == ERROR) {
    throwError("It could not watch that folder");
  }

  while(TRUE) {
    i = 0;
    length = read(init, buffer, EVENT_BUF_LEN);

    while (i < length) {
      struct inotify_event *event = (struct inotify_event *) &buffer[i];
      if (event->len) {
        if (event->mask & IN_CREATE) {
            cout << "Arquivo criado" << '\n';
        }
        if (IN_MODIFY) {

        }
        if (event->mask & IN_DELETE) {

        }
        if (IN_MOVED_FROM) {

        }
        if (IN_MOVED_TO) {

        }
        if (IN_OPEN || IN_ACCESS) {

        }

        i += EVENT_SIZE + event->len;
      }
    }
  }
}
