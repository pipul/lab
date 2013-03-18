CC = gcc
CFLAGS = -Wall -Wno-sign-compare -Wno-missing-braces -Wno-parentheses -Wno-unknown-pragmas -Wno-switch -Wno-comment -Werror -fno-common -ggdb -m32 -I ../include -I ./ -DGOEXPERIMENT=\"\"
LIBOBJS = ../8l/enam.o gc/cplx.o gc/pgen.o cgen.o cgen64.o galign.o ggen.o gobj.o gsubr.o list.o peep.o reg.o gc/align.o gc/bits.o gc/builtin.o gc/closure.o gc/const.o gc/dcl.o gc/esc.o gc/export.o gc/fmt.o gc/gen.o gc/init.o gc/inl.o gc/lex.o gc/md5.o gc/mparith1.o gc/mparith2.o gc/mparith3.o gc/obj.o gc/order.o gc/range.o gc/reflect.o gc/select.o gc/sinit.o gc/subr.o gc/swt.o gc/typecheck.o gc/unsafe.o gc/walk.o gc/y.tab.o

all:$(LIBOBJS)
build:
	$(CC) $(CFLAGS) -o ../bin/8g $(LIBOBJS) ../src/libmach/libmach.a ../src/libbio/libbio.a ../src/lib9/lib9.a -lm

clean:
	-rm $(LIBOBJS) -rf
