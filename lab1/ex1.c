//
// Lab 1 
// Exercise 1
//
// Author: Kevin Hock

#include <stdio.h>
#include <stdlib.h>

#define inc(arg1) asm("inc %0": "=r" (arg1) : "0" (arg1));

int main(int argc, char **argv)
{
  int x = 1;
  
  printf("Before x = %d.\n", x);
  inc(x);
  printf("After x = %d.\n", x);

  if(x == 2)
    printf("OK\n");
  else
    printf("ERROR\n");
  
  return 0;
}
