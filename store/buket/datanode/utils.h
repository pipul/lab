#ifndef __UTILS_H_
#define __UTILS_H_



long fs_get_free_inodes(const char *path);
long fs_get_free_blocks(const char *path);
long fs_get_block_size(const char *path);
long fs_get_free_space_size(const char *path);








#endif
