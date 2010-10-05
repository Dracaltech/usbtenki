CC=gcc
LD=$(CC)

CFLAGS=-Wall -g -O  -I../common -DVERSION=\"1.8a\"
LDFLAGS=-lusb -lm

PREFIX=/usr/local

PROGS=usbtenkiget usbtenkisetup

.PHONY : clean install

all: usbtenkiget usbtenkisetup

usbtenkiget: usbtenki.o main.o timestamp.o
	$(LD) $(LDFLAGS) $^ -o $@
	
usbtenkisetup: usbtenki.o usbtenkisetup.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROGS)

install:
	@echo "Install not done yet. Sorry"

