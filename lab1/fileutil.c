#include "mysyscall.h"
#include <sys/stat.h>
#include <sys/mman.h>

void util_start(void);
int length(char* string);
void stdinFlagSetter(char* string);
void stdoutFlagSetter(char* string);
void letterFlagSetter(char* string);
char* turnToChar(int number);

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
//
char crlfArr[]="\r\n";//Stupid -Werror bs
char* crlf = (char *)&crlfArr;
char crChar = '\r';
char* cr = &crChar;
char lfChar = '\n';
char* lf = &lfChar;
char* null = 0;
int dFlag=0,cFlag=0,hFlag=0,rFlag=0,uFlag=0,stdoutFlag=0,stdinFlag=0,uselessPara=0,i=0,status=0;

//Global for dangling pointer
char ttcArr[20];
char* ttcRet;

int main(int argc, char **argv){
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
	sys_write(4,1,errMsg,length(errMsg),uselessPara);
	//printf("\nResults of Write=%d\n",resultsOfWrite);
	dFlag=uFlag=rFlag=cFlag=0;
        hFlag=1;
  }
  //Help flag
  if(hFlag==1){
	char* helpMsg = "Proper use is fileutil [-cdhru] infile|- outfile|-\n-c: Counts the newlines in the file and outputs to stderr.\n-d: Converts the output to use DOS-style newlines.\n-h: Prints a help message.\n-r: Reverses the contents of the file, on a line-by-line basis.\n-u: Converts the output to use Unix-style newlines.\n";
	sys_write(4,1,helpMsg,length(helpMsg),uselessPara);
	dFlag=uFlag=rFlag=cFlag=0;
	status=0;
  	sys_exit(status);
  }
  
  struct stat stbuf;
  struct stat *pointy=&stbuf;
  short ifileSize=0;
//If there is an input file to get the size of
  if(stdinFlag==0){
	//get the size of it.
  	statistics(195,*(argv+argc-2), pointy);
  	ifileSize = stbuf.st_size;
  }
  char ibufferArr[10000];
  char* ibuffer = (char *)&ibufferArr;
  int fdin=20;
  int* f=&fdin;
//Open input file
  if(stdinFlag==0){
	if(f==0){/*f helps fdin not change, who knows why*/}//Stupid -Werror bs
	fdin=-50;
	//Open, char * filename, flags, mode, file descriptor
  	sys_write(5,*(argv+argc-2),0,0,fdin);
	//If input file doesn't exist
	if(fdin<0){
	     char* helpMsgOpenF = "Unable to open input file.\n";
	     sys_write(4,1,helpMsgOpenF,length(helpMsgOpenF),uselessPara);
	     status=1;
  	     sys_exit(status);
	}
	//File into buffer based on st_size
	sys_read(3,fdin, ibuffer,ifileSize);//5000
	//Close input file
	twoArg(6,fdin);
  }else{//Read from stdin
	sys_read(3,0, ibuffer,3);
	sys_write(4,1,ibuffer,3,uselessPara);
	status=1;
  	sys_exit(status);
	//ghi sys_read(3,0, ibuffer,9998);//Maybe read one char at a time? ghi
  }
  

  char currChar=*(ibuffer); 
  int newLineCount=0,offset=0;
  //Need number of newlines for -c and -r
  if(cFlag==1 || rFlag==1)
	for(;;){
            currChar=*(ibuffer+offset++);
            if(10==(int)currChar)
                newLineCount++;
            if(currChar==0 || currChar<0)
                break;
        }
  //c: Count the newlines in the file and output this on stderr (file handle 2)
  if(cFlag==1){
	//Print "The number of newlines ="
	char* nlMsg = "The number of newlines =";
	sys_write(4,2,nlMsg,length(nlMsg),uselessPara);
	//Print number of newlines
	char* numberToPrint = turnToChar(newLineCount);
	sys_write(4,2,numberToPrint,length(numberToPrint),uselessPara);
	//Newline
	sys_write(4,2,lf,1,uselessPara);
  }
	//ghi
	char wtf[4] = "ABCD";//WHAT THE FUCK
	wtf[0] = (char)*ibuffer;
	wtf[1] = *(ibuffer+1);
	wtf[2] = *(ibuffer+2);
	wtf[3] = *(ibuffer+3);


