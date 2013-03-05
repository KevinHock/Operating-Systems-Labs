/* CSE 306: Sea Wolves Interactive SHell */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termio.h>
#include <unistd.h>
#include <errno.h>

int getch(void);
int getInput(char *cmd);
int removeLetterAt(char *msg, int index);
int insertLetterAt(char *msg, char letter, int index);
int executeCommand(char *cmd, char *arg);
int parseLine(char *cmd);
int killMe(int code);
void itoa(int x, char *buffer, int base);
void runScript(char *input);
char *strrev(char *str);
char *resolveVar(char *variable);

#define MAX_INPUT 1024
#define MAX_DIR_LENGTH 255

//Kevin
int cbi(char* cmd,char **envp);
int qCd(char* cmd,char **envp);
int qPwd(char* cmd,char **envp);
int check4Env(char *);
char *foundEnvVar;
//Kevin

char *prompt = "swish> ";
const char *EXIT_CMD = "exit";
const char *CD_CMD = "cd";
const char DEBUG_FLAG = 'd';
const char TIMED_FLAG = 't';
const char *history_filename = ".swishtory";
const char *history_file;
char dir[MAX_DIR_LENGTH];
char *home_dir;
int last_exit_code;
int fd_history;
int debug = 0;
int timed = 0;

/*
 * The main control flow:
 */
int main (int argc, char ** argv, char **envp) {  
  char command[MAX_INPUT];
  char *cursor;
  int i;
  
  // Handle all, if any, of the flags:
  for(i=1; i<argc ; i++){
	if(*argv[i] == '-'){
	  cursor = argv[i] + 1;
	  printf("Found flags: %s\n", cursor);
	  while(*cursor != ' ' && *cursor != '\0' && *cursor != '\n'){
  	    if(*cursor == DEBUG_FLAG && debug == 0){
	      printf("DEBUG MODE ACTIVATED\n");
	      debug = 1;
	    }
	    cursor++;
	  }
	} else {
      runScript(argv[i]);
	}
  } 

  //  Load the command history file:
  // NOT YET IMPLEMENTED:
  home_dir = resolveVar("HOME");
  history_file = malloc(strlen(home_dir) + strlen(history_filename) + 1);
  cursor = home_dir;
  while(*cursor != '\0'){
	cursor++;  
  }  
//  printf("File is: %s", history_file);	
//  fd_history = open(



  // MAIN PROGRAM:
  while (1) {
    getInput(command);
    parseLine(command);
  } return 0;
}




int parseLine(char *cmd){
  char *cursor = cmd;
  char *first_command;
  char *args;
  int blocks;
  
  if(*cmd != '\0') blocks = 1;
  else blocks = 0;
  
  while(*cursor != '\0'){
    cursor++;
    if(*cursor == ' ' && *(cursor - 1) != ' ')
      blocks++;
  }

  if(debug){
    printf("DEBUG: COMMAND RECEIVED WITH %d TEXT BLOCKS\n", blocks);
  } fflush(stdout);
  
 /*
  We need to change all of this.
*
*check if built in, if it is then built in returns 1 and we don't pass to exec
 otherwise we loop through args add them to the array and then pass that to special exec


*
*
*
* 
 */
  // If the command is a single block of text, 
  // just execute it:
//or check for specifics here except for $?

  if(blocks == 1){

	if(!strcmp(cmd, EXIT_CMD)){
	  killMe(0);
    } else if(strcmp(cmd, CD_CMD))//ghi
      last_exit_code = executeCommand(cmd, "");
	//all in here

  // If the command is a single block of text with one argument,
  // just execute it:
  } else if(blocks == 2) {
	cursor = cmd;
	while(*cursor != ' ') cursor++;//?
	first_command = malloc(cursor-cmd);//cursor not at space
	strncpy(first_command, cmd, (cursor-cmd));
	//ghi print first_command see if 1 past space
	args = (cursor + 1);
	
	if(*args == '$' && *(args + 1) != '\0')
	  args = resolveVar(args + 1);

	if(*args == '~')
	  if(*(args+1) == '\0' || *(args+1) == ' ')
	    args = resolveVar(args);
	 
    if(!strcmp(first_command, CD_CMD)){
	  if(chdir(args)){
	    printf("Directory Not Found [%s]\n", args);
	    fflush(stdout);
        last_exit_code = 1;
	  } else errno = 0;
    } else last_exit_code = executeCommand(first_command, args);
	free(first_command);
  }
  
  return 0;
}


