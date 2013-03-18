objlib = buket.o net.o
libbuket.a: $(objlib)
	ar rcs libbuket.a $(objlib)
buket.o: buket.h net.h
	gcc -g -c buket.c
net.o: net.h
	gcc -g -c net.c

.PHONY:clean
clean:
	-rm *.o
