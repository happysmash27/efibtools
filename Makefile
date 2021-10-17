all: efib

efib: efib.c
	gcc efib.c -o efib

clean:
	rm -f efib
