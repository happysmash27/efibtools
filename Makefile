all: efib

efib: efib.c
	gcc efib.c -Wall -g -o efib

clean:
	rm -f efib
