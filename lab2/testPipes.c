#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

void ls2file();

int main(void){
        int     fd[2], nbytes;
        pid_t   childpid;
        pipe(fd);
        if((childpid = fork()) == -1){
                perror("fork");
                exit(1);
        }
        if(childpid == 0)
        {
                // Child process closes up input side of pipe 
                close(fd[0]);
                
                dup2(fd[1],1);
                //Black box
                ls2file();
                exit(0);
        }
        else
        {
                //Parent process closes up output side of pipe 
                close(fd[1]);
		//Make fd[0]
		dup2(fd[0],0);
		pid_t pid = fork();
	        //The child's code
	        if(pid == 0){
                int result = execl("/bin/grep","grep","w",NULL);
                if(result==-1){
              		printf("Unfortunatly cmd failed to run.");
        	}
 	        }else{
 	 	       wait(NULL);
      		}
      		//Close file
	        close(fd[0]);
        } 
        return(0);
}

void ls2file(){
    int fd = open("qqc", O_RDONLY);
      //CHECK FOR FAILURE
      if (fd < 0) {
        //perror("Creating %s failed.\n",file);
        exit(-1);
      }
  //Write to file
    //Save stdout
    int oldout = dup(0);
    //All output goes to fd.
    dup2(fd,0);
    //Fork			
    pid_t pid = fork();
    //CHECK FOR FAILURE	
    if (pid < 0) {					
      perror("Fork failed.\n");			
      exit(-1);			
    }			
  //The child's code
    if(pid == 0){
      int result = execl("/bin/grep","grep","t",NULL);
      if(result==-1){
        printf("Unfortunatly cmd failed to run.");
      }
      close(fd);		
    }else{
      wait(NULL);
    }
    //Close file
    close(fd);
    //Restore stdout
    dup2(oldout, 2);
}
