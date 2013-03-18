CC = gcc
CFLAGS = -g

LIB_OBJS = \
	./sstable.o	\
	./utils.o \
	./memtable.o

LIBRARY = liblsm.a

all: $(LIBRARY)

clean:  
	-rm -f $(LIB_OBJS)

$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)
