#include <stdio.h>
#include <stdlib.h>

void printEnvVars(char** envp);

void printEnvVars(char** envp){
	
	for(; *envp != 0; envp++)
		printf("%s\n",envp[0]);
}

int main (int argc, char ** argv, char **envp) {
	//printEnvVars(envp);
	char *e = getenv("HOME");
	printf("\n%s\n",e);
	setenv("HOME","/home/cse306/lab",1);
	e = getenv("HOME");
	printf("\n%s\n",e);
	//printEnvVars(envp);
	return(0);
}
