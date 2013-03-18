#include "util.c"
#include <stdio.h>

int main(int argc, char **argv)
{
	printf("%ld gb\n",fs_get_free_space_size(argv[1]));
	return(0);
}
