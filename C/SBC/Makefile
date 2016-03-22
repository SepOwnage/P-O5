LDFLAGS=-lm
CFLAGS=-Wall -ansi -pedantic-errors -g -O0 -std=c99
SBC: main.o wavpcm_io.o encode.o globals.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
