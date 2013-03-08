#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//Test fillArgs

void fillArgs(char** argList, char* cmd, int numArgs);

int main(int argc, char ** argv, char **envp){
	//printenv --gdfgd 2>err
	int numOArgs = getNumOArgs(cmd);		
	char** argList = malloc(sizeof(int)*numOArgs);	
	
	//Fill each of the arguments
	fillArgs(argList,cmd);	
	return(0);
}

//Fills argList with each command by splitting by ' '
void fillArgs(char** argList, char* cmd, int numArgs){
	int i=0;//Global i might be effected
	//WATCH FOR DANGLING POINTERS
	
	char* currArg = &argList;
	
	for(strtok(cmd,' ')){
		if(numArgs==i){
			//if(currArg is a number)
			//****dup entire function to the number**** somecommand 2>err.log
		}
		if contains '/'{
			dirFlag=1;
			dir = malloc(sizeof(char)*length(tok)+1);
			strncpy(dir,tok,sizeof(char)*length(tok));
		}else{
			//Malloc length and put it in argList
			currArg = malloc(sizeof(char)*length(tok)+1);
			strncpy(currArg,tok,sizeof(char)*length(currArg));
		}
		currArg++;
		i++;
	}
}













































































































//Test run()

/*
//Run Prototype:	
int run(char* cmd, int numOArgs){	
	int numOArgs = getNumOArgs(cmd);		
	char** argList = malloc(sizeof(int)*numOArgs);	
	fillArgs(argList,cmd);	
	if(!dirFlag){
			//Loop through PATH? Probably not.
			execvp(arg0,argList);//man says "const char *file, char *const argv[]"
	}	
	else{
		execv(dir token, argList);
	}
	free(argList);
}
*/






//Test open("outfilePipes", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
//Test fork setenv effecting parent some how

