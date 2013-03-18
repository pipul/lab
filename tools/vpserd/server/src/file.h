#ifndef _FILE_H_
#define _FILE_H_

int _rm(const char *path, int fileNo);
int _touch(const char *fileName, int fileNo);
int _cp(const char *src, const char *dest, int fileNo);

/*
int _mv(const char *src, const char *dest, int op);
int _zip(const char *file);
int _unzip(const char *fileName);
int _chmod(const char *file, int mask);
int _chown(const char *file, int mask);
int _chgrp(const char *file, int mask);
int _ll(void);
int _ls(void);
int _cd(const char *path);
int _get(const char *file);
int _put(const char *file);
int _down(const char *link);
*/

#endif
