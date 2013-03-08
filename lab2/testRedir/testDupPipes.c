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
    char* const lsParam[] = {"/bin/ls",NULL};
    char* const grepParam[] = {"/bin/grep",NULL};
    char* const echoParam[] = {"/bin/echo",NULL};
    fd = open("outfilePipes", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //
    int oldout = dup(1); 
    //how you write to the file
    dup2(fd,1);//put stdout into fd
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
    //dup2(1,1);
    //fprintf(stdout,"hi\nhi\nhi\n");
    fd=open("outfilePipes", O_RDONLY, S_IRWXU);
    //
    //char*  testLs = malloc(31);
    //read(fd,testLs,30);
    //fprintf(stderr,"\nghighi\n");
    //fprintf(stderr,"\nFile is = %s123123\n",testLs);
    //fflush(NULL);
    //
    //dup2(1,fd);
    dup2(fd,0);//0,fd);//put fd into stdin
    //
    //fd;
    //char* testAbc = malloc(31);
    //read(0,testAbc,30);
    /*fprintf(stderr,"\nghighi\n");
    fprintf(stderr,"\nFile is = %s\n",testAbc);
    fflush(NULL);*/
    //
    pid_t pid2 = fork();     
    if (pid2 == 0) {
	//fprintf(stderr,"I'm grepin\n\n");
	//fflush(NULL);
	//execvp("echo blahvlahddddddd",echoParam);
        /*printf("Hello World.\n");
	
	char* testAbc = malloc(31);
        read(0,testAbc,30);
        printf("testAbc=%sB4GREPB4GREP\n",testAbc);
	*/
	//it has printf and stdin ^^^^
	//execv("/bin/cat",0);
        printf("\n\nqqqqqqqqq\n\n");
 	execl("/bin/grep","","qq",NULL);	
	//execl("/bin/cat","",NULL);
	close(fd);
    }
    else {
         wait(NULL);//
    }
    printf("Hello Hello Hello\n");
    close(fd);
    return(0);
}
