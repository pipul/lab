#include "apue.h"
#include <math.h>
#define MAXSIZE 64

static int casi(int );
static char nt[MAXSIZE];  //The type of the number ,real or unsigned
static float w=0,p=0,j=0;
static int d=0,e=1;
static void le();

int main(void)
{
	if(casi(d=fgetc(stdin))!=1)
		err_quit("Error1:local the wrong number!\n");
	while(casi(d)==1){
		w=w*10+(d-48);
		d=fgetc(stdin);
	}
	if(d==46){
		if(casi(d=fgetc(stdin))!=1)
			err_quit("Error2:local the wrong number!\n");
		while(casi(d)==1){
			w=w*10+(d-48);
			j=j+1;
			d=fgetc(stdin);
		}
		if(d==69) le();
		else if(d==59) printf("%f\n",w*pow(10,e*p-j));
		else err_quit("Error3:local the wrong number!\n");
	}else if(d==69) le();
	else if(d==59) printf("%f\n",w*pow(10,e*p-j));
	else err_quit("Error4:local the wrong number!\n");
}	

static int casi(int arg){
	if(arg>=48&&arg<=57)	return 1;	//indicate number
}
static void le(){
	if((d=fgetc(stdin))==45){
		e=-1;
		d=fgetc(stdin);
	}else if(d==43)	d=fgetc(stdin);
	while(casi(d)==1){
		p=p*10+(d-48);
		d=fgetc(stdin);
	}
	if(d==59) printf("%f\n",w*pow(10,e*p-j));
	else err_quit("Error5:local the wrong number!\n");	
}
