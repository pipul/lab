CC = gcc
CFLAGS = -std=c99

LIB_OBJS = \
	./src/db.o 	\
	./src/hfile.o	\
	./src/hable.o	\
	./src/utils.o

LIBRARY = libdb.a

all: $(LIBRARY)

clean:
	-rm -f $(LIBRARY)  
	-rm -f $(LIB_OBJS)
	-rm -f src/db-bench.o 
	-rm -f db-bench
cleandb:
	-rm -rf dbase/*

$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)

db-bench: src/db-bench.o $(LIB_OBJS)
	$(CC) -g src/db-bench.o $(LIB_OBJS) -o $@ -lm
