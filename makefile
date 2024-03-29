CFLAGS := -std=c99 -pedantic -Wall -fanalyzer -O3
LDFLAGS := -L. -linesi
LDFLAGS_STATIC :=

LIB_TARGETS := libinesi.so
BIN_TARGETS := inesi
ifdef AR
	LIB_TARGETS += libinesi.a
	BIN_TARGETS += inesi_static
endif
SCRIPT_CREATE_PC := create_pc

check-var-defined = $(if $(value $1),,$(error `$1` is not defined.))

.PHONY: all install install_dev install_lib install_include install_pc install_bin clean

all: $(LIB_TARGETS) $(BIN_TARGETS)
static: libinesi.a inesi_static

install: install_dev install_bin

install_dev: install_lib install_include install_pc

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

lib%.so: %.o
	$(CC) $(CFLAGS) -shared -o $@ $<

inesi: main.c libinesi.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

%_static.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lib%.a: %_static.o
	$(call check-var-defined,AR)
	$(AR) -rcs $@ $<

inesi_static: main.c libinesi.a
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -static

%.pc: $(SCRIPT_CREATE_PC)
	bash $< > $@ || ( rm $@ && exit 1 )

install_lib: $(LIB_TARGETS)
	$(call check-var-defined,PREFIX)
	cp $^ $(PREFIX)/lib

install_include: inesi.h
	$(call check-var-defined,PREFIX)
	cp $^ $(PREFIX)/include

install_pc: inesi.pc install_lib install_include
	$(call check-var-defined,PREFIX)
	mkdir -p $(PREFIX)/lib/pkgconfig
	cp $< $(PREFIX)/lib/pkgconfig

install_bin: inesi install_lib
	$(call check-var-defined,PREFIX)
	cp $< $(PREFIX)/bin

clean:
	rm -f $(wildcard *.o)
