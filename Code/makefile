
all: main clean

main: main.o graphe.o inout.o utils.o
	gcc -o steiner.exe main.o graphe.o inout.o utils.o -std=c99 -Wno-unused-variable -Wno-unused-parameter

main.o: main.c
	gcc -o main.o -c main.c -Wall -Wextra -O3 -std=c99 -Wno-unused-variable -Wno-unused-parameter

graphe.o: graphe.c
	gcc -o graphe.o -c graphe.c -Wall -Wextra -O3 -std=c99 -Wno-unused-variable -Wno-unused-parameter

inout.o: inout.c
	gcc -o inout.o -c inout.c -Wall -Wextra -O3 -std=c99 -Wno-unused-variable -Wno-unused-parameter
	
utils.o: utils.c
	gcc -o utils.o -c utils.c -Wall -Wextra -O3 -std=c99 -Wno-unused-variable -Wno-unused-parameter

.PHONY: clean

clean:
	rm -rf *.o

debug: maind clean

maind: main.od graphe.od inout.od utils.od
	gcc -o steiner.exe_debug main.o graphe.o inout.o utils.o -g -std=c99

main.od: main.c
	gcc -o main.o -c main.c -Wall -Wextra -g -std=c99

graphe.od: graphe.c
	gcc -o graphe.o -c graphe.c -Wall -Wextra -g -std=c99

inout.od: inout.c
	gcc -o inout.o -c inout.c -Wall -Wextra -g -std=c99

utils.od: utils.c
	gcc -o utils.o -c utils.c -Wall -Wextra -g -std=c99

script: script.o
	gcc -o script.exe script.o -std=c99 -Wno-unused-variable -Wno-unused-parameter


