CC=gcc
LD=$(CC)

CFLAGS=-Wall -g -O  -I../common -DVERSION=\"1.1\"
LDFLAGS=-lusb -lm

PREFIX=/usr/local

PROG=usbtenkiget

.PHONY : clean install

all: usbtenkiget usbtenkisetup

usbtenkiget: usbtenki.o main.o
	$(LD) $(LDFLAGS) $^ -o $@
	
usbtenkisetup: usbtenki.o usbtenkisetup.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROG)

install:
	@echo "Install not done yet. Sorry"

