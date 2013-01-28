all: my_getpid fileutil pointers ex1

pointers: pointers.c
	gcc -Wall -Werror -o pointers pointers.c

ex1: ex1.c
	gcc -Wall -Werror -o ex1 ex1.c


my_getpid: my_getpid.c mysyscall.h
	gcc -Wall -Werror -o my_getpid my_getpid.c

fileutil: fileutil.c mysyscall.h
	gcc -m32 -Wall -Werror -nostdlib -static -g -e util_start -o fileutil fileutil.c

handin:	clean
	@if [ `git status --porcelain| wc -l` != 0 ] ; then echo "\n\n\n\n\t\tWARNING: YOU HAVE UNCOMMITTED CHANGES\n\n    Consider committing any pending changes and rerunning make handin.\n\n\n\n"; fi
	@git tag -f -a lab1-handin -m "Lab1 Handin"
	@git push --tags handin

clean:
	rm -f *~ *.o my_getpid fileutil ex1 pointers
