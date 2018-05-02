
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

debug: maind clean

maind: main.od graphe.od inout.od
	gcc -o steiner.exe_debug main.o graphe.o inout.o -g

main.od: main.c
	gcc -o main.o -c main.c -Wall -Wextra -g

graphe.od: graphe.c
	gcc -o graphe.o -c graphe.c -Wall -Wextra -g

inout.od: inout.c
	gcc -o inout.o -c inout.c -Wall -Wextra -g


