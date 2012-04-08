#include <string.h>
#include <stdio.h>

int main()
{
	char a[]=" str len ";
	char *pbuf=a;
	int leek=0;
	printf("%s,%d\n",a,strlen(a));
	leek=strlen(a);
	pbuf=a;
	while(*pbuf==' '){
		pbuf++;
		leek--;
	}

	while(pbuf[leek-1]==' '){
		pbuf[leek-1]='\0';
		leek--;
	}

	printf("%s,%d\n",pbuf,strlen(pbuf));
}
