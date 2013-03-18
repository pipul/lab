#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#define INT2S_BUFSIZE 10
#define NAME_MAX 255
#define BUFFSIZE 4096

int __cp_file(int src, int dest) {
	int n;
	char buf[BUFFSIZE];
	
	while ((n = read(src, buf, BUFFSIZE)) > 0)
		if (write(dest, buf, n) != n)
			/* write error. */
			return(-1);
	
	if (n < 0)
		/* read error. */
		return(-1);
	
	return(0);
}

int __cp_dir(const char *src, const char *dest) {
	char srcPath[NAME_MAX] = "", *_SRCPATH;
	char destPath[NAME_MAX] = "", *_DESTPATH;
	
	DIR *sdp, *ddp;
	struct dirent *dir;
	
	umask(0000);
	
	if ((sdp = opendir(src)) == NULL)
		/* that's not a direct. */
		return(-1);

	if ((ddp = opendir(dest)) == NULL)
		mkdir(dest, 0751);
	else
		closedir(ddp);

	
	strcpy(srcPath, src); _SRCPATH = srcPath + strlen(srcPath);
	strcpy(destPath, dest); _DESTPATH = destPath + strlen(destPath);
	
	while (*(_SRCPATH - 1) == '/') {
		_SRCPATH--;
	}
	
	while (*(_DESTPATH - 1) == '/') {
		_DESTPATH--;
	}
	
	*_SRCPATH++ = '/';
	*_DESTPATH++ = '/';
	
	int sfd, dfd;
	
	while ((dir = readdir(sdp)) != NULL) {
		
		if (strcmp(dir->d_name, ".") == 0 ||
					strcmp(dir->d_name, "..") == 0)
			continue;
		*_SRCPATH = '\0';
		*_DESTPATH = '\0';
		strcpy(_SRCPATH, dir->d_name);
		strcpy(_DESTPATH, dir->d_name);
		
		sfd = open(srcPath, O_RDONLY);
		if (sfd == -1) {
			__cp_dir(srcPath, destPath);
			continue;
		}
		
		if ((dfd = open(destPath, O_WRONLY|O_CREAT, 0644)) == -1)
			return(-1);
		
		/* staring copy the file. */
		__cp_file(sfd,dfd);
			
		close(sfd);
		close(dfd);
		
	}
	
	closedir(sdp);
	return(0);
}
   
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

char *bm(const char *s) {
	while (*s++ != 'i');
	return(s);
}

int main(int argc, char **argv)
{
	char buf[123] = "wo ai ni.";
	bm(buf);
	printf("%s\n",buf);
	return(0);
}
