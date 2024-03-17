CC := gcc
AR := ar

CFLAGS := -std=c99 -pedantic -Wall -fanalyzer
LDFLAGS :=

all: libinesi.a inesi

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lib%.a: %.o
	$(AR) -rcs libinesi.a

inesi: main.c inesi.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) inesi