/*
 * Execute a process and return the exit code:
 */
int executeCommand(char *cmd, char *arg){
  int exit_code = 0;  
  int pid;

  if(debug){
    printf("RUNNING: %s\n", cmd);
  } fflush(stdout);
 
  // Handle empty arguments:	 
  if(strlen(arg) < 1) arg = NULL;
  
  // Fork and execute the command:
  pid = fork();
  if(pid == 0){
    execlp(cmd, cmd, arg, NULL);
    printf("Command [%s] was not found.\n", cmd);
    killMe(exit_code);
  } else {
	  // Wait for the child and store the exit code:
      waitpid(pid, &exit_code, 0);
//      printf("Exit code is now: %d\n", WEXITSTATUS(exit_code));
  }
  
  if(debug){
    printf("ENDED: \"%s\" (ret = %d)\n", cmd, errno);
  } fflush(stdout);

  return WEXITSTATUS(exit_code);
}


void runScript(char *input){
  printf("Going To Run Batch Job [%s]\n", input);
}

/*
 * Return the contents of an environmental variable:
 */
char *resolveVar(char *variable){
  char *result;
  
  if(debug){
    printf("DEBUG: RESOLVING VARIABLE [%s], RESULT: ", variable);
  } fflush(stdout);

  if((*variable == '~') && *(variable+1) == '\0'){
	result = getenv("HOME");
  } else if ((*variable == '?') && *(variable+1) == '\0') {
	  result = malloc((sizeof(char) * 4));
	  itoa(last_exit_code, result, 10);
  } else {
    result =  getenv(variable);
  }
  
  if(debug){
    printf("%s\n", result); 
  } fflush(stdout);
  return result;
}

//Checks if built in
int cbi(char* cmd,char **envp){
     //CHECK RETURN VALUES
	int popEipVal=-1;
	popEipVal = qCd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal = qPwd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal = qPrintenv(cmd,envp);
	if(popEipVal>=0)
		return(0);
	return(-1);
	//popEipVal = ;//after env variables	
}
void printEnvVars(char* cmd ,char** envp){
	if(strcmp("printenv",cmd)==0)
		for(; *envp != 0; envp++)
			printf("%s\n",envp[0]);
}
int qPwd(char* cmd ,char **envp){
	if(strcmp("pwd",cmd)==0){
		//get curr working dir
		findEnvVar("PWD",envp);
		printf("\n\n%s\n\n",foundEnvVar);
		return 0;
	}
	return -1;
}
int qCd(char* cmd,char **envp){
   	char cd[]="cd";
	if(strncmp("cd",cmd,2)==0){
		if(strcmp("cd",cmd)==0){
			findEnvVar("HOME",envp);//Home dir
		 	return(chdir(foundEnvVar));
		}
		if(strcmp("cd -",cmd)==0){
			findEnvVar("OLDPWD",envp);//Last dir
			int result = chdir(foundEnvVar);
			if(debug){
				fprintf(stderr, "result of changing dir is %d.\n",result);
				fflush(NULL);
			}
			return(result);
		}
		if(strncmp("cd ",cmd,3)==0){
			printf("Break here line 64  ghi\n");
			printf("Dir were changing to is %s.\n",cmd+3);
			fflush(NULL);
			return(chdir( cmd+3 ));
		}
		return -1;
	}
}



/*
 * Checks for SET and ECHO of variables
 * Sameple input from comments "set $PATH = /bin:/usr/bin:."
 */
