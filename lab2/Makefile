all: swish

swish: swish.c
	gcc -Wall -Werror -o swish swish.c

handin:	clean
	@if [ `git status --porcelain| wc -l` != 0 ] ; then echo "\n\n\n\n\t\tWARNING: YOU HAVE UNCOMMITTED CHANGES\n\n    Consider committing any pending changes and rerunning make handin.\n\n\n\n"; fi
	@git tag -f -a lab2-handin -m "Lab2 Handin"
	@git push --tags handin

clean:
	rm -f *~ *.o swish
