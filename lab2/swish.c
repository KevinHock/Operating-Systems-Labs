/* CSE 306: Sea Wolves Interactive SHell */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT 1024

void checkDebug(int argc, char** argv);
void pwd(char **envp);
int printPrompt(char **envp);
void findEnvVar(char* envVar,char **envp);

void bin(char* var, char* progName);

void bin(char* var, char* progName){
	/*	It then splits it by ':' and tries to execl.
		If it succeeds in one attempt then it returns 1 and if not it return -1;	i*/
	//strtok
	
}












char *foundEnvVar;
//Create a function that accepts (variable name,**env) then returns(the string, size).
void findEnvVar(char* envVar,char **envp){
	for(; *envp != 0; envp++)
		if(strncmp(envVar, envp[0], strlen(envVar))==0){
			//If it matches
			//Use char* strchr(cs,c); and size_t()
			//Go to =+1 and malloc the size -4 +1 //Use strtok for next function
			char* equals=strchr(envp[0],'=');
			equals++;
			foundEnvVar=(char *)malloc(sizeof(char)*strlen(equals));
			strcpy(foundEnvVar,equals);
		}
}
/*
void pwd(char **envp){
	char* pwd ="PWD=";
	for(; *envp != 0; envp++)
		if(strncmp(pwd, envp[0], 4)==0)
			for(i=4;envp[0][i]!=0;i++)
				printf("%c",envp[0][i]);
}
*/



int i=-1;
int debug=0;//1 for development 0 for production

char *prompt = "swish> ";
char *osb="[";
char *csb="]";

//If "cd" then "HOME"
//If "cd -" then "OLDPWD"

int main (int argc, char ** argv, char **envp) {
	checkDebug(argc,argv);
	printf("Debug = %d\n",debug);
	int finished = 0;
	
	char cmd[MAX_INPUT];

	
	
  	while(!finished){
    		char *cursor;
    		char last_char;
    		
		int rv;
    		int count;		
    		
		// Print the current directory
		// "[/tmp] "
		// "To inspect your environment variables use the printenv command"
		
   		// Print the prompt
		// "swish> "

	 	
		findEnvVar("SSH_CONNECTION",envp);
		printf("\n\n%s\n\n",foundEnvVar);
		rv = printPrompt(envp);
		fflush(NULL);			
		
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

int printPrompt(char **envp){
	write(1, osb, 1);
	pwd(envp);
	fflush(NULL);
	write(1, csb, 1);
    	return(write(1, prompt, strlen(prompt)));
}

// Go through and find PWD in **envp
// Then print after = [4] to n
void pwd(char **envp){
	char* pwd ="PWD=";
	for(; *envp != 0; envp++)
		if(strncmp(pwd, envp[0], 4)==0)
			for(i=4;envp[0][i]!=0;i++)
				printf("%c",envp[0][i]);
}

void checkDebug(int argc, char** argv){
	for(i=1;i<argc;i++){
    		if(argv[i][0]!='-')
        		break;
    		if(argv[i][1] == 'd')
			debug=1;
	}
}
