CC = gcc
CFLAGS = -g

LIB_OBJS = ae.o anet.o zthread.o protobuf.o zqp.o

LIBRARY = libzqp.a

all: $(LIBRARY)

clean:  
	-rm -f $(LIB_OBJS)

$(LIBRARY): $(LIB_OBJS)
	rm -f $@
	$(AR) -rs $@ $(LIB_OBJS)
