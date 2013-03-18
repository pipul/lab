#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 256

#define SLIDEWIN 63	/* 6bits: 63=64-1=2^6-1 contain 6bits	*/
#define MATCHLEN 2	/* 2bits: also 3=2^2-1 contain 2bits	*/
#define PTHREADS 25	/* Sopporting decompress by pthreads	*/


#define BUFFSIZE 65536

static unsigned char buffer[BUFFSIZE];
static size_t hTable[HASHSIZE][SLIDEWIN+1];

int hashInsert(size_t , size_t );
int hashDelete(size_t , size_t );
int insertSort(size_t *, size_t );
int maxMatch(size_t , size_t * , size_t );
int inslide(FILE *, FILE *);
int deslide(FILE *, FILE *);

int insertSort(size_t *s, size_t len)
{
	/*
	 * insertion sort:
	 *
	 * compare all the array elements and
	 * all larger elements are moved one pot to the left.
	 *
	 */

	int i,j;
	for(i=1;i<(int)len;i++){
		for(j=i;j>0;j--){
			if(s[j]<s[j-1]){
				s[j]^=s[j-1];
				s[j-1]^=s[j];
				s[j]^=s[j-1];
			}
		}
	}
	return(0);
}

int hashInsert(size_t value, size_t position)
{
	int l=1,c;
	size_t *p=hTable[value],h=p[0];

	/*
	 * check the elements whether have been appeared
	 * in the slideWindows
	 * hTable[value][0] save the all Occurrences witch
	 * value is value.
	 *
	 */

	if(h==0){
		p[0]++;
		p[1]=position;
	}else if(h>0){

		/*
		 * find the first place whose value just
		 * less than position a little.
		 *
		 */

		while(l<h){
			c=(l+h)>>1;
			if(p[c]<position)
				l=c+1;
			else if(p[c]>position)
				h=c-1;
			else
				h=c;
		}
		
		/*
		 * found! OK.
		 * the elements occurrences add 1 and take the
		 * first location's value to the last one.
		 * also,save the position in the plase witch just
		 * have found and insertion_sort the right of the
		 * array.
		 *
		 */

		p[0]++;
		p[p[0]]=p[l];
		p[l]=position;
		insertSort(p+l,p[0]-l+1);
	}

	return(0);
}

int hashDelete(size_t value, size_t position)
{
	int l=1,c=0,t=0;
	size_t *p=hTable[value],h=p[0];
	
	if(h==1){
		if(p[1]==position){
			t=1;
			c=1;
		}
	}else if(h>1){
		while(l<=h){
			c=(l+h)>>1;
			if(p[c]<position)
				l=c+1;
			else if(p[c]>position)
				h=c-1;
			else{
				t=1;
				break;
			}
		}
	}
	
	if(t==1){
		p[c]=p[p[0]];
		p[p[0]]=0;
		p[0]--;

		insertSort(p+c,p[0]-c+1);
	}else{
		return(-1);
	}

	return(0);
}

int maxMatch(size_t value, size_t *ptr, size_t n)
{
	/*
	 * c: the current maxMatch's length
	 * r: the last bytes save the match's length
	 *    the firsh bytes save the match's index
	 *    if the c=5 and the recall=ptr-m=40, so
	 *    r=40<<8+5
	 */

	size_t c=0,r=0,*p=hTable[value];

	/*
	 * m: the maxMatch's index and the l represent the current
	 * match length
	 *
	 */

	size_t i,j,m,l,s;

	if(*ptr==n||p[0]==0){
		r=(size_t)buffer[*ptr];
		hashInsert(buffer[*ptr],*ptr);
		if(*ptr>SLIDEWIN-1)
			hashDelete(buffer[*ptr-SLIDEWIN],*ptr-SLIDEWIN);
		(*ptr)++;
		//return((int)r);
	}else{
		
		/*
		 * version 1.2
		 *

		j=m=hTable[value][1+(rand()%(hTable[value][0]))];
		s=*ptr;
		c=0;
		while(buffer[j]==buffer[s]){
			j++;
			s++;
			c++;
			if(s>=n||c>=((2<<MATCHLEN-1)-1))
				break;
		}
		*/

		for(i=1;i<=p[0];i++){
			l=0;
			s=*ptr;
			j=p[i];

			while(buffer[j]==buffer[s]){
				j++;
				s++;
				l++;
				if(s>=n||l>=((2<<MATCHLEN-1)-1))
					break;
			}

			if(l>c){
				c=l;
				m=j-l;
			}
		}
		r=*ptr-m;
		r=(r<<MATCHLEN)+c;
	
	/*
	 * insert the match string with the last one to hash
	 * table and delete
	 * the same length of string from hash table header
	 *
	 */

		for(i=0;i<c;i++){
			hashInsert(buffer[*ptr],*ptr);
			if(*ptr>SLIDEWIN-1)
				hashDelete(buffer[*ptr-SLIDEWIN],*ptr-SLIDEWIN);
			(*ptr)++;
		}

		r=r<<8;
	}

	return((int)r);
}

int inslide(FILE *in, FILE *out)
{
	int ifile=fileno(in);
	int n,ptr;
	size_t segment=0;

	while((n=read(ifile,buffer,BUFFSIZE))>0){
		ptr=0;
		memset(hTable,0,HASHSIZE*(SLIDEWIN+1)*sizeof(size_t));
		while(ptr<n){
			segment=(size_t)maxMatch(buffer[ptr],&ptr,n-1);
			if((segment>>8)!=0)
				fputc((unsigned char)(segment>>8),out);
			else{
				fputc((unsigned char)(segment>>8),out);
				fputc((unsigned char)segment,out);
			}
		}
		memset(buffer,0,BUFFSIZE*sizeof(unsigned char));
	}
}

int deslide(FILE *in, FILE *out)
{
	int i,ptr=0;
	
	/*
	 * r: the length of recall.
	 * l: the match length.
	 * c: the last one charactor of each match.
	 *
	 */

	size_t r=0,l=0;
	unsigned char c;

	while(!feof(in)){

		memset(buffer,0,BUFFSIZE*sizeof(unsigned char));
		ptr=0;

		while(ptr<BUFFSIZE){

			r=(size_t)fgetc(in);

			/*
			 * important! see the "#define" for more
			 * detail, SLIDEWIN and MATCHLEN
			 *
			 */

			if(r!=0 && !feof(in)){
				l=r & ((2<<MATCHLEN-1)-1);
				r=r>>MATCHLEN;
				for(i=ptr;i<=(int)(ptr+l-1);i++){
					buffer[i]=buffer[i-r];
				}
				ptr+=l;
			}else{
				c=(unsigned char)fgetc(in);
				if(!feof(in))
					buffer[ptr++]=c;
				else
					break;
			}
		}
		
		for(i=0;i<ptr;i++){
			if(buffer[i]!=0)
				fputc(buffer[i],out);
			else
				break;
		}
	}

}


size_t main(size_t argc,char **argv)
{
	FILE *ifile,*ofile;

	if(argc!=4){
		printf("...\n");
		exit(0);
	}

	if((ifile=fopen(argv[2],"r"))==NULL){
		printf("Error for open %s file.\n",argv[2]);
		exit(0);
	}

	if((ofile=fopen(argv[3],"w"))==NULL){
		printf("Error for create %s file.\n",argv[3]);
		exit(0);
	}
	
	if(strcmp(argv[1],"-i")==0)
		inslide(ifile,ofile);
	else if(strcmp(argv[1],"-u")==0)
		deslide(ifile,ofile);
	
	fclose(ifile);
	fclose(ofile);

	return(0);
}
