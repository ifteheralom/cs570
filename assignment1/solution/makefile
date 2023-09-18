

all: save load print query create change

save: save.c header.h
	gcc -Wall -o save save.c header.c
load: load.c header.h
	gcc -Wall -o load load.c header.c
print: print.c header.h
	gcc -Wall -o print print.c header.c
query: query.c header.h
	gcc -Wall -o query query.c header.c
create: create.c header.h
	gcc -Wall -o create create.c header.c
change: change.c header.h
	gcc -Wall -o change change.c header.c
clean:
	rm -f save load print query create  change  *~ core
