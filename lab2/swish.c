/* CSE 306: Sea Wolves Interactive SHell 
 * 
 * TODO: Look at proc-args to see if not checking for null
 * TODO: Test set and echo more 
 * TODO: -? if built in command
 * TODO: Custom FD "printenv --gdfgd 2>errlog"
 * TODO: Pipes "printenv --gdfgd 2|grep '.txt'>letsee"
 */

#include "swish.h"

/**********************************************************************
 * int main (int argc, char ** argv, char **envp)                     *
 * ----------------------------------------------                     *
 * The entry function for this program. We will handle the processing *
 * of runtime flags, load the command history from a file, and begin  *
 * a loop for the logic of this program.                              *
 *********************************************************************/
int main (int argc, char ** argv, char **envp) {  
  char *command = malloc(MAX_INPUT);
  char *cursor;
  int i;

  // Initialize History Linked List:
  history_head = malloc(sizeof(struct hist_node));
  history_head->prev = NULL;
  history_head->entry = NULL;
  history_head->next = NULL;
  history_ptr = history_head;
  history_tail = history_head;

  // Process Runtime Flags:
  for(i = 1; i < argc ; i++){
	if(*argv[i] == '-'){
	  cursor = argv[i] + 1;
	  while(*cursor != ' ' && *cursor != '\0' && *cursor != '\n'){
  	    if(*cursor == DEBUG_FLAG && debug == 0){
	      debug = 1;
	    } else if(*cursor == TIMED_FLAG && timed == 0){
	      timed = 1;
	    } else {
		  printf("Sorry, the flag '-%c' is not supported.\n", *cursor);
		  killMe(3);
		}
	    cursor++;
	  }
	} else runScript(argv[i]);
  } 

  if(debug) printf("DEBUG MODE ACTIVATED\n");
  if(timed) printf("PROCESS TIMING MODE ACTIVATED\n");


  home_dir = resolveVar("HOME");
  full_name = malloc(strlen(home_dir) + strlen(HISTORY_FILENAME) + 1);
  strncpy(full_name, (char *)home_dir, strlen(home_dir));
  strncpy((char *)(full_name + strlen(full_name)), (char *)HISTORY_FILENAME, strlen(HISTORY_FILENAME));
  if(debug) printf("[DEBUG] Opening history file (%s).....", full_name);
  history_fd = open(full_name, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
  if(debug){
	if(history_fd > 0) printf(" SUCCESS!\n");
	else printf(" FAILED!\n[DEBUG] Creating New History File");
  }
      
  
  char *temp_loader = malloc(MAX_INPUT + 1);
  cursor = temp_loader;

  while(read(history_fd, cursor, 1)){
	if(*cursor == '\n'){
	  *cursor = '\0';
	  historyAdd(temp_loader, 0);
	  cursor = temp_loader;
	} else cursor++;	
  }
  free(temp_loader);
    

  // Execute Main Program Loop:
  while (1) {
    getInput(command);
    parseLine(command);
  } 
 
  return 0;
}



/**********************************************************************
 * void historyAdd(char *command)                                     *
 * ----------------------------------------------                     *
 * Log the given command to the open history_buffer, and write        *
 * the buffer to the history_fd file descriptor                       *
 *********************************************************************/
void historyAdd(char *command, int persist){  
  if(strlen(command) > 0){
    struct hist_node *new_node = malloc(sizeof(struct hist_node));
	char *temp = malloc(strlen(command) + 1);
	strcpy(temp, command);

    history_tail->entry = temp;
    history_tail->next = new_node;
    new_node->prev = history_tail;
    new_node->entry = NULL;
    new_node->next = NULL;
    history_tail = new_node;

    if(debug) 
      printf("[DEBUG] Added %s to history memory.\n", history_ptr->entry);

    history_ptr = history_tail;
    
    if(persist && history_fd > 2){
      write(history_fd, temp, strlen(temp));
      write(history_fd, "\n", sizeof(char));
      if(debug) 
        printf("[DEBUG] Added %s to history file.\n", temp);
    }
  }  
}



/**********************************************************************
 * char *historyBack()                                                *
 * ----------------------------------------------                     *
 * Return the previous command in the history buffer, if any          *
 *********************************************************************/
char *historyBack(){
  char *result;
  
  if(history_head == NULL){
	result = "";
  } else if(history_ptr->prev != NULL){
    history_ptr = history_ptr->prev;
	result = history_ptr->entry;
  } else {
	result = "";
  }
  
  return result;
}



/**********************************************************************
 * char *historyForward()                                             *
 * ----------------------------------------------                     *
 * Return the next command in the history buffer, if any              *
 *********************************************************************/
char *historyForward(){
  char *result;
  
  if(history_head == NULL){
	  result = "";
  } else if(history_ptr->entry != NULL){
	result = history_ptr->entry;
    history_ptr = history_ptr->next;
  } else {
	result = "";
  }
  
  return result;
}



/**********************************************************************
 * char *historyClear()                                               *
 * ----------------------------------------------                     *
 * Clear the history stored in memory and delete the history file.    *
 *********************************************************************/
void historyClear(){
  if(debug) printf("[DEBUG] Clearing History!\n");

  history_fd = open(full_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

  history_ptr = history_tail;
  while(history_ptr != history_head){
	history_ptr = history_ptr->prev;
	free(history_ptr->next);
  } 
  history_tail = history_ptr;
  history_ptr->entry = NULL;
  history_ptr->next = NULL;
}



/**********************************************************************
 * char *historyPrint()                                               *
 * ----------------------------------------------                     *
 * Print the history stored in memory.                                *
 *********************************************************************/
void historyPrint(){
  struct hist_node *temp_ptr = history_head;
  int i = 1;

  printf("-----------------\n");
  printf("History Contents:\n");
  printf("-----------------\n");
  while(temp_ptr->next != NULL){
	printf("[%03d]: %s\n", i, temp_ptr->entry);
	temp_ptr = temp_ptr->next;
    i++;
  }
}

/**********************************************************************
 * void wolfiePrint()                                                *
 * ----------------------------------------------                    *
 * Print the signature wolfie.             		                     *
 *********************************************************************/
void wolfiePrint(){
  printf("					$I7$I7$IO$$ZZ$$\n");
  printf("				  +  ?+II?+I?I+7I7+777$ ?\n");
  printf("			      II?=.  ,==7???+II7$II7    II?Z\n");
  printf("			     8OI77Z8  IOO?+?+?I$77$7Z  OO$$7Z\n");
  printf("			    +DZ7$7O$8  O:,I++??IIZO8   FDZ$OO88\n");
  printf("			     D8D  ZOIO ~+++7?II??88Z  8OOD  8?I\n");
  printf("		           INDDODZ8O ZOZ+?7I7?=$ZO8  DO8ZDDNNII \n");
  printf("			    ?ZDZD+7Z O8.?O?7$+?$IZOOD 88888D8FR\n");
  printf("			      $8Z   ZOZ$7=?+?+?7$IZODO   D8 \n");
  printf("			     ??ZZO$I$77,7+I++7??$$I8ODD8$O88$ \n");
  printf("			    7?ZZ8Z$7+:~7+II+?$+?I?ZZ7Z8IZ7Z$Z7 \n");
  printf("	      		  $II=$I??7.$=I+?III??+7I7?7ZZ$Z$$8OO$$ \n");
  printf("           		  I??ZI=+7IO.I?.I7I$II$7?7$IZZ8Z8ZZOZZ$$7 \n");
  printf("			 ?Z+I+ZI=7:$,7=II?$II=II77I778O8O8OIOOZ$$ \n");
  printf("		      =7ZZIZ$7=7777?O$OZ88ZOOO$$I$OZ8OZ8$DO$ZOZZZ \n");
  printf("		      Z$O?7$ZZI$7$8O$Z$Z$ZOZZ$$OOZDO$8OO8OZ8Z7$$OZ$ \n");
  printf("		     ZZOZ$O$7$$OOZZ887  ZD8$  8DOOOO888OO8O$88$7ZZ$ \n");
  printf("		    ?I+$8O+IO78OOO$O8   8OOZ  ZO8OII$8NN8OZOZOOO7Z$Z \n");
  printf("		   =+?ZOZI$8OZ8N+  DD   D8NM  D8NN?  DDMDDD8OOO8ZOOZ \n");
  printf("		 $7$OOD~7ZIZ8O8D  MDND:,8DDNN DDDDN  DNNNDD888O88OOZZZ \n");
  printf("		$7?ZODIZ$$$$888O .DMMNNDD8D88DNNNMD  O88ODDDOD88$D8OOZOO \n");
  printf("	     IO8+$O$7ZZZZ$$ZZ8MNMMMMMMN8DDDDDNMMMMMMDN8DNNDNN88Z$Z8O88ZZ \n");
  printf("	   ++I$O?OZ$8O77$ZZ8NMMMMMMMMMMNNDDNMNMMMNMMMMMMNND8DOOZOZZOOZO7$ \n");
  printf("	  ???7$$7+7OO8OZ8OMNMNNMMMNNNDDMNDNNDDDDDDDNNNNNNDD88O8D8OOOZOZ77 \n");
  printf("	  =~II+$IZOODDDNDO7Z7$N,~ D8DD8D8D88OO888:   DDOZ8ONND$DD888O$OZ$ \n");
  printf("	 ~$:?:?7ZO8DDDNIOI7I$?I?~  7 ??8ZO$,   7?~  $ZZ7Z$OONNDD8DNDDOZ$Z \n");
  printf("	 ~?7I77I$8NDOO7??$I$$Z,Z   ++~  7I$ +    +=$7ZZZODOOZ8OZO8D8OOO$Z \n");
  printf("	  +~?I$$IZ$$:=7$=+Z.888~I+++++     $    +I~=Z$Z8Z77$8ZOOZOO8OZ \n");
  printf("	    ?$7777$?$$$8:8ZZ$$..=++?+$ . :$+   ID=7ZZO8DDOO8DD8DO8DD \n");
  printf("		$ZIO$ZZI8Z788OO$7I$I~77,7+II7Z OD8Z8O88O8DNDDNNNND88D \n");
  printf("		 7$=ZZZO88O8DDZ8Z$$I7I$I7I+$DDZOOOD8D88DNNMNNNMDDOOD \n");
  printf("	  	   $IZZZONDNNOO8D8ZOZ8NDDNNDN8DD$8NN8NOODNNDD8DODN \n");
  printf("	 		 $Z$ZZ8DDOD8N88ODNNDNNDDDN8NDDNMODDDDDDDN \n");
  printf("				O8$ODO8NDNMNNNNNDN8D8MDNDNDNNDND \n");
  printf("\n");
  printf("                   .__    _____      ____    ____  __\n");
  printf("                  /  \\    /  \\ ____  |  | _/ ____\\|__|  ____  \n");
  printf("                  \\   \\/\\/   //  _ \\ |  | \\   __\\ |  |_/ __ \\ \n");
  printf("                   \\        /(  <_> )|  |__|  |   |  |\\  ___/ \n");
  printf("                    \\__/\\  /  \\____/ |____/|__|   |__| \\___  >\n");
  printf("                         \\/                                \\/ \n");
  printf("                      _________.__             .__   .__   \n");
  printf("                     /   _____/|  |__    ____  |  |  |  |  \n");
  printf("                     \\_____  \\ |  |  \\ _/ __ \\ |  |  |  |  \n");
  printf("                     /        \\|   Y  \\\\  ___/ |  |__|  |__\n");
  printf("                    /_______  /|___|  / \\___  >|____/|____/\n");
  printf("                            \\/      \\/      \\/             \n");
  printf("\n");   
}






/**********************************************************************
 * int parseLine(char *cmd)                                           *
 * ----------------------------------------------                     *
 * This function will analyze a user-provided command and determine   *
 * if it is valid or not. If it is valid, we will execute the command *
 * in a forked process and return the error code.                     *
 *********************************************************************/
int parseLine(char *cmd){
  // Initialize our process list:
  process_list *proc_list = malloc(sizeof(process_list));
  proc_list->num_processes = 0;
  proc_list->processes = malloc(sizeof(process *) * proc_list->num_processes);  

  char *cmd_blocks;
  int result = 0;
    
  // Add the command to history:
  historyAdd(cmd, 1);

  cmd_blocks = strtok(cmd, "|");  
  while(cmd_blocks){	
    process *p = malloc(sizeof(process));
	result = parseCommand(p, cmd_blocks);
	
	if(result == 1){
	  printf("Sorry, I failed to open that file.\n");
	  return 1;
    } else if (result == 2){
      printf("Sorry, I dont know how to handle that command.\n");
      return 2;
    } else if (result == 3){
      printf("That is not a valid file descriptor.\n");
      return 3;
    } else {
	  if(debug)
  	    printf("Added %s to the process list.\n", p->args[0]);
    }
    
    addProcessToList(p, proc_list);	
    cmd_blocks = strtok(NULL, "|");
  }
  //ghi
  int letsSee;
  char str[15];
  if(proc_list->num_processes > 0){
    letsSee=executeCommand(proc_list);
    sprintf(str, "%d", letsSee);
    setenv("?",str,1);
  }
  if(debug) printf("? is set to %d\n",letsSee);
  return result;
}


/*
 * Execute a process and return the exit code:
 */
int executeCommand(process_list *pa){
  int exit_code = 0;  
  int pid;
  
  process *proc = pa->processes[0];

  if(debug){
    printf("RUNNING: [%s]\n", proc->args[0]);
  } fflush(stdout);
 
  // Handle Built-In Commands!
  if(!strcmp(proc->args[0], EXIT_CMD)){
	  killMe(0);
    return 0;   
  } 
  
  else if(!strcmp(proc->args[0], CD_CMD)){
    char *olddir = malloc(strlen(dir + 1));
    strcpy(olddir, dir);
    if(proc->args[1] == NULL){
       chdir(home_dir);
       if(strcmp(olddir, home_dir))
        setenv("OLDPWD", olddir, 1);
       free(olddir);
       return 0;
    } else if(!strcmp(proc->args[1], "-")) {
       chdir(resolveVar("OLDPWD")); 
       setenv("OLDPWD", olddir, 1);
       free(olddir);
       return 0;
    } else if(chdir(proc->args[1])) {
      printf("Directory Not Found [%s]\n", proc->args[1]);
        return 1;
    } else {
      if(strcmp(olddir, proc->args[1]))
        setenv("OLDPWD", olddir, 1);
      free(olddir);
      return 0;
      }
  } else if(!strcmp(proc->args[0], CLEAR_CMD)){
      historyClear();  
      return 0;
  } else if(!strcmp(proc->args[0], ECHO_CMD)){
      int i=1;
      while(proc->args[i]!=NULL){
        //Had to hard code against printing $IDONTEXIST
        if(proc->args[i][0]!='$')printf("%s ",proc->args[i]);
        i++;
      }
      printf("\n");
      return(0);
  } else if(!strcmp(proc->args[0], HISTORY_CMD)){
      historyPrint();
      return 0;
  } else if(!strcmp(proc->args[0], SET_CMD)){
      int i=0;
      while(proc->args[i++]!=NULL){}
      if(i>4){
        if(!strcmp(proc->args[2], "=")){
          setenv(proc->args[1],proc->args[3],1);
          return 0;
        } else{
          printf("Improper use of arguments.\n");
          return -1;
        }
      }else{
        printf("Not enough arguments to set\n");
        return -1;
      }
  } else if(!strcmp(proc->args[0], WOLFIE_CMD)){
	    wolfiePrint();
	    return 0;  
  } else {
    
    // Fork and execute the command:
    pid = fork();
    
    if(pid == 0){
  	  if(debug){
	    printf("[DEBUG] Replacing STDOUT with FD %d \n", proc->out_file_handle);
        printf("[DEBUG] Replacing STDIN with FD %d \n", proc->in_file_handle);
      }
      
      if(proc->out_file_handle >= 0){
	    dup2(proc->out_file_handle, 1);
	  }

	  if(proc->in_file_handle >= 0){
	    dup2(proc->in_file_handle, 0);
	  }
      proc->exit_code = execvp(proc->args[0], proc->args);
      printf("Command [%s] was not found.\n", proc->args[0]);
      killMe(proc->exit_code);
    } else {

	  // Wait for the child and store the exit code:
      waitpid(pid, &exit_code, 0);
      //ghi
      /*char str[15];
      sprintf(str, "%d", exit_code);
      if(debug) printf("Going to set ? to %s\n",str);
      setenv("?",str,1);
      */ 
      //ghi
    }
  }

  if(debug){
    printf("ENDED: \"%s\" (ret = %d)\n", proc->args[0], errno);
  } fflush(stdout);
  
  if(proc->out_file_handle > 2){
    close(proc->out_file_handle);
    proc->out_file_handle = -1;    
  }
  if(proc->in_file_handle > 2){
    close(proc->in_file_handle);
    proc->in_file_handle = -1;
  }

  return WEXITSTATUS(exit_code);
}



void runScript(char *input){
  int fd = open(input, O_RDONLY);
  char *line = malloc(MAX_INPUT + 1);
  char *cursor = line;

  printf("Running Batch Job [%s]:\n", input);
  if(fd < 0){
	printf("Could not open file [%s]. Please check your input.\n", input);
  } else {
    while(read(fd, cursor, 1)){
	  if(*cursor == '\n'){
	    while(cursor < (line + MAX_INPUT + 1)){
  	      *cursor = '\0';
  	      cursor++;
	    } 
        
      if(strlen(line) > 0 && *line != '#'){
          if(debug) printf("Executing [%s]\n", line); 
          parseLine(line);
	    } 
	    
	    cursor = line;	   
	  } else cursor++;
	}
  }
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


int addProcessToList(process *p, process_list *pl){
  pl->processes = realloc(pl->processes, sizeof(process *) * (pl->num_processes + 1));
  if(pl->processes <= 0) 
    return 1;
  pl->processes[pl->num_processes] = p;
  pl->num_processes++;
  return 0;
}


char **buildArgs(char *in){
  char **result = NULL;//ghi Danglin pointer man
  char *tok = strtok(in, " ");
  int elements = 0;
    
  while(tok){
 	  elements++;
	  result = realloc(result, sizeof(char *) * elements);
	
	  if(result == NULL) return NULL;
	
    //Make arg point to tok if it isn't an environmental variable
    if(!strncmp(tok,"$",1) && getenv(tok+1)!=NULL){
      result[elements-1] = getenv(tok+1);
    }else result[elements - 1] = tok;
    
    //On to the next one
	  tok = strtok(NULL, " ");
  }
  
  result = realloc(result, sizeof(char *) * (elements + 1));
  result[elements] = NULL;  
  return result;
}


int parseCommand(process *p, char *cmd){
  int result = 0;
  p->in_file_handle = -1;
  p->out_file_handle = -1;
  char *args, *file, *junk;
  int fd;

  // DISABLE MULTIPLE INPUT REDIRECTS
  if(countIn('<', cmd) > 1){
    return 2;
	

  // DISABLE MULTIPLE OUTPUT REDIRECTS BUT CHECK FOR APPENDING
  } else if (countIn('>', cmd) == 2) {
	  junk = cmd;
	  while(*junk != '>') junk++;
	  junk++;
	  if(*junk != '>') {
	    return 2;
	  } else {
	    args = strtok(cmd, ">");
	    file = strtok(NULL, ">");
	
	    p->args = buildArgs(args);

	    if(file != NULL){
  	      file = trim(file);
        } else return 1;
	
	    p->out_file_handle = open(file, O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
        if(p->out_file_handle < 0) return 1;    		  
	  }
  } else if(countIn('>', cmd) > 2){
	  return 2;

	  
  // HANDLE NO REDIRECTS
  } else if(countIn('<', cmd) == 0 && countIn('>', cmd) == 0){
	p->args = buildArgs(cmd);
	

  // HANDLE A SINGLE INPUT REDIRECT
  } else if(countIn('<', cmd) == 1 && countIn('>', cmd) == 0){
	args = strtok(cmd, "<");
	file = strtok(NULL, "<");
	
	p->args = buildArgs(args);

	if(file != NULL){
  	  file = trim(file);
    } else return 1;
	
	p->in_file_handle = open(file, O_RDONLY);
    if(p->in_file_handle < 0)
      return 1;
        

  // HANDLE A SINGLE OUTPUT REDIRECT
  } else if(countIn('<', cmd) == 0 && countIn('>', cmd) == 1){
	args = strtok(cmd, ">");
	file = strtok(NULL, ">");
	
	junk = args;
	while(*junk != 0) junk ++;
	while(*junk != ' ' && junk != args) junk--;
	
	if(junk != args){
	  *junk = '\0';
	  junk++;
	  fd = atoi(junk);
	  
	  if(debug) printf("Redirecting from FD %d\n", fd);

	  p->in_file_handle = fcntl(fd, F_DUPFD, 5);
      if(p->in_file_handle < 0)
		return 3; 

	  if(debug) printf("Dup'ed to %d\n", p->in_file_handle);
	}
	
	p->args = buildArgs(args);

	if(file != NULL){
  	  file = trim(file);
    } else return 1;
	
	p->out_file_handle = open(file, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if(p->out_file_handle < 0) return 1;    
	

  } else if(countIn('<', cmd) == 1 || countIn('>', cmd) == 1){
    return 2;
  }
  
  return result;
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
 * Adam says dont mess with the code below. Trust him, it works!!   *
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
				   while(cursor > cmd){
			         removeLetterAt(cmd, (cursor - cmd));
			         cursor--;
			         tail--;
                     printf("\r[%s] %s%s ", dir, prompt, cmd);			        
                     fflush(stdout);				         
			       }
			       char *hist_temp = historyBack();
			       while(*hist_temp != '\0'){
 				     if(insertLetterAt(cmd, *hist_temp, (cursor - cmd))) { 
				       hist_temp++;
				       cursor++;
				       tail++;
				     }
				   }
				 } else {
				   if(insertLetterAt(cmd, key, (cursor - cmd))) { 
				     cursor++;
				     tail++;
				   }
				 }
				 i = 0;
				 break;		     

	 case 66:    if(i == 2){
				   while(cursor > cmd){
			         removeLetterAt(cmd, (cursor - cmd));
			         cursor--;
			         tail--;
                     printf("\r[%s] %s%s ", dir, prompt, cmd);			        
                     fflush(stdout);				         
			       }
			       char *hist_temp = historyForward();
			       while(*hist_temp != '\0'){
 				     if(insertLetterAt(cmd, *hist_temp, (cursor - cmd))) { 
				       hist_temp++;
				       cursor++;
				       tail++;
				     }
				   }
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


char *strrev(char *str){
  char *p1, *p2;

  if (!str || !*str)
    return str;
  
  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2){
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
  }
  return str;
}


int countIn(char ch, char *str){
  int result = 0;
  char *ptr = str;
  while(*ptr != '\0'){
	if(*ptr == ch) result++;
	ptr++;
  }
  return result;
}

char *trim(char *str){
   char *result = str + strlen(str);
   while(*result == '\0' || *result == ' '){
     *result = '\0';
     result--;
   }

   result = str;
   while(*result == ' ')
     result++;   

   return result;
}
