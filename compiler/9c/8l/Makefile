CC = gcc
CFLAGS = -Wall -Wno-sign-compare -Wno-missing-braces -Wno-parentheses -Wno-unknown-pragmas -Wno-switch -Wno-comment -Werror -fno-common -ggdb -m32 -I ../include -I ./
LIBOBJS = ld/data.o ld/dwarf.o ld/elf.o ld/go.o ld/ldelf.o ld/ldmacho.o ld/ldpe.o ld/lib.o ld/macho.o ld/pe.o ld/symtab.o asm.o enam.o list.o obj.o optab.o pass.o prof.o span.o

all:$(LIBOBJS)
build:
	$(CC) $(CFLAGS) -o ../bin/8l $(LIBOBJS) ../src/libbio/libbio.a ../src/lib9/lib9.a ../src/libmach/libmach.a
clean:
	rm $(LIBOBJS) -rf
