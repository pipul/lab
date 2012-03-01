objcli = node.o net.o ae.o
node: $(objcli)
	gcc $(objcli) -o node
node.o: node.c net.h ae.h
	gcc -g -c node.c
net.o:net.h
	gcc -g -c net.c
ae.o:ae.h
	gcc -g -c ae.c

.PHONY:clean
clean:
	-rm *.o
