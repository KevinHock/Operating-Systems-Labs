all: dns-sequential dns-mutex dns-rw dns-fine

CFLAGS = -g -Wall -Werror -pthread

%.o: %.c *.h
	gcc $(CFLAGS) -c -o $@ $<

dns-sequential: main.c sequential-trie.o
	gcc $(CFLAGS) -o dns-sequential sequential-trie.o main.c

dns-mutex: main.c mutex-trie.o
	gcc $(CFLAGS) -o dns-mutex mutex-trie.o main.c

dns-rw: main.c rw-trie.o
	gcc $(CFLAGS) -o dns-rw rw-trie.o main.c

dns-fine: main.c fine-trie.o
	gcc $(CFLAGS) -o dns-fine fine-trie.o main.c

handin:	clean
	@if [ `git status --porcelain| wc -l` != 0 ] ; then echo "\n\n\n\n\t\tWARNING: YOU HAVE UNCOMMITTED CHANGES\n\n    Consider committing any pending changes and rerunning make handin.\n\n\n\n"; fi
	@git tag -f -a lab3-handin -m "Lab3 Handin"
	@git push --tags handin

clean:
	rm -f *~ *.o dns-sequential dns-mutex dns-rw dns-fine
