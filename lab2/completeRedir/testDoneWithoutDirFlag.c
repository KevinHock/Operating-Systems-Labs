#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

//Test fillArgs
//int runLine(char* cmd);

int i=-1;
int debug=1;
int fd;

int main(int argc, char ** argv, char **envp){
	//printenv --gdfgd 2>err
	char* cmd = "printenv --gdf gd";
	printf("The command:%s\n",cmd);
	runWTF(cmd);
	return(0);
}

//Run Prototype:	
int runLine(char* cmd){
	int hmm;	
	//Get how many meta
	for(i=0;i<length(cmd);i++)
		if(cmd[i]=='<' || ch=='>')
			hmm++;
	
	//Make array to hold where the metas are in the array (+1 for \0)
	int metaOS[] = malloc(sizeof(int)*(hmm+1));
	
	//Make array to hold what type of meta is at each index (+1 for \0)
	int metaT[] = malloc(sizeof(int)*(hmm+1));
	
	//Array Index
	int metaAI=0;
	
	//Put WHERE in metaOS and put WHAT in metaT
	for(i=0;i<length(cmd);i++){
		if(cmd[i]=='<' || cmd[i]=='>')
			metaOS[metaAI]=i;
		if(cmd[i]=='<')//LESS    THAN = 1
			metaT[metaAI]=1;
		if(cmd[i]=='>')//GREATER THAN = 2
			metaT[metaAI]=2;
		metaAI++;
	}
	
	//Null byte is the last metacharacter
	metaOS[metaAI]=strlen(mons);
	metaT[metaAI]=0;
	
	//Reset metaArrayIndex to zero
	metaAI=0;
	int lastIndex=0;
	////////////////////////////////////
	////////////////////////////////////
	//Execute       each      argument//
	////////////////////////////////////
	////////////////////////////////////
	START:
	if(metaAI!=hmm){
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//If next meta is LESS    THAN >
		if(metaT[metaAI]==1){
			
			//Malloc command string on the left
			char* cmd = malloc(sizeof(char)*(metaOS[metaAI]-lastIndex));
			strncpy(cmd,cmd[lastIndex],metaOS[metaAI]-lastIndex);
			
			//Malloc file string on the right
			char* file = malloc(sizeof(char)*(metaOS[metaAI+1]-metaOS[metaAI]));
			strncpy(file,cmd[metaAI],metaOS[metaAI+1]-metaOS[metaAI]);
			
			
			
		//Open file
			fd = open(file, O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		  	//CHECK FOR FAILURE
		  	if (fd < 0) {
				perror("Creating %s failed.\n",file);
				exit(-1);
			}
			
			
			
		//Write to file
			//Save stdout
			int oldout = dup(1);
			
			//All output goes to fd.
			dup2(fd,1);
			
			//Fork			
			pid_t pid = fork();
			//CHECK FOR FAILURE	
			if (pid < 0) {					
				perror("Fork in metaT[metaAI]==1 LESS THAN failed.\n");			
				exit(-1);			
			}			
			
		//The child's code
			if(pid == 0){
				/*
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * *		
				
				//Wrap if last char is number
				//i.e. "printenv --gdfgd 2"
				int result = runSingle(cmd);
				if(result==-1){
					printf("Unfortunatly cmd failed to run.");
				}
				close(fd);
				/*
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * 
				 * */		
				
			}else{
				wait(NULL);
			}
			
			
			//Close file
			close(fd);
			
			//Restore stdout
			dup2(oldout, 1);
			
			
			
			
			
			
			
			
			
			

		}

		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//If next meta is GREATER THAN <
		if(metaT[metaAI]==2){
			
			
			//Open right as input
			//IT HAS TO BE A FILE
			metaOS[metaAI+1]-metaOS[metaAI]
			
			//Write into left, IT HAS TO BE A COMMAND
			
		}
		
		lastIndex=metaOS[metaAI];
		metaAI++;
		goto START
	}
	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(metaAI==0)
		runSingle(cmd);
	
	
	
	
	free(metaOS);
	free(metaT);
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


//Test fork setenv effecting parent some how

