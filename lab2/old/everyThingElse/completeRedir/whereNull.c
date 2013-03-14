#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
	char* mons="ghi";
	size_t i=strlen(mons)-1;
	
	printf("mons[length(mons)-1] = %x",mons[i]);
	printf("mons[length(mons)] = %x",mons[strlen(mons)]);
	return(0);
}
