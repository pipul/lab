#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define alphabetSize 256	/* define alphabet 0x00 ~ 0xfe */
#define BUFFSIZE 4096

int count_codeWord( FILE *, unsigned *, size_t );
int min_head( unsigned *, size_t, size_t );
int getlen_codeWord( unsigned *, size_t, unsigned *, size_t );
int inhuffman( FILE *, FILE * );
int dehuffman( FILE *, FILE * );

int count_codeWord(in,out,out_s)

FILE *in;	/* input buf what should be treated	*/
unsigned *out;	/* witch save the result of huffman..	*/
size_t out_s;	/* ..... */

{
	unsigned int ch;
	int total=0;

	if( out_s!= alphabetSize ){
		/* error handler */
		return(-1);
	}

	while(!feof(in)){
		
		ch=fgetc(in);
		
		if(feof(in)){
			return(total);
			/* error handler */
		}else if(ferror(in)){
			return(-1);
		}

		if(ch<out_s){
			out[ch]++;
			total++;
		}else{
			/* error handler */
			return(-1);
		}
	}

	return(total);
}

int min_head( head,s,e )
unsigned *head;
size_t s;	/* s represent the start of the sub-head */
size_t e;	/* e represent the end of the head */
{
	/* 
	 * important! the start index of head is 0.
	 * l : left child of s.
	 * r : right child of s.
	 *
	 */

	size_t l = (s+s)+1;
	size_t r = l+1;

	size_t min=s;

	/*
	 * impotant! remenber this.
	 * 1. s can not be zero
	 * 2. l,r also can not be zero at the same time.
	 *
	 */

	if( l>e ){
		/* error handle */
		return(-1);
	}
	
	if( r>e ){
		if( head[head[l]]<head[head[s]] ){
			head[s] ^= head[l];
			head[l] ^= head[s];
			head[s] ^= head[l];
		}
	}else{
		if( head[head[l]]<head[head[min]] )
			min = l;
		if( head[head[r]]<head[head[min]] )
			min = r;
		if ( min!=s ){
			head[s] ^= head[min];
			head[min] ^= head[s];
			head[s] ^= head[min];
		}

	}

	if( min!=s ){
		min_head( head,min,e );
	}else
		return(0);
}

int getlen_codeWord(in,in_s,out,out_s)

unsigned *in;		/* count of codeWord 			*/
size_t in_s;		/* the size of the input buf		*/
unsigned *out;		/* save the result:len of codeWord 	*/
size_t out_s;		/* the size of the output buf		*/

{
	unsigned int moffat[alphabetSize+alphabetSize];
	unsigned int num_codeWord=0;


	/*
	 * m1,m2 represent the most small key 
	 *
	 */
	unsigned int m1,m2;

	size_t h,i,d,r;

	if( out_s!=alphabetSize ){
		/* error handle	*/
		return(-1);
	}
	
	memset(moffat,0,(in_s+out_s)*4);

	for(i=0;i<alphabetSize;i++){
		if(in[i]!=0){
			moffat[num_codeWord+alphabetSize]=in[i];
			moffat[num_codeWord]=num_codeWord+alphabetSize;
			num_codeWord++;
		}
	}
	
	/*
	 * build the min-head
	 */
	h=num_codeWord-1;
	i=num_codeWord;
	while( i>0 ){
		min_head( moffat,i-1,num_codeWord-1 );
		i--;
	}

	/*
	 * to caculate codeWord length for the huffman code.
	 * 
	 */

	while(h>0){
		m1 = moffat[0];
		moffat[0] = moffat[h];
		h--;
		min_head( moffat,0,h );
		m2 = moffat[0];

		moffat[h+1]=moffat[m1]+moffat[m2];
		moffat[0]=moffat[m1]=moffat[m2]=h+1;
		min_head( moffat,0,h );
	}

	for(i=alphabetSize;i<num_codeWord+alphabetSize;i++){
		d=0;
		r=i;
		while(r>1){
			d++;
			r=moffat[r];
		}
		moffat[i]=d;
	}
	
	i=alphabetSize-1;
	while(num_codeWord!=0){
		if(in[i]!=0){
			out[i]=moffat[alphabetSize+num_codeWord-1];
			num_codeWord--;
		}
		i--;
	}
	return(0);

}

