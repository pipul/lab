CC = gcc
CFLAGS = -g

LIB_OBJS = bloom.o md5.o mutable.o sst.o \
	 slice.o aot.o db.o sloter.o cp.o
LIBRARY = liblotus.a

all:$(LIBRARY)

clean:
	-rm -f *.o
$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)