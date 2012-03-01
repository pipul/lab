objcli = client.o net.o
client: $(objcli)
	gcc $(objcli) -o client
client.o: client.c net.h
	gcc -g -c client.c
net.o:net.h
	gcc -g -c net.c

.PHONY:clean
clean:
	-rm *.o
