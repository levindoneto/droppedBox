#include <iostream>
#include <string>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "../headers/folder.hpp"
#include "../../headers/dropboxUtils.hpp"

/*  IN_CREATE – File/directory created in watched directory
    IN_DELETE – File/directory deleted from watched directory
    IN_MODIFY – File was modified
    IN_MOVED_FROM – File moved out of watched directory
    IN_MOVED_TO – File moved into watched directory */
#define INOTIFY_EVENTS IN_MODIFY | IN_CREATE | IN_DELETE\
 | IN_MOVED_FROM | IN_MOVED_TO
#define EVENT_SIZE sizeof (struct inotify_event)
#define LEN_NAME 16
#define TRUE 1
#define MAX_EVENTS 1024
#define EVENT_BUF_LEN MAX_EVENTS * (EVENT_SIZE + LEN_NAME)

using namespace std;

Folder::~Folder() {
	cout << "Remove watch of sync...";
	// TODO: Remove watch when an obj folder is destroyed
}

Folder::Folder(string folderPath) {
	int lenName = folderPath.size();
	int init;
  int watchedFolder;
  int i;
  int length;
  char buffer[EVENT_BUF_LEN];

	if (folderPath.back() != END_PATH) { // str::back = last char of the string
		this->folderPath = folderPath;
	} else {
		this->folderPath = folderPath.substr(BEGIN_STR, lenName-1); // delete last char (/)
	}
  const char *folder = this->folderPath.c_str();
	init = inotify_init();
	if (init == -1) {
		throwError("Could not initialize inotify");
	}

	watchedFolder = inotify_add_watch(init, folder, INOTIFY_EVENTS);

	if (watchedFolder == -1) {
		throwError("Could not watch that folder");
	}

	while(1) {
		i = 0;
		length = read(init, buffer, EVENT_BUF_LEN);

		while (i < length) {
	    struct inotify_event *event = (struct inotify_event *) &buffer[i];
			if (event->len) {
		 		if (IN_CREATE) {
		  		if (IN_ISDIR) {
            cout << "CREATE DIR";
          } else {
            cout << "CREATE FILE";
          }
		  	}
        if (IN_MODIFY) {
          if (IN_ISDIR) {
            cout << "MODIFY DIR";
          } else {
            cout << "MODIFY FILE";
          }
		  	}
	  		if (IN_DELETE) {
    			if (IN_ISDIR) {
            cout << "DELETE DIR";
          }	else {
            cout << "DELETE FILE";
          }
	  		}
				if (IN_MOVED_FROM) {
		    	if (IN_ISDIR) {
            cout << "MOVED OUT DIR";
          } else {
            cout << "MOVED OUT FILE";
          }
		  	}
				if (IN_MOVED_TO) {
		    	if (IN_ISDIR) {
            cout << "MOVED IN DIR";
          } else {
            cout << "MOVED IN FILE";
          }
		  	}

		  	i += EVENT_SIZE + event->len;
      }
    }
  }
}

string Folder::getFolderPath() {
	return this->folderPath + END_PATH;
}