int inhuffman(in,out)

FILE *in;	/* input file */
FILE *out;	/* output file */

{
	
	size_t i,t=0,k=8,l;
	size_t total;
	unsigned char ch;
	unsigned cr,lm,rm;

	unsigned count[alphabetSize];

	/*
	 * important! save the length of each codeword.
	 */
	unsigned len[alphabetSize];

	unsigned maxlength=0,numl[alphabetSize];
	unsigned firstcode[alphabetSize],nextcode[alphabetSize];

	/*
	 * The rightmost len[i] bits of the integer codeword[i]
	 * are the code for symbol i.
	 */
	unsigned codeword[alphabetSize];

	memset(count,0,alphabetSize*4);
	memset(len,0,alphabetSize*4);
	memset(numl,0,alphabetSize*4);
	memset(firstcode,0,alphabetSize*4);
	memset(nextcode,0,alphabetSize*4);
	memset(codeword,0,alphabetSize*4);

	total=(unsigned int)count_codeWord(in,count,alphabetSize);
	getlen_codeWord(count,alphabetSize,len,alphabetSize);
	
	for(i=0;i<alphabetSize;i++){
		if(len[i]!=0)
			numl[len[i]]++;
		if(len[i]>maxlength)
			maxlength=len[i];
	}

	/*
	 * if maxlength more than 32,nextcode and codeword
	 * can't save the bit stream of coding result.
	 *
	 */

	if( maxlength>=32 ){
		/* error handler */
		return(-1);
	}

	for(i=maxlength-1;i>0;i--){
		firstcode[i]=(firstcode[i+1]+numl[i+1])/2;
		if(firstcode[i]*2<(firstcode[i+1]+numl[i+1]))
			firstcode[i]++;
		nextcode[i]=firstcode[i];
	}

	for(i=0;i<alphabetSize;i++){
		if(len[i]!=0){
			codeword[i]=nextcode[len[i]];
			nextcode[len[i]]++;
		}
	}
	
	
	/*
	 * Start canoncial huffman coding.
	 * first: reset the file point.
	 * second: once get a word, check the codeword[i]
	 *
	 */
	rewind(in);
	rewind(out);
	
	for(i=0;i<alphabetSize;i++)
		fputc((char)len[i],out);

	fputc((char)(total>>24),out);
	fputc((char)(total>>16),out);
	fputc((char)(total>>8),out);
	fputc((char)total,out);

	while(!feof(in)){
		if(k!=0){
			t=fgetc(in);
			if(feof(in)){
				if(k!=8){
					ch=ch<<k;
					fputc(ch,out);
				}
				break;
			}else{
				l=len[t];
				lm=rm=codeword[t];
			}
		}else{
			k=8;
			fputc(ch,out);
			ch='\0';
			rm=lm;
		}

		if( k>l ){
			ch=ch<<l;
			ch+=lm;
			k=k-l;
		}else{
			ch=ch<<k;
			l=l-k;
			lm=lm<<(32-l);
			lm=lm>>(32-l);
			rm=rm>>l;
			ch+=(unsigned char)rm;
			k=0;
		}
	}

	return(0);
}

int dehuffman(in,out)

FILE *in;	/* input file 	*/
FILE *out;	/* output file 	*/

