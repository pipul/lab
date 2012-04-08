obj = main.o _cmcc_interface.o _cmcc_function.o
debug/cuclient: $(obj)
	gcc -o debug/cuclient $(obj) -lmysqlclient
main.o: main.c _cmcc_interface.h
	gcc -c main.c
_cmcc_interface.o: _cmcc_interface.h
	gcc -c _cmcc_interface.c
_cmcc_function.o: _cmcc_interface.h _cmcc_function.h

.PHONY:clean
clean:
	-rm *.o
