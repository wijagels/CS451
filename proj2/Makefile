TARGET := run-mapreduce
CFLAGS := -std=gnu11 -g -O3 -lpthread -Wall -Wextra -Wno-variadic-macros -pedantic-errors
CC := gcc
WORKERS := 4

all: $(TARGET)

$(TARGET): main.o mapreduce.o usr_functions.o
	$(CC) $(CFLAGS) -o $@ main.o mapreduce.o usr_functions.o

main.o: main.c mapreduce.h usr_functions.h
	$(CC) $(CFLAGS) -c main.c

mapreduce.o: mapreduce.c mapreduce.h common.h
	$(CC) $(CFLAGS) -c $*.c

usr_functions.o: usr_functions.c usr_functions.h common.h
	$(CC) $(CFLAGS) -c $*.c

test: test-count test-find

test-count: test-count-sm test-count-md test-count-lg

test-find: test-find-sm test-find-md test-find-lg

test-count-sm: $(TARGET)
	./$(TARGET) counter input-alice30.txt $(WORKERS)
	./charcount.py input-alice30.txt
	diff charcount-out.rst mr.rst

test-count-md: $(TARGET)
	./$(TARGET) counter input-moon10.txt $(WORKERS)
	./charcount.py input-moon10.txt
	diff charcount-out.rst mr.rst

test-count-lg: $(TARGET)
	./$(TARGET) counter input-warpeace.txt $(WORKERS)
	./charcount.py input-warpeace.txt
	diff charcount-out.rst mr.rst


test-find-sm: $(TARGET)
	./$(TARGET) finder input-alice30.txt $(WORKERS) the
	cat input-alice30.txt | grep the > find.rst
	diff find.rst mr.rst


test-find-md: $(TARGET)
	./$(TARGET) finder input-moon10.txt $(WORKERS) the
	cat input-moon10.txt | grep the > find.rst
	diff find.rst mr.rst

test-find-lg: $(TARGET)
	./$(TARGET) finder input-warpeace.txt $(WORKERS) the
	cat input-warpeace.txt | grep the > find.rst
	diff find.rst mr.rst

clean:
	rm -rf *.o *.a $(TARGET) *.itm *.rst
