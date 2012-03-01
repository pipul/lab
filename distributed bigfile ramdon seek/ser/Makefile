objser = server.o ae.o net.o conhash.o debug.o
server: $(objser)
	gcc $(objser) -o server
server.o: server.c ae.h net.h conhash.h debug.h
	gcc -g -c server.c
ae.o: ae.h
	gcc -g -c ae.c
net.o: net.h
	gcc -g -c net.c
conhash.o: conhash.h
	gcc -g -c conhash.c
debug.o: debug.h
	gcc -g -c debug.c

.PHONY:clean
clean:
	-rm *.o
