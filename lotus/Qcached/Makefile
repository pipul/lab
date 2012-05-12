CC = gcc
CFLAGS = -g

LIB_OBJS = htree.o lirs.o qcached.o skiplist.o

LIBRARY = libqc.a

all: $(LIBRARY)

clean:  
	-rm -f $(LIB_OBJS)

$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)