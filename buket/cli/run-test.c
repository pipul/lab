#include "buket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


long long _ustime(void)
{
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust / 1000000;
}

int main(int argc, char **argv)
{
	BFILE *fp;
	time_t start,end;
	int count,n,offset;
	char BUFF[11] = "";

	srand((unsigned)time(NULL));

	start = _ustime();
	count = 0;
	while (count < 10000) {
		snprintf(BUFF,4,"%d",rand()%1000);
		offset = rand()%3972843;

		fp = b_open("127.0.0.1","7777",BUFF);

		if ((n = b_read(fp,BUFF,10,offset)) > 0) {
			BUFF[n] = '\0';
			printf("%s\n",BUFF);
		}

		b_close(fp);		
		count++;
	}
	end = _ustime();

	printf("Cost = 10000 process --> %lds\n",end - start);
}
