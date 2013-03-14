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
	char cc='a';
	int isEnv=0;
	i=0;
	int j;
	while(cc!=NULL){
		cc = cmd[i++];
		if(cc=='$'){
			isEnv=1;
			break;
		}		
	}
	if(isEnv==0)
		return(0);
	printf("this far. line 54\n");
//Now that we've found $
	//Check for Set
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
			if(cc=='\0')
				return(-1);
		}
		//malloc PATH
		char* varName = malloc(sizeof(char)*(j-i));
		strncpy(varName,cmd+i,(j-i));
		printf("\nThe var name is = %s\n",varName);
		
		//j=9 i=5
		i=j;
		//Now there both 1 char after varName
		//Make sure to check for failure
		//Search for first char not ' ' or '=' and if it's '\0' than ff=1;
		//After that go until '\0' and malloc duh diff
		++i;
		if(cmd[i]=='\0' || cmd[j]=='\0')
			return(-1);
		for(;;){
			++i;
			cc=cmd[i];
			if(cc=='\0')
				return(-1);
			if(cc!=' '||cc!='=')
				break;
		}
		//cmd[i] == /
		j=i;
		++j;
		for(;;){
			if(cmd[j]=='\0')
				break;
			++j;
		}
		//cmd[j-1]== NULL
		j++;
		char* pathName = malloc(sizeof(char)*(j-i));
		strncpy(pathName,cmd+i,(j-i));
		printf("\nThe path name is %s\n",pathName);
		//ghi setenv(varName,pathName,1);	
		return(0);
	}
  	if(strncmp("echo",cmd,4)==0 || strncmp("echo",cmd,4)==0){
		//char after $ == cmd[i]
		j=i;
		while(cmd[j++]!='\0'){}
		//cmd[j-1]==NULL
		char* varName = malloc(sizeof(char)*(j-i));
		strncpy(varName,cmd+i,(j-i));
		printf("echo this:\"%s\"\n\n",varName);
		
	}	
	return(-1);//ghi
}

int main (int argc, char ** argv, char **envp) {
	char path[] = "/bin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games";
	char c[] = "exit";
	char* var = "echo$PATH   dd";
	check4Env(var);
	return 0;
}
