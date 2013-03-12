/*************************************
 * Header Files                      *
 ************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>


/*************************************
 * Structures                        *
 ************************************/
struct hist_node {
  struct hist_node *prev;
  char *entry;
  struct hist_node *next;
};

typedef struct {
  char **args;
  int  in_file_handle;
  int  out_file_handle;
  int  startTime;
  int  endTime;
  int  exit_code;
} process;

typedef struct {
  int  num_processes;
  process **processes;
} process_list;	

/*************************************
 * Function Prototypes               *
 ************************************/
// String Manipulation Functions
char *strrev(char *str);
void itoa(int x, char *buffer, int base);
int countIn(char ch, char *str);
char **buildArgs(char *in);
char *trim(char *str);

// User Interaction Functions:
int getInput(char *cmd);
int getch(void);
int removeLetterAt(char *msg, int index);
int insertLetterAt(char *msg, char letter, int index);

// Shell Command Execution Functions:
int executeCommand(process_list *pa);
int parseLine(char *cmd);
int parseCommand(process *p, char *cmd);
void runScript(char *input);
char *resolveVar(char *variable);
int killMe(int code);
int addProcessToList(process *p, process_list *pl);

// Command History Functions:
void historyAdd(char *command, int persist);
char *historyBack();
char *historyForward();
void historyClear();
void historyPrint();

//Signature Wolfie Function:
void wolfiePrint();


/*************************************
 * Constants                         *
 ************************************/
#define MAX_INPUT 1024
#define MAX_DIR_LENGTH 255
const char DEBUG_FLAG = 'd';
const char TIMED_FLAG = 't';
const char *EXIT_CMD = "exit";
const char *CD_CMD = "cd";
const char *CLEAR_CMD = "clear";
const char *HISTORY_CMD = "history";
const char *HISTORY_FILENAME = "/.swishtory";
const char *WOLFIE_CMD = "wolfie";

/*************************************
 * Global Variables                  *
 ************************************/
char *prompt = "swish> ";
char dir[MAX_DIR_LENGTH];
struct hist_node *history_head;
struct hist_node *history_ptr;
struct hist_node *history_tail;
char *full_name; 
int history_fd;
char *home_dir;
int last_exit_code;
int debug = 0;
int timed = 0;
