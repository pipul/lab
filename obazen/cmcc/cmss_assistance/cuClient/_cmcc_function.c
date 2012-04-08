#include "_cmcc_interface.h"
#include "_cmcc_function.h"
#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define DataBase "cmcc2"

#define MAX_SIZE _terminal_width
#define hostname "localhost"
#define rootname "root"
#define password "136136"

MYSQL mysql,*Conn;
MYSQL_FIELD *fields;
MYSQL_RES *res;
MYSQL_ROW row;
char anykey[2];

int _cmcc_function_init()
{
	char usname[20]={"root"},passwd[20]={"136136"};
	mysql_init(&mysql);
	do{
                if(!(Conn=mysql_real_connect(&mysql,"localhost",usname,passwd,DataBase,0,NULL,0)))
                {
                        fprintf(stderr,"%s\n",mysql_error(&mysql));
                }
        }while(!Conn);
        mysql_set_character_set(Conn,"utf8");
}

int _cmcc_function_identify(char *_p_number,char *_p_passwd,char *sqlCom)
{
	int i;
	memset(sqlCom,0,sizeof(sqlCom));
	strcat(sqlCom,"select * from phone where _p_number=");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,";");
	
	if(_cmcc_function_sqlCom(sqlCom)==0)
		return(0);
	row=mysql_fetch_row(res);
	if(strcmp(row[3],_p_passwd)!=0){
		for(i=4;i>0;i--)
			_ci_passwd_wrong(i-1);
		return(0);
	}
	return(1);
}

int _cmcc_function_sqlCom(char *sqlCom)
{
	int i;
	printf("%s",sqlCom);
	if(!mysql_query(Conn,sqlCom))
	{
		for(i=4;i>0;i--)
			_ci_system_busy(i-1);
		return(0);
	}
	if(!(res=mysql_store_result(Conn)))
	{
		for(i=4;i>0;i--)
			_ci_system_busy(i-1);
		return(0);
	}
	return(1);
}



int _cmcc_function_paym()
{
	char _p_number[12],token[5],sqlCom[MAX_SIZE],buffer[MAX_SIZE];
	int i;
	
	_p_number[11]='\0';
	_ci_paym_step_1();
	printf("\n请输入需要充值的手机号码(输入N返回主菜单)：");
	while(1)
	{
		scanf("%s",buffer);
		if((strlen(buffer)!=11)&&(strcmp(buffer,"N")!=0)){
			_ci_paym_step_1();
			printf("\n不正确的号码->请重新输入(输入N返回主菜单):");
		}else if(strcmp(buffer,"N")==0){
			return(0);
		}else break;
	}
	strcpy(_p_number,buffer);
	
	memset(buffer,0,sizeof(buffer));
	strcat(sqlCom,"select _p_name,_p_balance from phone where _p_number=");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,";");
	
	_cmcc_function_sqlCom(sqlCom);
	
	row=mysql_fetch_row(res);
	_ci_paym_step_2(row[0],_p_number,row[1]);

	printf("\n帐户信息是否正确(Y/N):");
	scanf("%s",token);
	if(strcmp(token,"Y")==0){
		_ci_paym_step_3();
		printf("\n请输入你需要充值的金额：");
		scanf("%s",token);
	}else{
		sleep(2);
		return(0);
	}
	
	memset(sqlCom,0,sizeof(sqlCom));
	strcat(sqlCom,"update phone set _p_balance=_p_balance+");
	strcat(sqlCom,token);
	strcat(sqlCom," where _p_number=");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,";");
	if(mysql_query(Conn,sqlCom)!=0)
	{
		for(i=4;i>0;i--)
			_ci_system_busy(i-1);
		return(0);
	}else{
		_ci_paym_step_4(_p_number,token,"55678998765162");
		sleep(2);
	}
	return(1);
}

