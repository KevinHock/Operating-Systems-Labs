/* CSE 306: Sea Wolves Interactive SHell */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT 1024

int i=-1;
int debug=1;//1 for development 0 for production

void checkDebug(int argc, char** argv);
void pwd(char **envp);

// Go through and find PWD in **envp
// Then print after = [4] to n
void pwd(char **envp){
	char** env;
	char thisEnvArr[2];
     	char zero;
	char one;
	for(env = envp; *env != 0; env++){
    		//char* thisEnv = *env;
		char* envCaps ="EN";
		//char[2] thisEnvArr;
		zero=env[0][0];
		one=env[0][1];
		thisEnvArr[0]=zero;
		thisEnvArr[1]=one;
		thisEnvArr[2]=(char)0;
		printf("thisEnvArr=%s\n",thisEnvArr);
    		if(thisEnvArr==envCaps){
			printf("Woohoo\nWoohoo\n");
		}  
    	}
}


//If "cd" then "HOME"
//If "cd -" then "OLDPWD"

int main (int argc, char ** argv, char **envp) {
	checkDebug(argc,argv);
	printf("Debug = %d\n",debug);
	int finished = 0;

	char *prompt = "swish> ";
	char cmd[MAX_INPUT];
	
  	while(!finished){
    		char *cursor;
    		char last_char;
    		
		int rv;
    		int count;		
    		
		// Print the current directory
		// "[/tmp] "
		// "To inspect your environment variables use the printenv command"
		pwd(envp);
		
   		// Print the prompt
		// "swish> "
    		rv = write(1, prompt, strlen(prompt));
    		
		/*
		pid = fork()
		if(pid==0)
			I'm the child
		else
			I'm parent of Pid

		int trying = execl("/usr/bin/firefox","",null);
		trying;
		if(trying==-1)
			perror("execl");
		*/


    		if (!rv) { 
      			finished = 1;
      			break;
    		}
   		
		break;	
		
    		// Read and parse the input
    		for(rv = 1, count = 0, cursor = cmd, last_char = 1;
		rv && (++count < (MAX_INPUT-1))&& (last_char != '\n');
		cursor++){ 
      			rv = read(0, cursor, 1);
      			last_char = *cursor;
    		} 
		
    		*cursor = '\0';
		
    		if (!rv) { 
      			finished = 1;
      			break;
    		}
		
    		// Execute the command, handling built-in commands separately 
    		// Just echo the command line for now
    		write(1, cmd, strnlen(cmd, MAX_INPUT));
  	}
  	return 0;
}

void checkDebug(int argc, char** argv){
	for(i=1;i<argc;i++){
    		if(argv[i][0]!='-')
        		break;
    		if(argv[i][1] == 'd')
			debug=1;
	}
}
