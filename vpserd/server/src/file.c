#include "file.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

int _rm(const char *pathName, int fileNo)
{
	int OK = 0;
	char s[] = "File delete successful.\n";
	char e[] = "Error for deleting the file!\n";
	
	OK = remove(pathName);
	
	if (OK == 0)
		write(fileNo, s, strlen(s));
	else
		write(fileNo, e, strlen(e));
	
	return(OK);
}

int _touch(const char *fileName, int fileNo)
{
	int OK = 0;
	int fd;
	char s[] = "File create successful.\n";
	char e[] = "Error for creating the file!\n";
	
	umask(0000);
	fd = open(fileName, O_CREAT|O_RDONLY, 0644);
	if (fd != -1) {
		OK = -1;
		write(fileNo, s, strlen(s));
	} else {
		OK = 0;
		write(fileNo, e, strlen(e));
	}
	
	close(fd);
	return(OK);
}


#define NAME_MAX 255
#define BUFFSIZE 4096

int __cp_file(int src, int dest)
{
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

int __cp_dir(const char *src, const char *dest)
{
	char srcPath[NAME_MAX] = "", *_SRCPATH;
	char destPath[NAME_MAX] = "", *_DESTPATH;
	
	DIR *sdp, *ddp;
	struct dirent *dir;
	
	umask(0000);
	
	if ((sdp = opendir(src)) == NULL)
		/* that's not a direct. */
		return(-1);

	if ((ddp = opendir(dest)) == NULL)
		if (mkdir(dest, 0751) == -1)
			return(-1);
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
		
		__cp_file(sfd, dfd);
		close(sfd);
		close(dfd);
		
	}
	
	closedir(sdp);
	return(0);
}
		
		
	

int _cp(const char *src, const char *dest, int fileNo)
{
	int srcIsDir = 0, destIsDir = 0;
	int sfd, dfd;
	int OK = 0;
	char s[] = "Copy the file/direct successfully.\n";
	char e[] = "Error for copying the file/direct!\n";

	DIR *sdp, *ddp;
	struct dirent *sDir, *dDir;
	
	umask(0000);
	
	if ((sdp = opendir(src)) != NULL) {
		srcIsDir = 1;
		closedir(sdp);
	}
	if ((ddp = opendir(dest)) != NULL) {
		destIsDir = 1;
		closedir(ddp);
	}
		
	if (srcIsDir) {
		__cp_dir(src,dest);
	} else if (!destIsDir) {
		if ((sfd = open(src, O_RDONLY)) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		if ((dfd = open(dest, O_WRONLY|O_CREAT, 0644)) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		if (__cp_file(sfd,dfd) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
	} else {
		char destPath[BUFFSIZE] = "";
		char *_DESTPATH;
		const char *sf_start, *sf_end;
		
		strcpy(destPath, dest); _DESTPATH = destPath + strlen(destPath);
		
		while (*(_DESTPATH - 1) == '/') {
			_DESTPATH--;
		}
		*_DESTPATH++ = '/';
		
		/* Get the source file name. Caculate the fileName length. */
		sf_end = src + strlen(src); 
		
		while (*(sf_end - 1) == '/') {
			sf_end--;
		}
		sf_start = sf_end;
		while (*(sf_start -1) != '/') {
			sf_start--;
		}
		
		/* Invalid fileName. */
		if (sf_end - sf_start == 0) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		strncpy(_DESTPATH, sf_start, sf_end-sf_start);
		
		if ((sfd = open(src, O_RDONLY)) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		if ((dfd = open(destPath, O_WRONLY|O_CREAT, 0644)) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		if (__cp_file(sfd,dfd) == -1) {
			OK = -1;
			write(fileNo, e, strlen(e));
			return(OK);
		}
		
	}
	
	write(fileNo, s, strlen(s));
	close(sfd);
	close(dfd);
	
	return(OK);

}

#undef NAME_MAX
#undef BUFFSIZE


