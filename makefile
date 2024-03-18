CFLAGS := -std=c99 -pedantic -Wall -fanalyzer -O3
LDFLAGS :=

SCRIPT_CREATE_PC := create_pc

check-var-defined = $(if $1,,$(error `$1` is not defined.))

.PHONY: all install install_dev install_lib install include install_pc install_bin check_prefix clean

all: libinesi.a inesi

install: install_dev install_bin

install_dev: install_lib install_include install_pc

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lib%.a: %.o
	$(AR) -rcs $@ $<

inesi: main.c inesi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.pc: $(SCRIPT_CREATE_PC)
	bash $< > $@ || ( rm $@ && exit 1 )

install_lib: libinesi.a check_prefix
	cp $< $(PREFIX)/lib

install_include: inesi.h check_prefix
	cp $< $(PREFIX)/include

install_pc: inesi.pc check_prefix install_lib install_include
	mkdir -p $(PREFIX)/lib/pkgconfig
	cp $< $(PREFIX)/lib/pkgconfig

install_bin: inesi check_prefix
	cp $< $(PREFIX)/bin

check_prefix:
ifndef PREFIX
	$(error `PREFIX` is not defined.)
endif

clean:
	rm -f $(wildcard *.o)
