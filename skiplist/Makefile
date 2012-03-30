obj = skiplist_test.o skiplist.o utils.o
skiplist: $(obj)
	gcc $(obj) -W -o skiplist
skiplist_test.o: skiplist_test.c skiplist.h utils.h
	gcc -g -c skiplist_test.c
skiplist.o: skiplist.c utils.h
	gcc -g -c skiplist.c
utils.o: utils.c
	gcc -g -c utils.c

.PHONY:clean
clean:
	-rm *.o
