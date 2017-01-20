AR ?= ar
CC ?= gcc
PREFIX ?= /usr/local

CFLAGS = -O3 -Wall -g

SRC = init.c


all: c_init

install: all
	cp -f c_init $(PREFIX)/bin/c_init

uninstall:
	rm -f $(PREFIX)/bin/c_init


c_init: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@


clean: 
	rm -rf c_init *.o

.PHONY: clean install uninstall

