#include "mysyscall.h"

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

int
main(int argc, char **argv) {
  int return_code = argc;

  // Exercise 6: Your code here.

  // Exit.  Until this is implemented,
  // your program will hang or give a segmentation fault.
  MY_SYSCALL1(0, return_code);

  return return_code;
}