int check4Env(char* cmd){
//Loop through for $
	char cc='a';
	int isEnv=0;
	i=0;
	int j;
	while(cc!=NULL){
		cc = cmd[i++];
		if(cc=='$'){
			isEnv=1;
			break;
		}		
	}
	if(isEnv==0)
		return(0);
	printf("this far. line 54\n");
//Now that we've found $
	//Check for Set
	if(strncmp("SET",cmd,3)==0 || strncmp("set",cmd,3)==0){
	//From $ to space make that the variable name and = to 
	//"set $PATH = /bin:/usr/bin:."
		//Loop through and find index after $ then find index after that = to ' ' or '='
		j=i;
		
		//i is 'P' and j is going to be ' '(9)
		for(;;){
			++j;
			cc=cmd[j];
			if(cc ==' ' || cc == '=')
				break;
			if(cc=='\0')
				return(-1);
		}
		//malloc PATH
		char* varName = malloc(sizeof(char)*(j-i));
		strncpy(varName,cmd+i,(j-i));
		if(debug)
			fprintf(stderr,"\nThe var name is = %s\n",varName);
		
		//j=9 i=5
		i=j;
		//Now there both 1 char after varName
		//Make sure to check for failure
		//Search for first char not ' ' or '=' and if it's '\0' than ff=1;
		//After that go until '\0' and malloc duh diff
		++i;
		if(cmd[i]=='\0' || cmd[j]=='\0')
			return(-1);
		for(;;){
			++i;
			cc=cmd[i];
			if(cc=='\0')
				return(-1);
			if(cc!=' '||cc!='=')
				break;
		}
		//cmd[i] == /
		j=i;
		++j;
		for(;;){
			if(cmd[j]=='\0')
				break;
			++j;
		}
		//cmd[j-1]== NULL
		j++;
		char* pathName = malloc(sizeof(char)*(j-i));
		strncpy(pathName,cmd+i,(j-i));
		if(debug)
			fprintf(stderr,"\nThe path name is %s\n",pathName);
		if(debug)
			
		setenv(varName,pathName,1);
		return(0);
	}
  	if(strncmp("echo",cmd,4)==0 || strncmp("echo",cmd,4)==0){
		//char after $ == cmd[i]
		j=i;
		while(cmd[j++]!='\0'){}
		//cmd[j-1]==NULL
		char* varName = malloc(sizeof(char)*(j-i));
		strncpy(varName,cmd+i,(j-i));
		if(debug)
			fprintf(stderr,"\n\necho'ing this:\"%s\"\n\n",varName);
		char *e = getenv(varName);
		printf("%s\n",e);
		return(0);
	}	
	return(-1);//ghi
}




/*
 * Wrapper code for exiting the shell:
 */
int killMe(int code){
  if(debug) printf("DEBUG: EXITING WITH CODE %d\n", code);
  fflush(stdout);
  exit(code);
}


/********************************************************************
 *                                                                  *
 * Adam does not recommend messing with the lines below. Not fun.   *
 *                                                                  *
 *******************************************************************/

/*
 * Take the user's input as raw input and handle it ourselves.
 * This will allow an easier method to handle commands such as
 * history navigation, ctrl functions and tab completion later on.
 */

int getch(void){
  struct termios old, new;
  int result;  
  
  tcgetattr(STDIN_FILENO, &old);

  memcpy(&new, &old, sizeof(new));
  new.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new);
  result = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &old);
  return(result);
}




/*
 * A heavily customized input method. Takes in raw input from the
 * key presses and handles the data on a lower level.
 * This allows for more customization when dealing with not-normal
 * keypresses such as arrow keys, ctrl combinations, and tabs.
 */
