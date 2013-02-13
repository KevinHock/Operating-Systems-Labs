#include "mysyscall.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

void util_start(void);

asm (".global util_start\r\n"
     "  .type util_start,@function\r\n"
     ".global main\r\n"
     "  .type main,@function\r\n");

/* Get the argc and argv values in the right registers */
asm ("util_start:\r\n"
     "movl %esp, %eax\r\n"
     "addl $4, %eax\r\n"
     "pushl %eax\r\n"
     "subl $4, %eax\r\n"
     "pushl (%eax)\r\n"
     "  call main\r\n");

int dFlag=0,cFlag=0,hFlag=0,rFlag=0,uFlag=0,stdoutFlag=0,stdinFlag=0;

int length(char* string);
void stdinFlagSetter(char* string);
void stdoutFlagSetter(char* string);
void letterFlagSetter(char* string);


int main(int argc, char **argv) {
  int flagLoop;
  //Set all letter flags
  if(argc>3)
  	for(flagLoop=1;flagLoop<argc-2;flagLoop++)
        	letterFlagSetter(*(argv+flagLoop));
  //Set stdin and stdout flags
  if(argc>2){
  	stdinFlagSetter( *(argv+argc-2));
        stdoutFlagSetter(*(argv+argc-1));
  }
  //Can't have DOS and Unix style newlines
  if(dFlag==1 && uFlag==1){
	char* errMsg = "ERROR: Cannot have DOS Style newlines and Unix Style newlines.\n";
	int resultsOfWrite;
	sys_write(4,1,errMsg,length(errMsg),resultsOfWrite);
	dFlag=uFlag=rFlag=cFlag=0;
        hFlag=1;
  }
  //Help flag
  if(hFlag==1){
	char* helpMsg = "Proper use is fileutil [-cdhru] infile|- outfile|-\n-c: Counts the newlines in the file and outputs to stderr.\n-d: Converts the output to use DOS-style newlines.\n-h: Prints a help message.\n-r: Reverses the contents of the file, on a line-by-line basis.\n-u: Converts the output to use Unix-style newlines.\n";
        int la;
	sys_write(4,1,helpMsg,length(helpMsg),la);
  }
  
  
//
  char *name = "abc.txt";
  struct stat stbuf;
  struct stat *pointy=&stbuf;
//stat(name, &stbuf);
  statistics(195,name, pointy);
  short fileSize = stbuf.st_size;
  printf("\nThe size is %d\n",fileSize);
 
  

  int g=20;
  int* f=&g;
//Open File
  sys_write(5,"abc.txt",0,0,g);
  

  printf("fd is %d\n",g);

//malloc                15 is preset
  char* buffer = mmap(0, fileSize, PROT_READ, MAP_SHARED, g, 0); 
  //if(buffer == MAP_FAILED)
  	//print bad map

  char currChar=*(buffer); 
  printf("\nChar = %c\n",currChar);

//-c flag   
  int newLineCount=0,offset=0;
  if(cFlag==1 || rFlag==1)
	for(;;){
            currChar=*(buffer+offset++);
            if(10==(int)currChar)
                newLineCount++;
            if(currChar==EOF)
                break;
        }
  if(cFlag==1)printf("The number of newlines = %d\n",newLineCount);
  printf("\ncFlag=%d\n",cFlag);
  
  
  //munmap(&buffer, fileSize);
//Close File
  twoArg(6,g);
   
//Exit
  int status=0;
  sys_exit(status);
  return(0);
}

void letterFlagSetter(char* string){
  int flagLoop=1;
  for(;flagLoop<length(string);flagLoop++)
    switch(*(string+flagLoop)){
        case 'd':
            dFlag=1;
            break;
        case 'c':
            cFlag=1;
            break;
        case 'h':
            hFlag=1;
            break;
        case 'r':
            rFlag=1;
            break;
        case 'u':
            uFlag=1;
        default:
            break;
    }
}

void stdinFlagSetter(char* string){
    if(length(string)==1 && *string==45)
        stdinFlag=1;
}

void stdoutFlagSetter(char* string){
    if(length(string)==1 && *string==45)
        stdoutFlag=1;
}

int length(char* string){
    int length=1;
    for(;;){
        if(*(string+length)==0)break;
        length++;
    }
    return length;
}
