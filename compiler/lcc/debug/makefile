all:
	cd ..; make all
build:
	./rcc ../test/8q.c -o 8q.s
	as 8q.s -o 8q.o
	ld -m elf_i386 -dynamic-linker /lib/ld-linux.so.2 /usr/lib/crt1.o /usr/lib/crti.o /usr/local/lib/lcc/gcc/crtbegin.o -L/usr/local/lib/lcc/ -llcc -L/usr/local/lib/lcc/gcc -lgcc -lc -lm /usr/local/lib/lcc/gcc/crtend.o /usr/lib/crtn.o 8q.o -o a.out

clean:
	rm *.o
