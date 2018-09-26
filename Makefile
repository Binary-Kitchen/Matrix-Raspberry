VERSION_MAJOR=0
VERSION_MINOR=3
TARGET=matrixd

DESTDIR ?= /
PREFIX ?= /usr
SYSCONFDIR ?= /etc

CC=gcc
CFLAGS=-ggdb -O3 -Wall -Wextra
LDFLAGS=-pthread -lwiringPi

COMMIT_HASH=$(shell git log -1 --format=%h)
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)

$(OBJS): config.h

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

config.h:
	echo \#define VERSION \"$(VERSION_MAJOR).$(VERSION_MINOR)\" > $@
	echo \#define COMMIT_HASH $(COMMIT_HASH) >> $@

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f config.h

install: $(TARGET) matrix-mqtt
	mkdir -p $(DESTDIR)/$(PREFIX)/bin/
	mkdir -p $(DESTDIR)/$(SYSCONFDIR)/systemd/system

	install $^ $(DESTDIR)/$(PREFIX)/bin/
	install -m 0644 matrix-mqtt.service matrixd.service $(DESTDIR)/$(SYSCONFDIR)/systemd/system
