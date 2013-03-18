# Copyright 2012 The Go Authors.  All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

CC = gcc
CFLAGS = -Wall -Wno-sign-compare -Wno-missing-braces -Wno-parentheses -Wno-unknown-pragmas -Wno-switch -Wno-comment -Werror -fno-common -ggdb -c -m32 -I ../../include
LIBOBJS = bbuffered.o bfildes.o bflush.o bgetc.o bgetd.o bgetrune.o binit.o boffset.o bprint.o bputc.o bputrune.o brdline.o brdstr.o bread.o bseek.o bwrite.o

all:$(LIBOBJS)

lib:
	ar rsc libbio.a $(LIBOBJS)
clean:
	rm *.o -rf

