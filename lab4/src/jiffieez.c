#include "jiffieez.h"

unsigned long long prologueTickCount, epilogueTickCount, tickDiff;

void function_prologue(void){
    prologueTickCount = (unsigned long long) get_jiffies_64();
}

void function_epilogue(void){
    int i,x=0;
    for(i=0;i<40000;i++){//20000 is 1 second
        x++;
    }

    epilogueTickCount = (unsigned long long) get_jiffies_64();
    tickDiff = epilogueTickCount - prologueTickCount;
    if(tickDiff<35){
    }else{
	//You are debugging
	//while(1){}
    }
}
