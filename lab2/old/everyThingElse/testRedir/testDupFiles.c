#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char ** argv, char **envp){
    int fd;
    /*
int f=open(somefile, O_WRONLY | O_CREAT, S_IRWXU);
dup2(f,1);  //redirecting output to file
//execute your first command here using fork
close(f);
int f=open(somefile, O_RDONLY | O_CREAT, S_IRWXU);
dup2(f,0);    //this will give the ouput of the first command to stdout 
//i.e. the input is present for second one
//execute your second command here
close(f);

ls | grep '.txt'


    */
    //char* const param[] = {"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games", NULL};
    char* const param[] = {"/bin/ls",NULL};
    char* const paramEnv[] = {"printenv"};
 
    fd = open("outfile", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int fd2 = open("error", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open failed\n");
        exit(0);
    }
	
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed\n");
        exit(0);
    }
    else if (pid == 0) {
        dup2(fd, STDOUT_FILENO);

        close(fd);

        //execl("/usr/bin/printenv","printenv",NULL);//,envp,NULL);
	execvp("ls", param);
	//fflush(NULL);
    	close(fd);
    }
    else {

/*
    int oldout = dup(2); 
    //how you write to the file
    dup2(fd,2);//put stdout into fd
    pid_t pid = fork();
    
    if (pid == 0){ 
        fprintf(stderr,"I'm lsing\n\n");
        fflush(NULL);   
        execvp("ls", lsParam);
        close(fd);
    }   
    else {
        wait(NULL);//
    }   
    close(fd);
    //make stdout 1 again
    dup2(oldout, 1); 
*/



	dup2(fd2,STDERR_FILENO);
	write(2,"hello motto", 8);
	execl("/usr/bin/printenv","printenv","---dsdssd",NULL);
	//write(2,"hello motto", 8);
	printf("\nhello\n");
	wait(NULL);
    }
    return(0);
}
