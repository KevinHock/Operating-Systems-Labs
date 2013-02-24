/* CSE 306: Sea Wolves Interactive SHell */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT 1024

int i=-1;
int debug=1;//1 for development purposes

void checkDebug(int argc, char** argv);
void pwd(char **envp);
int printPrompt(char **envp);
void findEnvVar(char* envVar,char **envp);
void bin(char* var, char* progName);

int cbi(char* cmd,char **envp);
int qExit(char* cmd);
int qCd(char* cmd,char **envp);
int qPwd(char* cmd,char **envp);

//int qPrintenv(char* cmd);

char *foundEnvVar;

int cbi(char* cmd,char **envp){
     //CHECK RETURN VALUES
	int popEipVal=-1;
	popEipVal = qExit(cmd);
	if(popEipVal>=0)
		return(0);
	popEipVal = qCd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal = qPwd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	return(-1);
	//popEipVal = qPrintenv();//after env variables	
}
int qPwd(char* cmd ,char **envp){//,char* envp
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
				printf("result of changing dir is %d.\n",result);
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
int qExit(char* cmd){
	char xit[] = "exit";
	if(strcmp(xit,cmd)==0){
		exit(0);
	}
	//If didn't exit
	return -1;
}

void bin(char* var, char* progName){
	int pid= fork();
	if(pid==0){
		char* firstTok = malloc(sizeof(char)*strlen(var));
		firstTok = strtok(var,":");
		char* firstTry = malloc(  (sizeof(char)*strlen(var) ) + ( sizeof(char)*strlen(progName)  )+1+1);
		strcpy(firstTry,firstTok);
		strcat(firstTry,"/");
		strcat(firstTry,progName);
		int trying = execl(firstTry,"",NULL);
		trying;
		firstTok=strtok(NULL,":");
		while(firstTok!=NULL){
			strcpy(firstTry,firstTok);
			strcat(firstTry,"/");
			strcat(firstTry,progName);
			trying = execl(firstTry,"",NULL);
			trying;
			if(trying!=-1)
				exit(0);
			firstTok=strtok(NULL,":");
		}
		exit(1);
	}
	int status;
	pid = wait(&status);
	printf("Parent process started.\n");
     	if (pid == -1)
 	   	perror("wait error");
     	else{                       /* Check status.                */
     		if(WIFSIGNALED(status) != 0)
          		printf("Child proc ended. Signal = %d.\n",WTERMSIG(status));
          	else if(WIFEXITED(status) != 0)
              		printf("Child proc ended. Status = %d.\n",WEXITSTATUS(status));
          	else
              		printf("Child proc got screwed up.\n");
	}
}

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
		cbi("cd /home/cse306/lab/lab1",envp);
	        	
		findEnvVar("PATH",envp);
		if(debug==1)
			printf("\n\nAbout to look through %s.\n\n",foundEnvVar);
		//If built in I guess bin will do nothing.
		bin(foundEnvVar,"ls");
		
		/*int cbiret = cbi("cd",envp);
		if(debug==1)
			if(cbiret<0)
				printf("\n\ncbi failed.\n\n");
		*/
		rv = printPrompt(envp);
		fflush(NULL);			
		
    		if(!rv){ 
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
