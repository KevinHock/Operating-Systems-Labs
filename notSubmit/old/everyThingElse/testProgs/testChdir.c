#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char ** argv, char **envp){
	char cmd[]="/home/cse306";
	printf("About to change the directory to %s.\n",cmd);
	int result = chdir(cmd);
	if(result == 0){
    		printf("directory changed\n");
    		exit(0);
  	}
  	else{
    		switch(result){
      		case EACCES:
			perror("Permission denied");
 	       	        break;
      		case EIO:
			perror("An input output error occured");
      		        break;
      		case ENAMETOOLONG:
			perror("Path is to long");
      		 	break;
      		case ENOTDIR:
			perror("A component of path not a directory"); 
	          	break;
      		case ENOENT:
			perror("No such file or directory");
			printf("enoent\n");      
      		default:
			perror("Couldn't change directory");
		}
	}
	return(0);
}
