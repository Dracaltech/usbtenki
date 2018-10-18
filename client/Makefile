CC=gcc
LD=$(CC)

#LIBUSB_CFLAGS=-DUSE_OLD_LIBUSB
#LIBUSB_LDFLAGS=-lusb -L/usr/local/opt/libusb-compat/lib

LIBUSB_CFLAGS=`pkg-config libusb-1.0 --cflags`
LIBUSB_LDFLAGS=`pkg-config libusb-1.0 --libs`

CFLAGS=-Wall -g -O -I../common -I../library $(LIBUSB_CFLAGS)
LDFLAGS=-lm `pkg-config libusb-1.0 --libs`

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

