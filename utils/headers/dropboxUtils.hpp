#define UNDEF -1 // not specified informationC
#define INIT 0 // for initialization of parameters
#define DEF 1 // specified information without worked out values
#define TRUE 1
#define ERROR -1
#define BYTE_IN_BITS 8
#define EXIT false
#define IS_THE_COMMAND 0
#define UPLOAD "upload"
#define DOWNLOAD "download"
#define LIST_SERVER "list_server"
#define LIST_CLIENT "list_client"
#define GET_SYNC_DIR "get_sync_directory"
#define EXIT_APP "exit"
#define HELP_C "HELP"
#define HELP_L "help"
#define EXIT_OPT_WRONG -1 // User typed a wrong option
#define EXIT_OPT_YES 1 // User wants to log off
#define EXIT_OPT_NO 0 // User wants to stay logged in
#define INVALID_OPTION "$ An invalid option was given"

#define CREATE_FILE 0

// Function which takes a string, and shows it, followed by exiting the app
void throwError (char* errorMessage);
