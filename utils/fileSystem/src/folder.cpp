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
#define INOTIFY_EVENTS IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_CREATE | IN_DELETE\
 | IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO | IN_OPEN
#define EVENT_SIZE sizeof (struct inotify_event)
#define EVENT_BUF_LEN 1024 * (EVENT_SIZE + 16)

using namespace std;

Folder::~Folder() {
	cout << "Remove watch of sync...";
	// TODO: Remove watch when an obj folder is destroyed
}

Folder::Folder(string folderPath) {
	int lenStr = folderPath.size();
	if (folderPath.back() != END_PATH) { // str::back = last char of the string
		this->folderPath = folderPath;
	} else {
		this->folderPath = folderPath.substr(BEGIN_STR, lenStr-1); // delete last char (/)
	}

	// TODO: Work on inotify with files
}

string Folder::getFolderPath() {
	return this->folderPath + END_PATH;
}
