CFLAGS=-g -static -Wall -Wextra -pedantic -std=c11
ARFLAGS=rcs
SRCEXT=c
SOURCES=$(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS=$(patsubst %.$(SRCEXT),%.o,$(SOURCES))
STATICLIB=hw1.a


all: $(STATICLIB)

$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o:%.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm $(STATICLIB) *.o

.PHONY: clean
