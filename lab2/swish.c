/* CSE 306: Sea Wolves Interactive SHell 
 * 
 * TODO: Implement 'set' command for adding variables,
 * TODO: Implement more than one argument
 * TODO: Implement piping, redirecting, etc
 * TODO: Implement scripting support (just need to read the lines of the file)
 * TODO: Implement job control
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
  
  if(proc_list->num_processes > 0)
    executeCommand(proc_list);
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
    
  } else if(!strcmp(proc->args[0], HISTORY_CMD)){
    historyPrint();
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
  char **result = NULL;
  char *tok = strtok(in, " ");
  int elements = 0;
    
  while(tok){
 	elements++;
	result = realloc(result, sizeof(char *) * elements);
	
	if(result == NULL) return NULL;
	
	result[elements - 1] = tok;
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
