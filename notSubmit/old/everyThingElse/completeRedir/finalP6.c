//Write run()
//	
//	
//Write cbi() with job control
//Finish Logic

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int i=-1,fd=-1,debug=1;

int meta(char ch);

/*
Sample Walk through

1         2           3       4
ls -a 2 | grep .txt | wc -l > q.txt

1.
before running make input file 
make input file stdout
call run which will make all stderr's go to stdout (the input file)
2.
grep reads the output file of ls
make another input file stdout
call run to make all output go to that file
3.
wc ""
""
""
4.
we make output of wc go into q.txt

 * Must take note of the metacharacter before (and obviously after) the command so it can read input from it's previous command.
 * Maybe make a ?custom name? MAYBE ONLY NEED ONE FILE for each custom file by strcatting an integer. like 1.If before char == |   2.Read lastFile 
 * Must make run() function to
 * 		Run each individual command (this includes making custom handler go to 1 (a file) if next mc isn't '<' or '\0'
 * Must make seperate command to check built in commands ?HOW SEPARATE SHOULD CBI AND RUN BE?
 */
int main(int argc, char ** argv, char **envp){
	char cmd[] = "ls | grep '.txt' | wc -l";
	char ch;
	int last;
	char* currCmd;
	i=0;
	while(1){
		ch=cmd[i];
		if(meta(ch)){
			if(ch=='<'){/*open file and input=file	cat<file*/}
			
			if(ch=='>'){/*open file and file=output	ls > newfile*/}
			
			if(ch=='|'){//input=output;
				
				/*
				 PART 
				 ONE
				 */
				
  				//Create temporary file to pipe output into and then use that file as input to other next function
  				fd = open("outfilePipes", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  				//CHECK FOR FAILURE
  				if (fd < 0) {
					perror("Creating temporary file in '|' failed.\n");
					exit(-1);
				}
  				
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
					//Malloc what the function is and pass it to run.
					//Get currCmd
					currCmd = malloc(sizeof(char)*((i-last)+1));//Unsure if +1 for NULL byte is needed
					strncpy(currCmd,cmd[i],(i-last));//ghi DOES THAT WORK
					
					if(debug ==1){
						fprintf(stderr,"The child is running the command %s\n",currCmd);
						fflush(NULL);
					}
					//Run command
					run(currCmd);
					
					free(currCmd);
					close(fd);
				}else{
					wait(NULL);
				}
				//Close temporary file
				close(fd);
				
				//Restore stdout
				dup2(oldout, 1);
            }
			if(ch=='\0'){/*just run it*/
				
			}
			last=i;//offset of after last meta char?
		}
		i++;
	}
	return(0);
}

/*
				 PART 
				 TWO
				 
				 WILL USE FOR GETTING INPUT FROM THE LEFT 
				 
				//Read the output
				fd=open("outfilePipes", O_RDONLY, S_IRWXU);
				//CHECK FOR FAILURE
  				if (fd < 0) {
					perror("Reading temporary file in '|' failed.\n");
					exit(-1);
				}
				
				dup2(fd,0);//All input goes to fd
				
				pid_t pid2 = fork();
				//CHECK FOR FAILURE
				if (pid < 0) {
					perror("Second fork in '|' failed.\n");
					exit(-1);
				}
				
				if (pid2 == 0) {
					execl("/bin/grep","","qq",NULL);
					close(fd);
				}
				
				else {
					wait(NULL);
					
				}
				close(fd);
*/



//Run Prototype:	
int run(char* cmd, int numOArgs){	
	int numOArgs = getNumOArgs(cmd);		
	char** argList = malloc(sizeof(int)*numOArgs);	
	fillArgs(argList,cmd);	
	if(!dirFlag){
			//Loop through PATH? Probably not.
			execvp(arg0,argList);//man says "const char *file, char *const argv[]"
	}	
	else{
		execv(dir token, argList);
	}
	free(argList);
}

//Fills argList with each command by splitting by ' '
void fillArgs(char** argList, char* cmd, int numArgs){
	int i=0;//Global i might be effected
	//WATCH FOR DANGLING POINTERS
	
	char* currArg = &argList;
	
	for(strtok(cmd,' ')){
		if(numArgs==i){
			//if(currArg is a number)
			//****dup entire function to the number**** somecommand 2>err.log
		}
		if contains '/'{
			dirFlag=1;
			dir = malloc(sizeof(char)*length(tok)+1);
			strncpy(dir,tok,sizeof(char)*length(tok));
		}else{
			//Malloc length and put it in argList
			currArg = malloc(sizeof(char)*length(tok)+1);
			strncpy(currArg,tok,sizeof(char)*length(currArg));
		}
		currArg++;
		i++;
	}
}	


