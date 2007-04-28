CC=gcc
LD=$(CC)

CFLAGS=-Wall -g -O  -I../common -DVERSION=\"1.0\"
LDFLAGS=-lusb -lm

PREFIX=/usr/local

PROG=usbtempctl

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

.PHONY : clean install

all: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROG)

install:
	@echo "Install not done yet. Sorry"

