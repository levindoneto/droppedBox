#ifndef DROPBOXSERVER_H
#define DROPBOXSERVER_H

#include "../../utils/headers/dropboxUtils.hpp"

struct file_info {
	char name[FILENAME_SIZE];
	char extension[LEN_EXT_ARQ];
	char last_modified[LEN_ISO_TIME];
	int size;
}

struct client {
	int devices[MAX_DEVS];
	char userid[LEN_ID_USER];
	struct file_info[N_FILES];
	int logged_in;
}

void sendArq(char *file);
void getArq(char *file);
void mergeServerClient();

#endif