int getInput(char *cmd){
  char *cursor = cmd;
  char *tail = cursor;
  char *ptr;
  int tab_count = 0;
  int key, i = 0;

  *cursor = '\0';

  // Print the prompt
  getcwd(dir, MAX_DIR_LENGTH);
  printf("[%s] %s", dir, prompt);
    
  while ((key=getch()) != '\n'){	  
    // Handle the user's key presses:
    switch(key){

	 // Look for a BACKSPACE keypress:
 	 case 127:   if(cursor > cmd){
			       removeLetterAt(cmd, (cursor - cmd));
			       cursor--;
			       tail--;
			     }
	 			 break;		     
                 
	 // Look for a TAB keypress:
	 case 9:     if(++tab_count == 2) {
				   printf("Double-TAB, handle auto-complete.....\n");
				   tab_count = 0;
				 }
				 break;		     

	 // Look for arrow keys:
	 case 27:    i = 1;
 				 break;		     

	 case 91:    if(i == 1) i = 2;
				 break;		     

	 case 65:    if(i == 2){
				   printf("You pressed the UP key, handle back in history....\n");
				 } else {
				   if(insertLetterAt(cmd, key, (cursor - cmd))) { 
				     cursor++;
				     tail++;
				   }
				 }
				 i = 0;
				 break;		     

	 case 66:    if(i == 2){
				   printf("You pressed the DOWN key, handle forward in history....\n");
				 } else {
				   if(insertLetterAt(cmd, key, (cursor - cmd))) { 
				     cursor++;
				     tail++;
				   }
				 }
				 i = 0;
				 break;		     

	 case 67:    if(i == 2){
				   if(cursor != tail){
		             putchar(27);
				     putchar(91);
				     putchar(67);
  				     cursor++;
				   }
			     } else {
				   if(insertLetterAt(cmd, key, (cursor - cmd))) { 
				     cursor++;
				     tail++;
				   }
			     }
				 i = 0;
				 break;		     

	 case 68:    if(i == 2){
				   if(cursor != cmd){
		             putchar(27);
				     putchar(91);
				     putchar(68);
				     cursor--;
			       }
			     } else {
				   if(insertLetterAt(cmd, key, (cursor - cmd))) { 
				     cursor++;
				     tail++;
				   }
			     }
				 i = 0;
				 break;		     

	 // Handle the rest of the characters:
	 default:    if(!i && insertLetterAt(cmd, key, (cursor - cmd))) { 
				   cursor++;
				   tail++;
				 }
				 i = 0;
				 break;		     
      }
      	        
      printf("\r[%s] %s%s ", dir, prompt, cmd);			        
	  for(ptr = tail; ptr >= cursor; ptr--){
	    printf("\b");
	  }
      fflush(stdout);				         
	}	  
     
    while(*cursor != '\0'){
      printf("%c", *cursor);
      cursor++;
    }
    
    *cursor = '\0';
    printf("\n");	
	return (cursor - cmd);
}	


/*
 * Given an index in the character array, remove a character
 * at that location and shift everything after it.
 * Return 1 on success; 0 on failure.
 */
int removeLetterAt(char *msg, int index){
  char *cursor = msg;
  while (index-- > 1){
	cursor++;
	if(*cursor == '\0')
	  return -1;
  }

  while(*cursor != '\0'){
	*cursor = *(cursor + 1);
	cursor++;
  }
  
  cursor--;
  *(cursor) = '\0';
  
  return 0;
}


 
/*
 * Given an index in the character array, insert a new character
 * at that location and shift everything after it.
 * Return 1 on success; 0 on failure.
 */
int insertLetterAt(char *msg, char letter, int index){
  char *cursor = msg;
  while(*cursor != '\0'){
	cursor++;	
  }
  
  *(cursor + 1) = '\0';
  
  if(index == (cursor-msg)){
    *cursor = letter;
    return 1;
  }
    
  // Looking at last char (\0)
  // byte before this one is empty
  if((cursor - msg) < MAX_INPUT){  
	
    while((cursor-msg) >= (index + 1)){
      *cursor = *(cursor - 1);
      cursor--;
    }
        
    *cursor = letter;
    return 1;
  } else return 0;
}


void itoa(int x, char *buffer, int base) {
    int i = 0 , n, s;
    n = x;
    
    if(n == 0){
      buffer[i] = '0';
      buffer[i + 1] = '\0';
	} else {
    
      while (n > 0) {
        s = n%base;
        n = n/base;
        buffer[i++] = '0' + s;
      }
    
      buffer[i] = '\0';
      strrev(buffer);
    }
    
}
/*
//Turn number to string

 char* turnToChar(int number){
  long int i;
  int length=1;
  for(i=1;i<1000000;i*=10){
	if( (number-(i*10))<0 ){
		break;
	}
	length++;
  }
  ttcArr[length]=0;
  //for(;Length)
  //if length is 3
  //0 1 2
  //965
  //mod 10 -> 5 ---> Length-1
  //number=number/10;
  //...tccLength-2
  int ttcindexfl=1;
  int ttcleftover=-1;
  while(length-ttcindexfl>=0){
	ttcleftover=number%10;
	ttcleftover+=48;
	ttcArr[length-ttcindexfl]=(char)ttcleftover;
	number/=10;
        ttcindexfl++;
  }
  ttcRet = (char *)&ttcArr;
  return ttcRet;
}

*/


char *strrev(char *str){
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}
