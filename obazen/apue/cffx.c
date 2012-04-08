#include "apue.h"
#include <stdio.h>

static int checknumber(int );  //success return 1 , error return 0
static long w=0;
static int p=0,j=0,d=0,e=1;

int main()
{
	while((d=fgetc(stdin)-48)!=11)
	{
		if(checknumber(d))
			w=w*10+d;
		else if(d==-2)
		{
			while((d=fgetc(stdin)-48)!=11)
			{
				if(checknumber(d))
				{
					w=w*10+d;
					j=j+1;
				}
				else if(d==21)
				{
					if((d=fgetc(stdin))==45)
						e=-1;
					else if(d==43)
						e=1;
					else{
						printf("local the wrong number!");
						exit(1);
					}
					while((d=fgetc(stdin)-48)!=11)
						if(checknumber(d))
							p=p*10+d;
				}
			}
		}

		else
		{
			err_ret("local the wrong number!\n");
			break;
		}
	}
	printf("%l",w*10^(p-j));
}

static int checknumber(int arg)
{
	if(arg>9||arg<0)
		return 0;
	else return 1;
}

