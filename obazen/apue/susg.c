#include "apue.h"
#include <pwd.h>
#include <shadow.h>
#include <grp.h>

void searchuid(char *);
void searchsdw(char *);
void searchgid(char *);
char *itoa(int );
char *ltoa(long );
int  comp(char *,char *); /* success return 0,Wrong return -1 */
static char int2char[32];
static char long2char[64];


int main(int argc,char *argv[])
{
	struct passwd *userid;
	struct spwd *userpw;
	struct group *usergr;
	int sw;

	if((argc!=3)&&(argv[1]!="-u"|argv[1]!="-s"|argv[1]!="-g"))
		err_sys("usage:./susg <-u|-s|-g> <searching test");
	if(argv[1]="-n")
		sw=0;
	else if(argv[1]="-s")
		sw=1;
	else if(argv[1]="-g")
		sw=2;
	else if(argv[1]="-a")
		sw=3;

	switch(sw)
	{
		case 0:searchuid(argv[2]);
		       break;
		case 1:searchsdw(argv[2]);
		       break;
		case 2:searchgid(argv[2]);
		       break;
		case 3:searchuid(argv[2]);
		       searchsdw(argv[2]);
		       searchgid(argv[2]);
		       break;
	}

}

void searchuid(char *arg)
{
	struct passwd *uid;
	setpwent();
	while((uid=getpwent())!=NULL)
	{
		if(comp(arg,uid->pw_name)||comp(arg,uid->pw_passwd)||comp(arg,itoa(uid->pw_uid))||comp(arg,itoa(uid->pw_gid))||comp(arg,uid->pw_gecos)||comp(arg,uid->pw_dir)||comp(arg,uid->pw_shell))
			printf("%s;%s;%d;%d;%s;%s;%s;\n",uid->pw_name,uid->pw_passwd,uid->pw_uid,uid->pw_gid,uid->pw_gecos,uid->pw_dir,uid->pw_shell);
	}
	endpwent();
}

void searchsdw(char *arg)
{
	struct spwd *shad;
	setspent();
	while((shad=getspent())!=NULL)
	{
		if(comp(arg,shad->sp_namp)||comp(arg,shad->sp_pwdp)||comp(arg,ltoa(shad->sp_lstchg))||comp(arg,ltoa(shad->sp_min))||comp(arg,ltoa(shad->sp_max))||comp(arg,ltoa(shad->sp_warn))||comp(arg,ltoa(shad->sp_inact))||comp(arg,ltoa(shad->sp_expire))||comp(arg,ltoa(shad->sp_flag)))
			printf("%s;%s;%l;%l;%l;%l;%l;%l;%l;\n",shad->sp_namp,shad->sp_pwdp,shad->sp_lstchg,shad->sp_min,shad->sp_max,shad->sp_warn,shad->sp_inact,shad->sp_expire,shad->sp_flag);
	}
	endspent();
}

void searchgid(char *arg)
{
	struct group *gid;
	char *grm;
	setgrent();
	while((gid=getgrent())!=NULL)
	{
		if(comp(arg,gid->gr_name)||comp(arg,gid->gr_passwd)||comp(arg,itoa(gid->gr_gid)))
			printf("%s;%s;%d;",gid->gr_name,gid->gr_passwd,gid->gr_gid);
	}
	endgrent();
}

char *ltoa(long arg)
{
	sprintf(long2char,"%d",arg);
	return long2char;
}
char *itoa(int arg)
{
	sprintf(int2char,"%d",arg);
	return int2char;
}

int comp(char *arg,char *orig)
{
	if(strstr(arg,orig)!=NULL)
		return 1;
	else return 0;
}
