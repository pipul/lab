obj = vps.o anet.o
vps: $(obj)
	gcc $(obj) -o vps -lunp
vps.o: vps.c anet.h
	gcc -g -c vps.c
anet.o: anet.h
	gcc -g -c anet.c
	
.PHONY:clean
clean:
	-rm *.o
