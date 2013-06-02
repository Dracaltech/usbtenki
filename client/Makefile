CC=gcc
LD=$(CC)

# Note: The -I/opt/local/include and -L/opt/local/lib are there
# for compiling under MacOS X with macports.
CFLAGS=-Wall -g -O -I../common -I../library -I/opt/local/include
LDFLAGS=-lusb -lm -L/opt/local/lib/ -L/usr/local/opt/libusb-compat/lib

PREFIX=/usr/local

PROGS=usbtenkiget usbtenkisetup

.PHONY : clean install

all: usbtenkiget usbtenkisetup

usbtenkiget: usbtenki.o main.o timestamp.o convertRaw.o
	$(LD) $^ -o $@ $(LDFLAGS)
	
usbtenkisetup: usbtenki.o usbtenkisetup.o convertRaw.o
	$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

convertRaw.o: ../library/convertRaw.c 
	$(CC) $(CFLAGS) -c $< -o convertRaw.o

usbtenki.o: ../library/usbtenki.c 
	$(CC) $(CFLAGS) -c $< -o usbtenki.o

clean:
	rm -f *.o $(PROGS)

install:
	@echo "Install not done yet. Sorry"

