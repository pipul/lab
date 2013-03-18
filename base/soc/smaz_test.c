#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smaz.h"

int main(void) {
    char in[512];
    char out[4096];
    
    char str[] = "This is a small string";


    smaz_compress(str,strlen(str),out,4096);
    printf("%s\n",out);
	smaz_decompress(out,strlen(out),in,512);
	printf("%s\n",in);
	
    return 0;
}
