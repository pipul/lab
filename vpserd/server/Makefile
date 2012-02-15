obj = src/vpserd.o src/ae.o src/anet.o src/file.o src/cmd.o
vpserd: $(obj)
	gcc $(obj) -o vpserd -lunp
vpserd.o: vpserd.c ae.h anet.h file.h cmd.h
	gcc -g -c vpserd.c
ae.o: ae.h
	gcc -g -c ae.c
anet.o: anet.h
	gcc -g -c anet.c
file.o: file.h
	gcc -g -c file.c
cmd.o: cmd.h file.h
	gcc -g -c cmd.c

.PHONY:clean
clean:
	-rm src/*.o
