# Copyright 2012 The Go Authors.  All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

CC = gcc
CFLAGS = -Wall -Wno-sign-compare -Wno-missing-braces -Wno-parentheses -Wno-unknown-pragmas -Wno-switch -Wno-comment -Werror -fno-common -ggdb -c -m32 -I ../../include
LIBOBJS = 8.o 8db.o 8obj.o access.o executable.o fakeobj.o linux.o machdata.o map.o obj.o setmach.o swap.o sym.o

all:$(LIBOBJS)

lib:
	ar rsc libmach.a $(LIBOBJS)
clean:
	rm *.o -rf
