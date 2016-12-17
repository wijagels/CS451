TARGET=mybarrier.o
CFLAGS=-std=gnu11 -g -Wall -Wextra -pedantic -pthread -D PIPE_BUF_MAX=1024
CC=gcc

all: $(TARGET)
	
mybarrier.o: mybarrier.c mybarrier.h
	$(CC) $(CFLAGS) -c $*.c

test.o: test.c mybarrier.h
	$(CC) $(CFLAGS) -c $*.c

test.out: mybarrier.o test.o
	$(CC) -pthread -o test.out test.o mybarrier.o
	

clean:
	rm -rf *.o *.a $(TARGET)

