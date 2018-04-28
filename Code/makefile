
all: main clean

main: main.o graphe.o inout.o
	gcc -o steiner.exe main.o graphe.o inout.o

main.o: main.c
	gcc -o main.o -c main.c -Wall -Wextra -O3

graphe.o: graphe.c
	gcc -o graphe.o -c graphe.c -Wall -Wextra -O3

inout.o: inout.c
	gcc -o inout.o -c inout.c -Wall -Wextra -O3

.PHONY: clean

clean:
	rm -rf *.o


