#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int i=-1;
/*
1.
Environmental variables
	-Search for $...
		-if SET
		-if echo
2.
	qPrintenv();
3.
	Ret value in $?
*/
int check4Env(char *);


/*
[/home/porter] swish> echo $PATH
/bin:/usr/bin
[/home/porter] swish> set $PATH = /bin:/usr/bin:.
[/home/porter] swish> echo $PATH
/bin:/usr/bin:.
[/home/porter] swish> ls
foo.c Makefile
[/home/porter]swish> echo $?
0
[/home/porter]swish> ls /blah
/blah: no such file or directory
[/home/porter]swish> echo $?
1
*/
int check4Env(char* cmd){
//Loop through for $
	char cc;
	int isEnv=0;
	i=0;
	int j;
	int ff=0;//Format Failure
	while(cc!=NULL){
		cc = cmd[i++];
		if(cc=='$'){
			isEnv=1;
		}		
	}
	if(isEnv==0)
		return(0);

//Now that we've found $
	//Check for Set
	char set[]="SET";
	if(strncmp("SET",cmd,3)==0 || strncmp("set",cmd,3)==0){
	//From $ to space make that the variable name and = to 
	//"set $PATH = /bin:/usr/bin:."
		//Loop through and find index after $ then find index after that = to ' ' or '='
		j=i;
		
		//i is 'P' and j is going to be ' '(9)
		for(;;){
			++j;
			cc=cmd[j];
			if(cc ==' ' || cc == '=')
				break;
			if(cc=='\0'){
				ff=1;
				break;
			}
		}
		if(ff)
			return(0);
		//malloc PATH
	}
}

int main (int argc, char ** argv, char **envp) {
	char path[] = "/bin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games";
	char c[] = "exit";
	
	return 0;
}
