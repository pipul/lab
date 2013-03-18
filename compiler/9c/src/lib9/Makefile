# Copyright 2012 The Go Authors.  All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

CC = gcc
CFLAGS = -Wall -Wno-sign-compare -Wno-missing-braces -Wno-parentheses -Wno-unknown-pragmas -Wno-switch -Wno-comment -Werror -fno-common -ggdb -c -m32 -DPLAN9PORT -I ../../include -I ./fmt -DGOOS=\"linux\" -DGOARCH=\"386\" -DGOROOT=\"/home/fangdong/go\" -DGOVERSION=\"go1.0.2\"

LIBOBJS = _exits.o _p9dir.o argv0.o atoi.o await.o cleanname.o create.o ctime.o dirfstat.o dirfwstat.o dirstat.o dirwstat.o dup.o errstr.o exec.o execl.o exitcode.o exits.o fmt/charstod.o fmt/dofmt.o fmt/dorfmt.o fmt/fltfmt.o fmt/fmt.o fmt/fmtfd.o fmt/fmtfdflush.o fmt/fmtlocale.o fmt/fmtlock.o fmt/fmtnull.o fmt/fmtprint.o fmt/fmtquote.o fmt/fmtrune.o fmt/fmtstr.o fmt/fmtvprint.o fmt/fprint.o fmt/nan64.o fmt/pow10.o fmt/print.o fmt/seprint.o fmt/smprint.o fmt/snprint.o fmt/sprint.o fmt/strtod.o fmt/test.o fmt/vfprint.o fmt/vseprint.o fmt/vsmprint.o fmt/vsnprint.o fmtlock2.o getenv.o getfields.o getuser.o getwd.o goos.o jmp.o main.o nan.o notify.o nulldir.o open.o readn.o rfork.o seek.o strecpy.o sysfatal.o time.o tokenize.o utf/rune.o utf/runetype.o utf/utfecpy.o utf/utflen.o utf/utfnlen.o utf/utfrrune.o utf/utfrune.o utf/utfutf.o

all:$(LIBOBJS)

lib:
	ar rsc lib9.a $(LIBOBJS)
clean:
	rm $(LIBOBJS) -rf
