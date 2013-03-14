#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//Test fillArgs
//int runLine(char* cmd);

int i=-1;
int debug=1;
int fd;


int runWTF(char* cmd){
	//i.e. "printenv --gdfgd 2"
  //or    "/bin/ls"
  //Make 2 of "printenv --gdfgd" go to output
    int i=-1;
    
    int dirFlag=0;
//IF executive contains a '/'
//THEN dirFlag = 1
    char currCh;
    int lastSlash=-1;
    int firstSpace=-1;
    //
    //lastSlash=firstSpace=-1;
    //lastSlash=-1;
    //
    int stop=-1;
    char* exename;
    int numberOfSpaces=0;


    for(i=0;i<strlen(cmd);i++){
        if(cmd[i]=='/' && stop==0){
            lastSlash=i;
	    for(;;){}
	}else if((cmd[i]==' '||cmd[i]=='\0')&& stop==0){
            firstSpace=i;
            stop=1;
        }
        numberOfSpaces++;
    }
    
    
    if(lastSlash==-1){
        //Set dirFlag
        dirFlag=0;
    }else{
         // 0123456789
         //"/bin/ls -a"
         //     lS fS
         //     4  7
        //printf("the strincmd[");
	 
        //Take from first space to last integer
        exename=malloc(sizeof(char)*(firstSpace-lastSlash+1));

		printf("firstSpace-lastSlash = %d\n",firstSpace-lastSlash);
        //if(exename!=NULL){	
		strncpy(exename,cmd[lastSlash],firstSpace-lastSlash);
        //}
        //Set dirFlag
        dirFlag=1;
    }
    
//Make array of args
    //Get number of args              FOR NULL
    int numberOfArgs=numberOfSpaces+1+1;
    char** args = malloc(sizeof(int)*numberOfArgs);
    
    //"printenv --gdfgd"
    //         \0
    // 0123456789012345
    
    //Array for each offset of each space
    int** nulls = malloc(sizeof(int)*numberOfSpaces);
    int* p2Null = &nulls;
    int nullInd=0;
    for(i=0;i<strlen(cmd);i++){
        if((cmd[i]==' ')){
            p2Null=i;
            *p2Null++;
            nullInd++;
        }
    }

    printf("The first space is located at %d, should be at 8\n",nulls[0]);
	return(0);
}

int main(int argc, char ** argv, char **envp){
	//printenv --gdfgd 2>err
	char* cmd = "printenv --gdfgd";
	runWTF(cmd);
	return(0);
}