//Turns number into char* to be strcat'd to file name, might not be needed.
/*
char* turnToChar(int number){
	//DANGLING DANGERS SO TTCRET GLOBAL
  long int i;
  int length=1;
  for(i=1;i<1000000;i*=10){
	if( (number-(i*10))<0 ){
		break;
	}
	length++;
  }
  ttcArr[length]=0;
  //for(;Length)
  //if length is 3
  //0 1 2
  //965
  //mod 10 -> 5 ---> Length-1
  //number=number/10;
  //...tccLength-2
  int ttcindexfl=1;
  int ttcleftover=-1;
  while(length-ttcindexfl>=0){
	ttcleftover=number%10;
	ttcleftover+=48;
	ttcArr[length-ttcindexfl]=(char)ttcleftover;
	number/=10;
        ttcindexfl++;
  }
  ttcRet = (char *)&ttcArr;
  return ttcRet;
}
*/





int meta(char ch){
	if(ch=='<'||ch=='>'||ch=='|'||ch=='\0')
		return(1);
	return(0);
}


//ALL BUILT IN CRAP BELOW

/*
//Checks if built in
int cbi(char* cmd,char **envp){
     //CHECK RETURN VALUES
	int popEipVal=-1;
	popEipVal = qCd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal = qPwd(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal = qPrintenv(cmd,envp);
	if(popEipVal>=0)
		return(0);
	popEipVal=check4Env(cmd);
	if(popEipVal>=0)
		return(0);
	return(-1);
}
void printEnvVars(char* cmd ,char** envp){
	if(strcmp("printenv",cmd)==0)
		for(; *envp != 0; envp++)
			printf("%s\n",envp[0]);
}
int qPwd(char* cmd ,char **envp){
	if(strcmp("pwd",cmd)==0){
		//get curr working dir
		findEnvVar("PWD",envp);
		printf("\n\n%s\n\n",foundEnvVar);
		return 0;
	}
	return -1;
}
int qCd(char* cmd,char **envp){
   	char cd[]="cd";
	if(strncmp("cd",cmd,2)==0){
		if(strcmp("cd",cmd)==0){
			findEnvVar("HOME",envp);//Home dir
		 	return(chdir(foundEnvVar));
		}
		if(strcmp("cd -",cmd)==0){
			findEnvVar("OLDPWD",envp);//Last dir
			int result = chdir(foundEnvVar);
			if(debug){
				fprintf(stderr, "result of changing dir is %d.\n",result);
				fflush(NULL);
			}
			return(result);
		}
		if(strncmp("cd ",cmd,3)==0){
			printf("Break here line 64  ghi\n");
			printf("Dir were changing to is %s.\n",cmd+3);
			fflush(NULL);
			return(chdir( cmd+3 ));
		}
		return -1;
	}
}



 // Checks for SET and ECHO of variables
 // Sameple input from comments "set $PATH = /bin:/usr/bin:."
 
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
	printf("this far. line 311\n");//ghi
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
		if(debug)
			fprintf(stderr,"\nThe var name is = %s\n",varName);
		
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
		if(debug)
			fprintf(stderr,"\nThe path name is %s\n",pathName);
		if(debug)
			
		setenv(varName,pathName,1);
		return(0);
	}
  	if(strncmp("echo",cmd,4)==0 || strncmp("echo",cmd,4)==0){
		//char after $ == cmd[i]
		j=i;
		while(cmd[j++]!='\0'){}
		//cmd[j-1]==NULL
		char* varName = malloc(sizeof(char)*(j-i));
		strncpy(varName,cmd+i,(j-i));
		if(debug)
			fprintf(stderr,"\n\necho'ing this:\"%s\"\n\n",varName);
		char *e = getenv(varName);
		printf("%s\n",e);
		return(0);
	}	
	return(-1);//ghi
}
 */
/*
if(*args == '$' && *(args + 1) != '\0')
	  args = resolveVar(args + 1);
if(*args == '~')
	  if(*(args+1) == '\0' || *(args+1) == ' ')
	    args = resolveVar(args);
	    
//Return the contents of an environmental variable:
char *resolveVar(char *variable){
  char *result;
  
  if(debug){
    printf("DEBUG: RESOLVING VARIABLE [%s], RESULT: ", variable);
  } fflush(stdout);

  if((*variable == '~') && *(variable+1) == '\0'){
	result = getenv("HOME");
  } else if ((*variable == '?') && *(variable+1) == '\0') {
	  result = malloc((sizeof(char) * 4));
	  itoa(last_exit_code, result, 10);
  } else {
    result =  getenv(variable);
  }
  
  if(debug){
    printf("%s\n", result); 
  } fflush(stdout);
  return result;
}
*/
