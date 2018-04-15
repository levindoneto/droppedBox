#include <iostream>
#include <string>
#include <sys/inotify.h>

#include "../headers/folder.hpp"

/*  IN_ACCESS – File was accessed
    IN_ATTRIB – Metadata changed (permissions, timestamps, extended attributes, etc.)
    IN_CLOSE_WRITE – File opened for writing was closed
    IN_CLOSE_NOWRITE – File not opened for writing was closed
    IN_CREATE – File/directory created in watched directory
    IN_DELETE – File/directory deleted from watched directory
    IN_DELETE_SELF – Watched file/directory was itself deleted
    IN_MODIFY – File was modified
    IN_MOVE_SELF – Watched file/directory was itself moved
    IN_MOVED_FROM – File moved out of watched directory
    IN_MOVED_TO – File moved into watched directory
    IN_OPEN – File was opened */
#define INOTIFY_EVENTS IN_MODIFY | IN_CREATE | IN_DELETE\
 | IN_MOVED_FROM | IN_MOVED_TO
#define EVENT_SIZE sizeof (struct inotify_event)
#define LEN_NAME 16
#define AMX_EVENTS 1024
#define EVENT_BUF_LEN MAX_EVENTS * (EVENT_SIZE + LEN_NAME)

using namespace std;

Folder::~Folder() {
	cout << "Remove watch of sync...";
	// TODO: Remove watch when an obj folder is destroyed
}

Folder::Folder(string folderPath) {
	int lenName = folderPath.size();
	int init;
	char buffer[EVENT_BUF_LEN];

	if (folderPath.back() != END_PATH) { // str::back = last char of the string
		this->folderPath = folderPath;
	} else {
		this->folderPath = folderPath.substr(BEGIN_STR, lenStr-1); // delete last char (/)
	} 	

	init = inotify_init()
	if (init == -1) {
		throwError("Couldn't initislize inotify.");
	}

	watchedFolder = inotify_add_watch(folder, folderPath, INOTIFY_EVENTS);

	if (watchedFolder == -1) {
		throwError("Couldn't watch that folder.");
	}

	while(1) {
		i = 0;
		length = read(init, buffer, EVENT_BUF_LEN);

		while ( i < length ) {
	       		struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
			if ( event->len ) {
		 		if (IN_CREATE) {
		    			if (IN_ISDIR)
		      				// CREATE DIR
		    			else
		      				// CREATE FILE
		  		}
		   
		  		if (IN_MODIFY) {
		    			if (IN_ISDIR)
		      				// MODIFY DIR      
		    			else
		     				// MODIFY FILE      
		  		}
		   
		  		if (IN_DELETE) {
		    			if (IN_ISDIR)
		      				// DELETE DIR     
		    			else
		      				// DELETE FILE      
		  		} 
				if (IN_MOVED_FROM) {
		    			if (IN_ISDIR)
		      				// MOVED OUT DIR     
		    			else
		      				// MOVED OUT FILE      
		  		}
				if (IN_MOVED_TO) {
		    			if (IN_ISDIR)
		      				// MOVED IN DIR     
		    			else
		      				// MOVED IN FILE      
		  		}

		  	i += EVENT_SIZE + event->len;
        	}
      	}
}

string Folder::getFolderPath() {
	return this->folderPath + END_PATH;
}
