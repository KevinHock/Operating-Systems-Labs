#ifndef __MYSYSCALL_H__
#define __MYSYSCALL_H__
#include <asm/unistd.h>
#include <errno.h>

// Exercise 5: Your code here
// Populate each of these functions with appropriate
// assembly code for each number of system call arguments.
//
// Go ahead and fill in all 7 variants, as you will eventually
// need them.
//
// Friendly advice: as you figure out the signature of a system
// call, you might consider writing a macro for it for future reference, 
// like:
//
// #define MY_GETPID(...) MY_SYSCALL...(...)

//EAX = 1 for exit
//EBX = NUM for exit code (probably 0)

//push eax
//mov eax, 1
//mov ebx, num
//int x80
//pop eax

#define sys_exit(arg1) asm("pushl %%eax\n\t" "pushl %%ebx\n\t" "movl $1, %%eax\n\t" "movl %0, %%ebx\n\t" "int $0x80\n\t" "popl %%ebx\n\t" "popl %%eax\n\t":"=r" (arg1):"0" (arg1));

#define sys_getpid(arg1) asm("pushl %%eax\n\t" "movl $20, %%eax\n\t" "int $0x80\n\t" "movl %%eax, %0\n\t" "popl %%eax\n\t":"=r" (arg1):"0" (arg1));
/*
#define sys_exit(status) asm("pushl %%eax\n\t" "pushl %%ebx\n\t" "movl $1, %%eax\n\t" "movl %0, %%ebx\n\t" "int $0x80\n\t" "popl %%ebx\n\t" "popl %%eax\n\t"::"0" (status):);
*/




//eax 4 write
//ebx 1 for STDOUT fd
//ecx is string
//edx for size of string
//
/*
#define sys_write(arg1)	({							\
	int rv = -ENOSYS; 				\
	asm volatile("movl $4, eax\n\t":::); 		\
	asm volatile("movl $1, ebx\n\t":::);		\
	asm volatile("movl %0, ecx\n\t"::"0" (arg1):); \
	asm volatile("movl $4, edx\n\t":::);		\
	asm volatile("int x80\n\t");			\
	rv;						\ 
  })


#define MY_SYSCALL1(NUM, ARG1)				\
  ({							\
    int rv = -ENOSYS;					\
    asm volatile ("nop" : : :);				\
    rv;							\
  })


#define MY_SYSCALL2(NUM, ARG1, ARG2)			\
   ({							\
     int rv = -ENOSYS;					\
     asm volatile ("nop" : : :);			\
     rv;						\
   })

   
#define MY_SYSCALL3(NUM, ARG1, ARG2, ARG3)		\
   ({							\
     int rv = -ENOSYS;					\
     asm volatile ("nop" : : :);			\
     rv;						\
   })
   
#define MY_SYSCALL4(NUM, ARG1, ARG2, ARG3, ARG4)	\
   ({							\
     int rv = -ENOSYS;					\
     asm volatile ("nop" : : :);			\
     rv;						\
   })

#define MY_SYSCALL5(NUM, ARG1, ARG2, ARG3, ARG4, ARG5)	\
   ({							\
     int rv = -ENOSYS;					\
    asm volatile ("nop" : : :);				\
    rv;							\
   })

#define MY_SYSCALL6(NUM, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) \
   ({							     \
     int rv = -ENOSYS;					     \
     asm volatile ("nop" : : :);			     \
     rv;						     \
   })
*/
#endif // __MYSYSCALL_H__
