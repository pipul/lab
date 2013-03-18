#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

#define INT2S_BUFSIZE 10

   
static char *  
int2s(int x, char *s)  
{  
    unsigned i;  
   
    /* LINTED */  
    assert(sizeof(int) <= 8);  
   
    i = INT2S_BUFSIZE - 1;  
    s[i] = '\0';  
    do {  
        i--;  
        s[i] = "0123456789"[x % 10];  
        x /= (int)10LL;  
    } while (x > 0);  
   
    return (&s[i]);  
}  

int main(int argc, char **argv)
{
	int fd = 0;
	int sfd = 0;
	int fn = 0;
	char buf[INT2S_BUFSIZE];
	char *s;	
	srand(time(NULL));
	umask(0000);
	while(fd!=-1){
		fn = rand()%(1000 - 5 + 1) + 5;
		s = int2s(fn,buf);
		printf("%d %s\n",fn,s);
		fd = open(s, O_CREAT, 0664);
		if (fd != -1)
			sfd = sfd+1;
		close(fd);
		printf("%d ",fd);
	}
	printf("\n\n%d\n",sfd);
	return(0);
}
