CFLAGS=-g -static -Wall -Wextra -std=c11
ARFLAGS=rcs
SRCEXT=c
SOURCES=bitmap.c memory_manager.c
OBJECTS=$(patsubst %.$(SRCEXT),%.o,$(SOURCES))
STATICLIB=memory_manager.a


all: $(STATICLIB)

$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o:%.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm $(STATICLIB) *.o

.PHONY: clean
