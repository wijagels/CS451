CC ?= gcc
CFLAGS ?= -g -std=gnu11 -lrt -Wall -Wextra -Werror -pedantic

all: switch_column dir_stat oz_demo

switch_column: switch_column.c
	$(CC) $(CFLAGS) -o switch_column switch_column.c

dir_stat: dir_stat.c
	$(CC) $(CFLAGS) -o dir_stat dir_stat.c

oz_demo: oz_demo.c
	$(CC) $(CFLAGS) -o oz_demo oz_demo.c
