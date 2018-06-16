#ifndef DROPBOXUTILS_HPP
#define DROPBOXUTILS_HPP

#include <netinet/in.h>
#include <netdb.h> // hostent
#include <pthread.h>
#include <string.h> // bzero
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h> // close
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

using namespace std;

#define UNDEF -1 // not specified information
#define INIT 0 // for initialization of parameters
#define DEF 1 // specified information without worked out values
#define TRUE 1
#define SUCCESS 1
#define ERROR -1
#define BYTE_IN_BITS 8
#define EXIT false
#define IS_THE_COMMAND 0
#define UPLOAD "upload"
#define DOWNLOAD "download"
#define LIST_SERVER "listserver"
#define LIST_CLIENT "listclient"
#define GET_SYNC_DIR "getsyncdir"
#define EXIT_APP "exit"
#define HELP_C "HELP"
#define HELP_L "help"
#define EXIT_OPT_WRONG -1 // User typed a wrong option
#define EXIT_OPT_YES 1 // User wants to log off
#define EXIT_OPT_NO 0 // User wants to stay logged in
#define INVALID_OPTION "$ An invalid option was given"
#define CLIENT_LIST 0
#define SERVER_LIST 1
//#define BUFFER_SIZE 1024
#define CURRENT_FOLDER "."
#define CREATE_FILE 0
#define EMPTY_PATH ""
#define EQUAL 0
#define TAB "\t"
#define SEPARATOR_LIST ' | '
#define SIZE_FILENAME_LIST 26
#define SIZE_CTIMES_LIST 26
#define FILENAME_LABEL "     Name of the File      | "
#define ACCESS_TIME_LABEL "        Access Time      |"
#define MODIFICATION_TIME_LABEL "    Modification Time     |"
#define CREATION_TIME_LABEL "      Creation Time      "
#define DATABASE "db/clients/sync_dir_"
#define CHUNCK_SIZE 1024
#define BUFFER_SIZE sizeof(datagram)
#define TIMEO 1
#define NO_USER ""
#define FILENAME_LABEL_S "     Name of the File        "
#define ACCESS_TIME_LABEL_S "        Access Time      |"
#define MODIFICATION_TIME_LABEL_S "    Modification Time     |"
#define CREATION_TIME_LABEL_S "      Creation Time       |"
#define BREAK_LINE "\n"
#define FORMAT_NAME_FILE_S " "
#define FILENAME_SIZE 75
#define ACK_SIZE 10
#define SLASH "/"
#define SEP_SYNC_DIR " "
#define ACK_SAME 0
#define ACK_UPLOAD 1
#define ACK_DOWNLOAD 2
#define DELETE_FILE "deletefile"
#define CONFIRMED_DEL 1
#define ERROR_MSG_SIZE 150
#define MAX_USER_ID_SIZE 36
#define FILES_PER_USER_MAX 36
#define ISO_TIME_LEN_MAX 150
#define LEN_PATH_FOLDER 150
#define LEN_ISO_TIME 150
#define LEN_EXT_ARQ 50
#define MAX_DEVS 2
#define LEN_ISO_TIME 150
#define LEN_ID_USER 150
#define N_FILES 45
#define SEPARATOR_FILENAME '|'
#define SIZE_TYPESERVER_FLAG 3
#define PRIMARY_SERVER_TYPE "--p"
#define BACKUP_SERVER_TYPE "--b"

typedef struct datagram {
   int  chunckId;
   char  chunck[CHUNCK_SIZE];
} Datagram;

typedef struct userInfo {
   char  userId[CHUNCK_SIZE/8];
   char  message[CHUNCK_SIZE*7/8];
} UserInfo;

/*  IN_CREATE – File/directory created in watched directory
    IN_DELETE – File/directory deleted from watched directory
    IN_MODIFY – File was modified
    IN_MOVED_FROM – File moved out of watched directory
    IN_MOVED_TO – File moved into watched directory
    IN_OPEN – File was opened
    IN_ACCESS – File was read */
#define INOTIFY_EVENTS IN_MODIFY | IN_CREATE | IN_DELETE\
 | IN_MOVED_FROM | IN_MOVED_TO | IN_OPEN | IN_ACCESS | IN_CLOSE_WRITE

#define EVENT_SIZE sizeof (struct inotify_event)
#define LEN_NAME 16
#define TRUE 1
#define MAX_EVENTS 1024
#define EVENT_BUF_LEN MAX_EVENTS * (EVENT_SIZE + LEN_NAME)

// Function which takes a string, and shows it, followed by exiting the app
void throwError (char* errorData);
unsigned int fileSize(string filePath);

static const string HOME = string(getenv("HOME"));

bool allowSending(string nameOfTheFile);
void unlock_file(string fil);

string obatingNameOfTheFile(string filepath);
unsigned int obtainTSofFile(string nameOfTheFile);
string ObtaingJustNameOfTheFile(string nameOfTheFile);

class timeout_exception : public runtime_error
{
public:
  timeout_exception() : runtime_error("ERROR: Timeout") {}
};

#endif
