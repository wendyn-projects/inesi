CC := gcc
AR := ar

CFLAGS := -std=c99 -pedantic -Wall -fanalyzer -O3
LDFLAGS :=

.PHONY: all clean

all: libinesi.a inesi

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lib%.a: %.o
	$(AR) -rcs $@ $<

inesi: main.c inesi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) inesi
