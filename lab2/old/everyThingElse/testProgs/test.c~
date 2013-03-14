#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


void cbi(char* cmd);
void qExit(char* cmd);
int qCd(char* cmd);
int qPwd(char* cmd);


//add ret values to the command built in chain
void cbi(char* cmd){
	qExit(cmd);
	qCd(cmd);
	qPwd(cmd);
	//qPrintenv();//after env variables	
}

int qPwd(char* cmd){//,char* envp
	if(strcmp("pwd",cmd)==0){
		//get curr working dir
		findEnvVar("PWD",envp);
		//print it
		printf("\n\n%s\n\n",foundEnvVar);
		return 0;
	}
	return -1;
}

int qCd(char* cmd){
	printf("??");
   	char cd[]="cd";
	if(strncmp("cd",cmd,2)==0){
		if(strcmp("cd",cmd)==0){
			//get home dir
			findEnvVar("HOME",envp);
			//call chdir
		 	return(chdir(foundEnvVar));
		}
		if(strcmp("cd -",cmd)==0){
			//get last dir
			findEnvVar("OLDPWD",envp);
			//call chdir
			//ret success or not
			return(chdir(foundEnvVar));
		}
		if(strncmp("cd ",cmd,3)==0){
			//pass cmd+3 to chdir
			//ret success or not
			return(chdir( *(cmd+3) ));
		}
		return -1;
	}
	//Note chdir returns <0 if unsuccessful
	//If it is just "cd\00" then pass home directory to chdir
	//If it is "cd -" then pass last dir to chdir
	//If it is "cd "..."" pass ... to chdir
}
void qExit(char* cmd){
	char xit[] = "exit";
	if(strcmp(xit,cmd)==0){
		exit(0);
	}
}

/*
Do strktok(var,":")

See if it fails 

While(firstTok!=NULL)
	try execl

if what ev ==-1
	printf("failed");fflush(); 
*/
//int success=0;
void bin(char var[], char progName[]){
	int pid= fork();
   if(pid==0){
	char* firstTok = malloc(sizeof(char)*strlen(var));
	firstTok = strtok(var,":");
	char* firstTry = malloc(  (sizeof(char)*strlen(var) ) + ( sizeof(char)*strlen(progName)  )+1+1);
	strcpy(firstTry,firstTok);
	strcat(firstTry,"/");
	strcat(firstTry,progName);
	//success=1;
	int trying = execl(firstTry,"",NULL);
	trying;
	//if(trying==-1){printf("Ahh error!\n\n");	fflush(NULL);  exit(1);}
	//success=0;
	firstTok=strtok(NULL,":");
	while(firstTok!=NULL){
		strcpy(firstTry,firstTok);
		strcat(firstTry,"/");
		strcat(firstTry,progName);
		//success=1;
		trying = execl(firstTry,"",NULL);
		trying;
		//success=0;
		if(trying!=-1){
			printf("Hello\n\n");
			exit(0);
		}
		//printf("blah\n");
		//fflush(NULL);
		firstTok=strtok(NULL,":");
	}
	exit(1);
   }



	int status;
        pid = wait(&status);
	//printf("Pid = %d\n",pid);
        printf("Parent process started.\n");
        if (pid == -1)
        	perror("wait error");
        else {                       /* Check status.                */
           if (WIFSIGNALED(status) != 0)
              printf("Child process ended because of signal %d.\n",
                      WTERMSIG(status));
           else if (WIFEXITED(status) != 0)
              printf("Child process ended normally; status = %d.\n",
                      WEXITSTATUS(status));
           else
              printf("Child process did not end normally.\n");
        }
	
   
}



/*
void bin(char var[], char progName[]){
        //      It then splits it by ':' and tries to execl.
          //      If it succeeds in one attempt then it returns 1 and if not it return -1;        
        //strtok
	//int pid = fork();
        //if(pid==0){
                printf("I'm the child");
		//
		char* firstTok = malloc(sizeof(char)*strlen(var));
		firstTok = strtok(var,":");
		//
		char* firstTry = malloc(  (sizeof(char)*strlen(var) ) + ( sizeof(char)*strlen(progName)  ));
		strcat(firstTry,progName);
                //int trying = execl(firstTry,"",NULL);
                //printf("wtf");

		firstTok = strtok(NULL,":");

		printf("wtf");
		//trying;
                //if(trying==-1){
                 //       perror("execl");
                //}
                //exit(3);
                //return 0;
        //}
        //else
          //      printf("I'm parent of Pid");
}*/



int main (int argc, char ** argv, char **envp) {
	char path[] = "/bin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games";
	char c[] = "exit";
	bin(path,c);
	cbi(c);
	/*int pid = fork();
	if(pid==0){
		printf("I'm the child");
		int trying = execl("/bin/ls","",NULL);
		trying;
		if(trying==-1){
			perror("execl");
		}
		exit(3);
		return 0;
	}
	else{
		printf("I'm parent of Pid");
	}*/
	return 0;
}
