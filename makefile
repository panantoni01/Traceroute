CC = gcc
FLAGS = -std=c17 -Wall -Wextra
PROGRAM = traceroute

traceroute: main.o icmp.o
	$(CC) $(FLAGS) -o $(PROGRAM) $^

main.o: main.c

icmp.o: icmp.c icmp.h

clean:
	rm *.o

distclean:
	rm *.o $(PROGRAM)