{

	unsigned len[alphabetSize],numl[alphabetSize];
	unsigned codeword[alphabetSize];
	unsigned hash[alphabetSize+alphabetSize];
	unsigned firstcode[alphabetSize],nextcode[alphabetSize];

	size_t i,j,c,k=0,l=0,t=0;
	size_t total;
	unsigned char ch,cr;
	unsigned re=0,maxlength=0,num_codeword=0;
	
	memset(len,0,alphabetSize);
	memset(hash,0,alphabetSize*2);
	memset(codeword,0,alphabetSize);
	memset(numl,0,alphabetSize);
	memset(firstcode,0,alphabetSize);
	memset(nextcode,0,alphabetSize);

	/*
	 * get codeword's len from the input file.
	 * stroe in the first block. alphabetSize!
	 *
	 */
	
	rewind(in);
	rewind(out);

	for(i=0;i<alphabetSize;i++){
		len[i]=fgetc(in);
		if(len[i]!=0){
			numl[len[i]]++;
		}
		if(len[i]>maxlength){
			maxlength=len[i];
		}
	}

	for(i=0;i<4;i++){
		total=total<<8;
		total=total+(size_t)fgetc(in);
	}

	if( maxlength>=32 ){
		/* error handler */
		return(-1);
	}

	for(i=maxlength-1;i>0;i--){
		firstcode[i]=(firstcode[i+1]+numl[i+1])/2;
		nextcode[i]=firstcode[i];
	}

	for(i=0;i<alphabetSize;i++){
		if(len[i]!=0){
			codeword[i]=nextcode[len[i]];
			hash[num_codeword++]=i;
			nextcode[len[i]]++;
		}
	}
	
	for(i=1;i<num_codeword;i++){
		for(j=i;j>0;j--){
			if(codeword[hash[j]]<codeword[hash[j-1]]){
				hash[j]^=hash[j-1];
				hash[j-1]^=hash[j];
				hash[j]^=hash[j-1];
			}
		}
	}

	while(!feof(in)){

		if(k==l){
			ch=fgetc(in);
			k=l+8;
		}

		for(i=1;i<=8;i++){
			cr=ch;
			re=re<<i;
			re+=(size_t)(cr>>(8-i));
			l++;
			
			if(re<firstcode[l]){
				if(l==k){
					break;
				}else{
					re-=(size_t)(cr>>(8-i));
					re=re>>i;
					continue;
				}
			}
			else{
				t=1;
				break;
			}
		}
		if(t==1){

			/*
			 * binary search ---
			 * i:low bound
			 * j:hign bound
			 * c:current
			 *
			 */

			i=0;
			j=num_codeword-1;

			while(i<j){

				c=(i+j)>>1;
				
				if(codeword[hash[c]]<re)
					i=c+1;
				else if(codeword[hash[c]]>re)
					j=c-1;
				else{
					j=c;
				}
			}

			for(;i<num_codeword+j;i++){
				if(codeword[hash[i]]==re&&len[hash[i]]==l){
					fputc((unsigned char)hash[i],out);
					if(k>8)
						ch=ch<<(l-(k-8));
					else
						ch=ch<<l;
					k=k-l;
					total--;
					re=0;
					l=0;
					t=0;
					break;
				}
			}

		}
		
		if(total==0)
			break;

		if(ferror(in)){
			/* error handle */
			return(-1);
		}

	}

}

int main( size_t argc,char **argv)
{
	FILE *ifile;
	FILE *ofile;
	int h=0;
	char ch1,ch2;

	if( argc!=4 ){
		printf("...\n");
		exit(0);
	}
	
	if((ifile=fopen(argv[2],"r"))==NULL){
		printf("error for open %s file.\n",argv[1]);
		exit(0);
	}

	if((ofile=fopen(argv[3],"w+"))==NULL){
		printf("error for create %s file.\n",argv[2]);
		exit(0);
	}
	
	/*
	while(1){
		ch1=fgetc(ifile);
		ch2=fgetc(ofile);
		if(ch1!=ch2){
			printf("%d\n",h);
			break;
		}
		if(ch1=='\n')
			h++;
	}
	*/

	if(strcmp(argv[1],"-i")==0)
		inhuffman(ifile,ofile);
	else if(strcmp(argv[1],"-u")==0)
		dehuffman(ifile,ofile);

	fclose(ifile);
	fclose(ofile);

	return(0);
}