int _cmcc_function_info()
{
	char _p_number[11],_p_passwd[16],sqlCom[MAX_SIZE];
	_p_number[11]='\0';
	
	system("clear");
	_cmcc_interface_header();
	printf("\n请输入你的手机号码：");
	scanf("%s",_p_number);
	printf("请输入你的服务密码：");
	scanf("%s",_p_passwd);
	
	if(_cmcc_function_identify(_p_number,_p_passwd,sqlCom)==0){
		return(0);
	}
	else
		_cmcc_interface_info(row[1],row[0],row[2],row[4]);
	
	printf("\n按下任意键回车返回主菜单：");
	scanf("%s",anykey);
	return(1);
}

int _cmcc_function_call()
{
	char _p_number[11],_p_passwd[16],sqlCom[MAX_SIZE];
	_p_number[11]='\0';
	
	system("clear");
	_cmcc_interface_header();
	printf("\n请输入你的手机号码：");
	scanf("%s",_p_number);
	printf("请输入你的服务密码：");
	scanf("%s",_p_passwd);
	
	if(_cmcc_function_identify(_p_number,_p_passwd,sqlCom)==0)
		return(0);
		
	memset(sqlCom,0,sizeof(sqlCom));
	strcat(sqlCom,"select * from ");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,"_call");
	strcat(sqlCom,";");
	
	_cmcc_function_sqlCom(sqlCom);

	_ci_call_header();
	while((row=mysql_fetch_row(res))!=NULL)
	{
		if(strcmp(row[2],"1")==0)
			_ci_call_row(row[0],row[1],"主叫",row[3],row[4]);
		else
			_ci_call_row(row[0],row[1],"被叫",row[3],row[4]);
	}
	_ci_call_border();
	_cmcc_interface_footer();
	printf("\n按下任意键回车返回主菜单：");
	scanf("%s",anykey);
	return(1);
}

int _cmcc_function_hsms()
{
	char _p_number[11],_p_passwd[16],sqlCom[MAX_SIZE];
	_p_number[11]='\0';
	
	system("clear");
	_cmcc_interface_header();
	printf("\n请输入你的手机号码：");
	scanf("%s",_p_number);
	printf("请输入你的服务密码：");
	scanf("%s",_p_passwd);
	
	if(_cmcc_function_identify(_p_number,_p_passwd,sqlCom)==0)
		return(0);
		
	memset(sqlCom,0,sizeof(sqlCom));
	strcat(sqlCom,"select * from ");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,"_sms");
	strcat(sqlCom,";");
	
	_cmcc_function_sqlCom(sqlCom);

	_ci_hsms_header();
	while((row=mysql_fetch_row(res))!=NULL)
	{
		if(strcmp(row[1],"1")==0)
			_ci_hsms_row(row[0],"发送",row[2],row[3]);
		else
			_ci_hsms_row(row[0],"接收",row[2],row[3]);
	}
	_ci_hsms_border();
	_cmcc_interface_footer();
	printf("\n按下任意键回车返回主菜单：");
	scanf("%s",anykey);
	return(1);
}

int _cmcc_function_hpay()
{
	char _p_number[11],_p_passwd[16],sqlCom[MAX_SIZE];
	_p_number[11]='\0';
	
	system("clear");
	_cmcc_interface_header();
	printf("\n请输入你的手机号码：");
	scanf("%s",_p_number);
	printf("请输入你的服务密码：");
	scanf("%s",_p_passwd);
	
	if(_cmcc_function_identify(_p_number,_p_passwd,sqlCom)==0)
		return(0);
		
	memset(sqlCom,0,sizeof(sqlCom));
	strcat(sqlCom,"select * from ");
	strcat(sqlCom,_p_number);
	strcat(sqlCom,"_payment");
	strcat(sqlCom,";");
	
	_cmcc_function_sqlCom(sqlCom);

	_ci_hpay_header();
	while((row=mysql_fetch_row(res))!=NULL)
	{
		if(strcmp(row[0],"1")==0)
			_ci_hpay_row("充值",row[1],row[2]);
		else
			_ci_hpay_row("预付费充值",row[2],row[3]);
	}
	_cmcc_interface_footer();
	printf("\n按下任意键回车返回主菜单：");
	scanf("%s",anykey);
	return(1);
}