/*
int* f=&fdin;
//Open input file
  if(stdinFlag==0){
	
*/
  //Open output file
  struct stat ostbuf;
  struct stat *opointy=&ostbuf;
  short ofileSize=0;
  int holder=2;
  int fdout=-6;
  int* idk=&fdout;
  if(stdoutFlag==0){
	if(idk==0){/*helps fdout not change*/}
	sys_write(5,*(argv+argc-1),100|1,511,fdout);
    	//Call to stat changes value of fdout because fdout is just *(argv+arg-1)
	holder=fdout;
	statistics(195,*(argv+argc-1), opointy);
	ofileSize = ostbuf.st_size;//HOW DOES THIS MAKE ibuffer NULL IN IT'S FIRST 4 BYTES?
	*(ibuffer)=wtf[0];//ghi
	*(ibuffer+1)=wtf[1];
 	*(ibuffer+2)=wtf[2];
	*(ibuffer+3)=wtf[3];
	fdout=holder;
  }else
   	fdout=1;
  

  offset=0;
  int skip=0,leftOver=0;
  

  if(dFlag && !rFlag)
  	while(1){
            currChar=*(ibuffer+offset);
            if((currChar == 0 || currChar<0)){
		if(stdoutFlag==0){
			//Difference = ofileSize-offset
			leftOver=ofileSize-offset;
			//get size of output file
 			for(i=1;i<leftOver;i++){
				sys_write(4,fdout,null,1,uselessPara);
			}
		}		
		break;
	    }
	    if(13==(int)currChar)
		skip=1;
	    if((10==(int)currChar) && skip==0)
		sys_write(4,fdout,cr,1,uselessPara);
	    sys_write(4,fdout,(ibuffer+offset),1,uselessPara);
	    offset++;
        }
//We already checked that d and u aren't both set so no offset=0;
  if(uFlag && !rFlag)
        while(1){
            currChar=*(ibuffer+offset);
            if((currChar == 0 || currChar ==-1)){
		if(stdoutFlag==0){
			//Difference = ofileSize-offset
			leftOver=ofileSize-offset;
 			for(i=1;i<leftOver;i++)
				sys_write(4,fdout,null,1,uselessPara);
		}
                break;
	    }
            if(13!=(int)currChar)
                sys_write(4,fdout,(ibuffer+offset),1,uselessPara);
	    offset++;
        }
  if(rFlag){
        //Get the offset of each newline
        offset=0;
        int nlaIndex=0;
	int* array[10000];
        int** newLineArray = (int **)&array;
        *(newLineArray+nlaIndex)=0;
        int ttt=1;
        //Get offsets
        for(;;){
            currChar=*(ibuffer+offset++);
            if(currChar == 0 || currChar<0)
                break;
            if(10==(int)currChar)
                nlaIndex++;
            else
                *(newLineArray+nlaIndex)=(int *)ttt;
            ttt++;
            //Count = *(newLineArray+nlaIndex)
        }
        /*
        for(p=newLineCount-1;p>=0;p--){
            printf("The number to add to buffer is %d \n",(int)(*(newLineArray+p)));
            printf("Offset %d should be %x\n",p,*(ibuffer+(int)(*(newLineArray+p))));
        }
	*/
        //We have the offsets
        int dosFlag=0;
	offset=0;
        //Assuming CR won't be in a UNIX text file.
        if(!uFlag && !dFlag)
            for(;;){
                currChar=*(ibuffer+offset++);
                if(currChar==13)
                    dosFlag=1;
                if(currChar == 0 || currChar<0 || currChar == 10)
                    break;
            }
        else
            dosFlag=dFlag;
	//If we just created the file don't put \r's in
	if(ofileSize==0)
		dosFlag=0;
	offset=0;
        int cobain=0;
        for(i=newLineCount-1;i>=-1;i--){
            if(i!=-1)
                cobain=(int)(*(newLineArray+i))+1;
            else
                cobain=0;
            //
            offset=0;
            //
            for(;;){
            	currChar=*(ibuffer+cobain+offset);
            	if( currChar == 10 || currChar == 0 || currChar<0  ){
                	if( (dosFlag && i)  != (newLineCount-1) )
                    		sys_write(4,fdout,crlf,2,uselessPara);
                	if( (!dosFlag && i) != (newLineCount-1) )
                    		sys_write(4,fdout,lf,1,uselessPara);
                	break;
                }
            	sys_write(4,fdout,(ibuffer+cobain+offset++),1,uselessPara);
            }
        }
  }    
  if(stdoutFlag==0)
  	twoArg(6,fdout); 
//Exit
  status=0;
  sys_exit(status);
  return(0);
}





//OTHER FUNCTIONS//
//OTHER FUNCTIONS//
//OTHER FUNCTIONS//
//OTHER FUNCTIONS//
//OTHER FUNCTIONS//
//OTHER FUNCTIONS//
//OTHER FUNCTIONS//

char* turnToChar(int number){
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
