#include "mysyscall.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
  pid_t pid1, pid2;
  pid1 = pid2 = 0;

  pid1 = getpid();
  
  // Exercise 5: Your code here.
  // Use the appropriate MY_SYSCALL#() macro.
  //
  // Hint: Read the Linux source code to figure out
  // which system call number selects getpid, and how many 
  // arguments the system call takes.
  //
  // Extra hint: Not all system calls have the same interface
  // as the similar libc function (although many do).  So best to 
  // double check this.
  // 
  //  E.g.: 
  // pid2 = MY_SYSCALL...(NUM);

  if (pid1 == pid2 && pid2 > 0)
      printf("Congratulations!  You have correctly gotten your pid with inline assembly, which is is %d\n", pid1);
  else 
    printf("Oops: Pid 1 is %d, pid 2 is %d\n", pid1, pid2);

  return 0;
}
