CC = gcc
FLAGS = -std=c17 -Wall -Wextra
PROGRAM = traceroute

traceroute: main.o
	$(CC) $(FLAGS) -o $(PROGRAM) $^

main.o: main.c

clean:
	rm *.o

distclean:
	rm *.o $(PROGRAM)