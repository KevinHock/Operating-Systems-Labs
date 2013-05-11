#ifndef JIFFIEEZ_H
#define JIFFIEEZ_H
#include <linux/jiffies.h>

void function_prologue(void);
void function_epilogue(void);

//#define ROOTKIT_DEBUG	1
#if defined(ROOTKIT_DEBUG) && ROOTKIT_DEBUG == 1
# define DEBUG(...)		printk("%d",__LINE__); printk(KERN_INFO __VA_ARGS__)
#else
# define DEBUG(...)
#endif

#endif
