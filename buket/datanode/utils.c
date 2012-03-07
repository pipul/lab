#include "utils.h"
#include <sys/vfs.h>
#include <stdio.h>
#include <string.h>

long fs_get_free_inodes(const char *path)
{
	struct statfs fs_stat;
	
	if (path == NULL)
		return(0);
	if (strlen(path) == 0)
		return(0);

	statfs(path,&fs_stat);
	return(fs_stat.f_ffree);
}

long fs_get_free_blocks(const char *path)
{
	struct statfs fs_stat;
	
	if (path == NULL)
		return(0);
	if (strlen(path) == 0)
		return(0);
	
	statfs(path,&fs_stat);
	return(fs_stat.f_bfree);
}

long fs_get_block_size(const char *path)
{
	struct statfs fs_stat;
	
	if (path == NULL)
		return(0);
	if (strlen(path) == 0)
		return(0);
	
	statfs(path,&fs_stat);
	return(fs_stat.f_bsize);
}

long fs_get_free_space_size(const char *path)
{
	struct statfs fs_stat;
	long _free_space_size;
	
	if (path == NULL)
		return(0);
	if (strlen(path) == 0)
		return(0);
	
	statfs(path,&fs_stat);
	_free_space_size = fs_stat.f_bfree >> 20;
	_free_space_size = _free_space_size*fs_stat.f_bsize >> 10;

	return(_free_space_size);
}
	
	
