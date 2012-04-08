#include "apue.h"
#include <stdio.h>

int sig(int );
int matrix[7][7]={{-1,-1,-1,-1,-1,1,-1},{-1,-1,-1,-1,-1,1,-1},
	{1,1,-1,-1,-1,1,-1},{1,1,-1,-1,-1,1,-1},{1,1,1,1,-1,1,-1},
	{1,1,1,1,1,1},{-1,-1,-1,-1,-1,0,-1}};
typedef struct stack{
	struct stack *next;
	int data;
}p_stack_t;

void p_stack_init(p_stack_t *q,int para){
	p_stack_t arg;
	arg=(void *)malloc(sizeof(p_stack_t));
	arg->data=para;
	if(q!=NULL)
		arg->next=q;
	else
		arg->next=NULL;
	q=arg;
}

int p_stack_get(p_stack_t *q){
	p_stack_t *des;
	int ret;
	if(q!=NULL){
		ret=q->data;
		des=q;
		q=q->next;
		free(des);
	}
	return(ret);
}

int main(void)
{
	int c;
	p_stack_t *sbs;

	printf("please input the contain:");
	c=fgetc(stdin);
	while(c!=10){
		if((c<=57)&&(c>=48))
			printf("%d",c-48);
		else if((c==43)||(c==45)||(c==42)||(c==47)||(c==94)||(c==40)||(c==41)){
			while(1){
				if(sbs==NULL){
					p_stack_init(sbs,c);
					break;
				}
				else{
					if(matrix[sig(c)][sig(sbs->data)]==1){
						p_stack_init(sbs,c);
						break;
					}
					else if(c==41){
						while(1){
							if(sbs->data==40){
								p_stack_get(sbs);
								break;
							}else if(sbs==NULL)
								err_quit("ERROR.!");
							else{
								printf("%c",p_stack_get(sbs));
								break;
							}
						}
					}else printf("%c",p_stack_get(sbs));
				}
			}

		}else{
			err_quit("wrong input");
		}
		c=fgetc(stdin);
	}
	while(1){
		if(sbs!=NULL)
			exit(0);
		else
			printf("%c",p_stack_get(sbs));
	}
}

int sig(int arg){
	switch(arg){
		case 43:return 0;
		case 45:return 1;
		case 42:return 2;
		case 47:return 3;
		case 94:return 4;
		case 40:return 5;
		case 41:return 6;
	}
}

