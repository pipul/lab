obj = ts.o _ts_configure.o _ts_handledict.o
ts: $(obj)
	gcc $(obj) -o ts -lapue
ts.o: src/ts.c include/_ts_configure.h include/_ts_handledict.h
	gcc -g -c src/ts.c
_ts_configure.o: include/_ts_configure.h
	gcc -g -c include/_ts_configure.c
_ts_handledict.o: include/_ts_handledict.h
	gcc -g -c include/_ts_handledict.c

.PHONY:clean
clean:
	-rm *.o
