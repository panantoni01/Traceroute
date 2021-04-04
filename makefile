CC = gcc
FLAGS = -std=c17 -Wall -Wextra
PROGRAM = traceroute

traceroute: main.o icmp.o wrappers.o
	$(CC) $(FLAGS) -o $(PROGRAM) $^

main.o: main.c

icmp.o: icmp.c icmp.h

wrappers.o: wrappers.c wrappers.h

clean:
	rm -f *.o

distclean:
	rm -f *.o $(PROGRAM)