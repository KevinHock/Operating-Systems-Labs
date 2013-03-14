#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
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
    
    fd = open("outfile", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

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

        execvp("ls", param);
    }
    else {
        wait(NULL);
    }
    return(0);
}
