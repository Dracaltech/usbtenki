CC=gcc
LD=$(CC)
JAVA_HOME=/opt/jdk1.6.0_06

CFLAGS=-Wall -g -O  -I../common -fpic -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
LDFLAGS=-lusb -lm

PREFIX=/usr/local

PROGS=usbtenkiget usbtenkisetup

.PHONY : clean install

all: libtenki.so 

libtenki.so: usbtenki.o com_raphnet_tenki_USBTenkiDevice.o
	$(LD) -shared $(LDFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROGS)

install:
	@echo "Install not done yet. Sorry"

