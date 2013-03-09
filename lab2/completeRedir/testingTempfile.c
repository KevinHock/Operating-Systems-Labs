#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int main(){
	int fd;
	
	fd = open("out", O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR); //O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  	//CHECK FOR FAILURE
  	if (fd < 0) {
		perror("Creating temporary file failed.\n");
		exit(-1);
	}
	
	
	
///////
///////
///////
	//Save stdout
	int oldout = dup(1);
	
	//All output goes to fd.
	dup2(fd,1);
				
	//Fork			
	pid_t pid = fork();
	//CHECK FOR FAILURE	
	if (pid < 0) {					
		perror("First fork in '|' failed.\n");			
		exit(-1);			
	}			
				
				//The child's code
				if (pid == 0){
					printf("LOLLYES");
					execl("/bin/ls","ls",NULL);
					close(fd);
				}else{
					wait(NULL);
				}
				//Close temporary file
				close(fd);
				
				//Restore stdout
				dup2(oldout, 1);
//////////
//////////
//////////
	
	
	printf("qqoutsidechild");
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*
	//Read the output
	fd=open("outfilePipes", O_RDONLY, S_IRWXU);
	//CHECK FOR FAILURE
  	if (fd < 0) {
		perror("Reading temporary file failed.\n");
		exit(-1);
	}
	
	
//////////
//////////
//////////
	dup2(fd,0);//All input goes to fd
				
	pid_t pid2 = fork();			
	//CHECK FOR FAILURE			
	if (pid < 0) {			
		perror("Second fork in '|' failed.\n");			
		exit(-1);			
	}
				
	if (pid2 == 0) {
		execl("/bin/grep","","",NULL);
		close(fd);
	}			
	else {
		wait(NULL);
	}
	close(fd);
//////////
//////////
//////////
	
	
	*/
	
	return(0);
}


















