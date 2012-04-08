#include "_cmcc_interface.h"	
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void _cmcc_interface_init()
{

	_ci_head_len_1 = 2*strlen(_ci_head_str_1)/3;
	_ci_pawd_table_left=(_terminal_width-4-_ci_pawd_len_1)>>1;
	_ci_pawd_table_right=_terminal_width-4-_ci_pawd_len_1-_ci_pawd_table_left;

	_ci_stby_table_left=(_terminal_width-4-_ci_stby_len_1)>>1;
	_ci_stby_table_right=_terminal_width-4-_ci_stby_len_1-_ci_stby_table_left;
	
	_ci_paym_table_width = 1+(_ci_info_td_1+1)+(_ci_info_td_2+1);
	_ci_pst2_len_1 = 2*strlen(_ci_pst2_str_1)/3;
	_ci_pst2_len_2 = 2*strlen(_ci_pst2_str_2)/3;
	_ci_pst2_len_3 = 2*strlen(_ci_pst2_str_3)/3;
	_ci_pst2_len_4 = 2*strlen(_ci_pst2_str_4)/3;
	_ci_pst2_th_left = (_ci_paym_table_width-2-_ci_pst2_len_1)>>1;
	_ci_pst2_th_right =  _ci_paym_table_width-2-_ci_pst2_len_1-_ci_pst2_th_left;
	_ci_pst3_len_1 = 2*strlen(_ci_pst3_str_1)/3;
	_ci_pst3_table_left = (_terminal_width-2-_ci_pst3_len_1)>>1;
	_ci_pst3_table_right = _terminal_width-2-_ci_pst3_len_1-_ci_pst3_table_left;
	_ci_pst4_len_1 = 2*strlen(_ci_pst4_str_1)/3;
	_ci_pst4_th_left = (_ci_paym_table_width-2-_ci_pst4_len_1)>>1;
	_ci_pst4_th_right = _ci_paym_table_width-2-_ci_pst4_len_1-_ci_pst4_th_left;
	
	_ci_paym_table_left=(_terminal_width-2-_ci_paym_table_width)>>1;
	_ci_paym_table_right=\
		_terminal_width-2-_ci_paym_table_width-_ci_paym_table_left;
		
	_ci_call_len_1 = 2*strlen(_ci_call_str_1)/3;
	_ci_call_len_2 = 2*strlen(_ci_call_str_2)/3;
	_ci_call_len_3 = 2*strlen(_ci_call_str_3)/3;
	_ci_call_len_4 = 2*strlen(_ci_call_str_4)/3;
	_ci_call_len_5 = 2*strlen(_ci_call_str_5)/3;
	_ci_call_td_1 = _ci_call_len_1+13; // 日期长度最短为19,要同标题长度一起考虑
	_ci_call_td_2 = _ci_call_len_2+2;
	_ci_call_td_3 = _ci_call_len_3+2;
	_ci_call_td_4 = _ci_call_len_4+5;  // 同样需要注意手机号码固定11位
	_ci_call_td_5 = _ci_call_len_5+2;
	_ci_call_table_width = 1 + (_ci_call_td_1+1)+(_ci_call_td_2+1)+\
		(_ci_call_td_3+1)+(_ci_call_td_4+1)+(_ci_call_td_5+1);
	_ci_call_table_left = (_terminal_width-2-_ci_call_table_width)>>1;
	_ci_call_table_right = _terminal_width-2-\
		_ci_call_table_width-_ci_call_table_left;
		
	_ci_hsms_len_1 = 2*strlen(_ci_hsms_str_1)/3;
	_ci_hsms_len_2 = 2*strlen(_ci_hsms_str_2)/3;
	_ci_hsms_len_3 = 2*strlen(_ci_hsms_str_3)/3;
	_ci_hsms_len_4 = 2*strlen(_ci_hsms_str_4)/3;
	_ci_hsms_td_1 = _ci_hsms_len_1+13; // 日期长度最短为19,要同标题长度一起考虑
	_ci_hsms_td_2 = _ci_hsms_len_2+2;
	_ci_hsms_td_3 = _ci_hsms_len_3+5;  // 手机号码最短为11，同样需要考虑标题长度
	_ci_hsms_td_4 = _ci_hsms_len_4+2;
	_ci_hsms_table_width = 1+(_ci_hsms_td_1+1)+\
		(_ci_hsms_td_2+1)+(_ci_hsms_td_3+1)+(_ci_hsms_td_4+1);
	_ci_hsms_table_left = (_terminal_width-2-_ci_hsms_table_width)>>1;
	_ci_hsms_table_right = _terminal_width-2-\
		_ci_hsms_table_width-_ci_hsms_table_left;

	_ci_info_len_1 = 2*strlen(_ci_info_str_1)/3;
	_ci_info_len_2 = 2*strlen(_ci_info_str_2)/3;
	_ci_info_len_3 = 2*strlen(_ci_info_str_3)/3;
	_ci_info_len_4 = 2*strlen(_ci_info_str_4)/3;
	_ci_info_table_width = 1+(_ci_info_td_1+1)+(_ci_info_td_2+1);
	_ci_info_table_left = (_terminal_width-2-_ci_info_table_width)>>1;
	_ci_info_table_right = _terminal_width-2-\
		_ci_info_table_width-_ci_info_table_left;
		
	_ci_hpay_len_1=2*strlen(_ci_hpay_str_1)/3;
	_ci_hpay_len_2=2*strlen(_ci_hpay_str_2)/3;
	_ci_hpay_len_3=2*strlen(_ci_hpay_str_3)/3;
	_ci_hpay_td_1 = _ci_hpay_len_1+2;
	_ci_hpay_td_2 = _ci_hpay_len_2+2;
	_ci_hpay_td_3 = _ci_hpay_len_3+4;
	_ci_hpay_table_width = 1+(_ci_hpay_td_1+1)+\
			(_ci_hpay_td_2+1)+(_ci_hpay_td_3+1);
	_ci_hapy_table_left = (_terminal_width-_ci_hpay_table_width-2)>>1;
	_ci_hapy_table_right = (_terminal_width-\
			_ci_hpay_table_width-2-_ci_hapy_table_left);
}

void _cmcc_interface_f(char *s,unsigned len)
{	 
	unsigned i;
	for(i=0;i<len;i++) printf("%s",s);
}

void _ci_field_latin(char *s,unsigned len)
{
	unsigned i,t=len-strlen(s);
	printf(" %s",s);
	for(i=0;i<t;i++) printf(" "); printf(" |");
}

void _ci_field_gbk(char *s,unsigned len)
{
	unsigned i,t=len-2*strlen(s)/3;
	printf(" %s",s);
	_cmcc_interface_f(" ",t); printf(" |");
}

void _ci_passwd_wrong(int second)
{
	system("clear");
	_cmcc_interface_header();
	printf("|"); _cmcc_interface_f(" ",_ci_pawd_table_left);
	printf("%s%d%s",_ci_pawd_str_1,second,"s");
	_cmcc_interface_f(" ",_ci_pawd_table_right);
	printf("|\n");
	_cmcc_interface_footer();
	sleep(1);
}

void _ci_system_busy(int second)
{
	system("clear");
	_cmcc_interface_header();
	printf("|"); _cmcc_interface_f(" ",_ci_stby_table_left);
	printf("%s%d%s",_ci_stby_str_1,second,"s");
	_cmcc_interface_f(" ",_ci_stby_table_right);
	printf("|\n");
	_cmcc_interface_footer();
	sleep(1);
}


/*---------------------------设备终端头部显示------------------------------------
 *
 +------------------------------------------------------------------------------+
 |                                                               /        China |
 |          欢迎使用中国移动自助终端服务系统                    /               |
 |                                                             /   Mobile-95568 |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_header_output(int c,char *s) // _cmcc_interface_header...
{
	int i;
	for(i=0;i<c;i++)
		printf("%s",s);
}

void _cmcc_interface_header()
{
	int i,temp;
	printf("+"); _ci_header_output(_terminal_width-2,"-");
	printf("+\n|"); _ci_header_output(_terminal_width-17," ");
	printf("/"); _ci_header_output(8," "); printf("China"); 
	printf(" |");
	printf("\n|"); _ci_header_output(10," "); 
	printf("%s",_ci_head_str_1); 
	_ci_header_output(_terminal_width-28-_ci_head_len_1," ");
		printf("/"); _ci_header_output(15," ");
		printf("|\n");printf("|"); 
	_ci_header_output(_terminal_width-19," ");
		printf("/"); printf("   Mobile-95568");
	_ci_header_output(_terminal_width-79," "); printf("|\n");
		printf("+"); 
	_ci_header_output(_terminal_width-2,"-"); printf("+\n");
}

void _cmcc_interface_footer()
{
	int i;
	printf("+");
	for(i=0;i<_terminal_width-2;i++)
		printf("-");
	printf("+\n");
}

/*------------------------------终端设备主界面-----------------------------------
 *
 +------------------------------------------------------------------------------+
 | +------------------------------------------------+ +-----------------------+ |
 | |                                                | |      (1)缴费充值      | |
 | +            中国移动自助服务终端设备            + +-----------------------+ |
 | +                 太原理工大学站                 + +-----------------------+ |
 | |                                                | |      (2)账户查询      | |
 | +------------------------------------------------+ +-----------------------+ |
 | +------------------------------------------------+ +-----------------------+ |
 | |                                                | |      (3)通话详单      | |
 | +                                                + +-----------------------+ |
 | +        欢迎使用中国移动自助终端服务系统        + +-----------------------+ |
 | |          在这里我们将为你提供方便快捷          | |      (4)短信详单      | |
 | +          的缴费，查询，打印等各种服务          + +-----------------------+ |
 | +                                                + +-----------------------+ |
 | |                                                | |      (1)缴费充值      | |
 | +------------------------------------------------+ +-----------------------+ |
 +------------------------------------------------------------------------------+
 *
 */
 
void _ci_main_output(char *f,char *p,char *s,int l,int k)
{
	int i,_pl_1,_pl_2;
	int rslen=2*(strlen(s)/3-1)+3; // right menu str length...
	if(k==1){
		_pl_1=(l-rslen-2*strlen(f))/2;
		_pl_2=l-rslen-2*strlen(f)-_pl_1;
	}else{
		_pl_1=(l-2*strlen(s)/3-2*strlen(f))/2;
		_pl_2=l-2*strlen(s)/3-2*strlen(f)-_pl_1;
	}
	printf("%s",f);
	for(i=0;i<_pl_1;i++) printf("%s",p);
		printf("%s",s);
	for(i=0;i<_pl_2;i++) printf("%s",p);
	printf("%s",f);
}

void _ci_main_opline(char *f,char *_p_1,
		char *_p_2,char *s_1,char *s_2,int k)
{
	unsigned int leftdiv=(_terminal_width-5)*2/3;	// 左右宽度比例为2:1
	unsigned int rightdiv=(_terminal_width-5)*1/3;
	
	printf("| ");
		_ci_main_output(f,_p_1,s_1,leftdiv,0);
		printf(" ");
		_ci_main_output(f,_p_2,s_2,rightdiv,k);
	printf(" |\n");
}

void _cmcc_interface_main(char *s)
{
	system("clear");
	_cmcc_interface_header();
	_ci_main_opline("+","-","-","","",0);
	_ci_main_opline("|"," "," ","",(char *)_ci_main_str_1,1);
	_ci_main_opline("+"," ","-",(char *)_ci_main_str_6,"",0);
	
	_ci_main_opline("+"," ","-",(char *)_ci_main_str_7,"",0);
	_ci_main_opline("|"," "," ","",(char *)_ci_main_str_2,1);
	_ci_main_opline("+","-","-","","",0);
	
	_ci_main_opline("+","-","-","","",0);
	_ci_main_opline("|"," "," ","",(char *)_ci_main_str_3,1);
	_ci_main_opline("+"," ","-","","",0);

	_ci_main_opline("+"," ","-",(char *)_ci_main_str_8,"",0);
	_ci_main_opline("|"," "," ",(char *)_ci_main_str_9,(char *)_ci_main_str_4,1);
	_ci_main_opline("+"," ","-",(char *)_ci_main_str_10,"",0);
	
	_ci_main_opline("+"," ","-","","",0);
	_ci_main_opline("|"," "," ","",(char *)_ci_main_str_5,1);
	_ci_main_opline("+","-","-","","",0);

	_cmcc_interface_footer();
	
	printf("%s",s);
}


/*
 *	缴费充值界面
 *
 */

void _ci_paym_latin(char *b,char *p,char *l,char *r)
{
	unsigned _sl_len,_sr_len;
	if(strlen(l)>1)
		_sl_len=2*strlen(l)/3;
	else
		_sl_len = 1;
	_sr_len=strlen(r);
	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_left); 
	printf("%s",b);
	_cmcc_interface_f(p,_ci_info_td_1-_sl_len-2); 
	printf("%s%s%s",l,p,p);
	printf("%s",b); 
	printf("%s%s%s",p,p,r); 
	_cmcc_interface_f(p,_ci_info_td_2-_sr_len-2);
	printf("%s",b); 
	_cmcc_interface_f(" ",_ci_paym_table_right); printf("|\n");
}

void _ci_paym_gbk(char *b,char *p,char *l,char *r)
{
	unsigned _sl_len=2*strlen(l)/3;
	unsigned _sr_len=2*strlen(r)/3;
	
	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_left); 
	printf("%s",b); _cmcc_interface_f(p,_ci_info_td_1-_sl_len-2); 
	printf("%s%s%s",l,p,p);
	printf("%s",b); 
	printf("%s%s%s",p,p,r); 
	_cmcc_interface_f(p,_ci_info_td_2-_sr_len-2);
	printf("%s",b); 
	_cmcc_interface_f(" ",_ci_paym_table_right); 
	printf("|\n");
}

/*------------------------------------充值步骤-----------------------------------
 *
 +------------------------------------------------------------------------------+
 |       +--+                                                                   |
 |       |步|    +------------+    +--------+    +--------+    +--------+       |
 |       +  +----|填写缴费信息|----|确认缴费|----|现金支付|->>>|缴费完成|       |
 |       |骤|    +------------+    +--------+    +--------+    +--------+       |
 |       +--+                                                                   |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_paym_step_menu(unsigned k)
{
	unsigned i;
	unsigned _p_left=(_terminal_width-66)>>1;	// padding-left
	unsigned _p_right=_terminal_width-66-_p_left;	// padding-right
	
	_cmcc_interface_header();printf("|");
	_cmcc_interface_f(" ",_p_left); printf("+--+"); 
	_cmcc_interface_f(" ",_terminal_width-6-_p_left);printf("|\n|");
	_cmcc_interface_f(" ",_p_left); printf("|步|    +"); 
	_cmcc_interface_f("-",12);
	for(i=0;i<3;i++){
		printf("+    +"); 
		_cmcc_interface_f("-",8);
	}
	printf("+");
	_cmcc_interface_f(" ",_p_right); printf("|\n|");
	_cmcc_interface_f(" ",_p_left); printf("+  +");

	if(k==1) printf("->>>");else printf("----"); printf("|%s|",_ci_paym_str_1);
	if(k==2) printf("->>>");else printf("----"); printf("|%s|",_ci_paym_str_2);
	if(k==3) printf("->>>");else printf("----"); printf("|%s|",_ci_paym_str_3);
	if(k==4) printf("->>>");else printf("----"); printf("|%s|",_ci_paym_str_4);

	_cmcc_interface_f(" ",_p_right); printf("|\n|");

	_cmcc_interface_f(" ",_p_left); printf("|骤|    +"); 
	_cmcc_interface_f("-",12);
	for(i=0;i<3;i++){ 
		printf("+    +"); 
		_cmcc_interface_f("-",8);
	}
	printf("+");
	_cmcc_interface_f(" ",_p_right); printf("|\n|");
	_cmcc_interface_f(" ",_p_left); printf("+--+"); 
	_cmcc_interface_f(" ",_terminal_width-6-_p_left);printf("|\n");
	_cmcc_interface_footer();
	
}

/*------------------------------------充值菜单一---------------------------------
 *
 +------------------------------------------------------------------------------+
 |          温馨提示：欢迎您使用自助缴费业务，适用于全国所有非智能网用户        |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_paym_step_1()
{
	system("clear");
	_ci_paym_step_menu(1);
	_cmcc_interface_footer(); printf("|");
	_cmcc_interface_f(" ",_ci_paym_table_left); printf("温馨提示：");
	printf("欢迎您使用自助缴费业务，适用于全国所有非智能网用户");
	_cmcc_interface_f(" ",_terminal_width-2-_ci_paym_table_left-60);
	printf("|\n");
	_cmcc_interface_footer();
}

/*------------------------------------充值菜单二---------------------------------
 *
 +------------------------------------------------------------------------------+
 |         ------------------------------------------------------------         |
 |         |                       充值信息确认                       |         |
 |         ------------------------------------------------------------         |
 |         |           用户姓名|李启明                                |         |
 |         +-------------------+--------------------------------------+         |
 |         |           缴费号码|15536888714                           |         |
 |         +-------------------+--------------------------------------+         |
 |         |           成交金额|34                                    |         |
 |         +-------------------+--------------------------------------+         |
 +------------------------------------------------------------------------------+
 *
 */
 
void _ci_paym_step_2(char *name,char *pnumber,char *balance)
{
	unsigned i;
	system("clear");
	_ci_paym_step_menu(2);
	_ci_paym_latin("-","-","-","-");
	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_left); 
	printf("|"); _cmcc_interface_f(" ",_ci_pst2_th_left); 
	printf("%s", _ci_pst2_str_1); _cmcc_interface_f(" ",_ci_pst2_th_right); 
	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_right); printf("|\n");

	_ci_paym_latin("-","-","-","-");
	_ci_paym_gbk("|"," ",(char *)_ci_pst2_str_2,name);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst2_str_3,pnumber);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst2_str_4,balance);
	_ci_paym_latin("+","-","-","-");

	_cmcc_interface_footer();
}

void _ci_paym_step_3()
{	
	system("clear");
	_ci_paym_step_menu(3);
	printf("|"); _cmcc_interface_f(" ",_ci_pst3_table_left);
		printf("%s",_ci_pst3_str_1);
	_cmcc_interface_f(" ",_ci_pst3_table_right); printf("|\n");
	_cmcc_interface_footer();
}

/*------------------------------------充值菜单四---------------------------------
 *
 +------------------------------------------------------------------------------+
 |         ------------------------------------------------------------         |
 |         |                       缴费结果确认                       |         |
 |         ------------------------------------------------------------         |
 |         |           缴费号码|15536888714                           |         |
 |         +-------------------+--------------------------------------+         |
 |         |           缴费时间|2011-06-05                            |         |
 |         +-------------------+--------------------------------------+         |
 |         |           成交金额|33                                    |         |
 |         +-------------------+--------------------------------------+         |
 |         |           流水线号|342222222222220                       |         |
 |         +-------------------+--------------------------------------+         |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_paym_step_4(char *pnumber,char *sum,char *s_number)
{
	unsigned i;

	char _p_time[11];
	time_t timer;
	timer = time(NULL);
	struct tm *_p_t = localtime(&timer);
	strftime(_p_time,11,"%F",_p_t);

	
	_ci_paym_step_menu(4);
	_ci_paym_latin("-","-","-","-");

	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_left); 
	printf("|"); _cmcc_interface_f(" ",_ci_pst4_th_left); 
	printf("%s", _ci_pst4_str_1); _cmcc_interface_f(" ",_ci_pst4_th_right); 
	printf("|"); _cmcc_interface_f(" ",_ci_paym_table_left); printf("|\n");

	_ci_paym_latin("-","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst4_str_2,pnumber);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst4_str_3,_p_time);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst4_str_4,sum);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_pst4_str_5,s_number);
	_ci_paym_latin("+","-","-","-");

	_cmcc_interface_footer();
}


void _cmcc_interface_paym()  		// test case...
{
	char pnumber[2][11]={"",""}; 
	_ci_paym_step_4("15536888714","33","342222222222220");
	printf("请输入手机号码："); scanf("%s",pnumber[0]);
	printf("确认您的手机号码："); scanf("%s",pnumber[1]);
	if(strcmp(pnumber[0],pnumber[1])==0)
		printf("输入号码正确\n");
}

/*--------------------------------账户信息查询-----------------------------------
 *
 +------------------------------------------------------------------------------+
 |         +-------------------+--------------------------------------+         |
 |         |           客户姓名|方栋                                  |         |
 |         +-------------------+--------------------------------------+         |
 |         |           手机号码|15536888714                           |         |
 |         +-------------------+--------------------------------------+         |
 |         |           账户余额|88.2                                  |         |
 |         +-------------------+--------------------------------------+         |
 |         |         手机归属地|广东深圳                              |         |
 |         +-------------------+--------------------------------------+         |
 +------------------------------------------------------------------------------+
 *
 *-----------------------------------------------------------------------------*/
 
void _cmcc_interface_info(char *n,char *p,char *b,char *r)
{
	/*
	 * n -> name ; p -> phone number ;
	 * b -> balance ; r -> attribute ;
	 */
	
	system("clear");
	_cmcc_interface_header();

	_ci_paym_latin("+","-","-","-");
	_ci_paym_gbk("|"," ",(char *)_ci_info_str_1,n);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_info_str_2,p);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_latin("|"," ",(char *)_ci_info_str_3,b);
	_ci_paym_latin("+","-","-","-");
	_ci_paym_gbk("|"," ",(char *)_ci_info_str_4,r);
	_ci_paym_latin("+","-","-","-");
	
	_cmcc_interface_footer();
}



/*----------------------------------通话记录查询---------------------------------
 *
 +------------------------------------------------------------------------------+
 |        +---------------------+------+----------+-------------+------+        |
 |        |            通话时间 | 时长 | 呼叫类型 |    对方号码 | 费用 |        |
 |        +---------------------+------+----------+-------------+------+        |
 |        | 2011-04-02 18:31:17 | 19   | 1        | 15536888714 | 3    |        |
 |        +---------------------+------+----------+-------------+------+        |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_call_row(char *s_1,char *s_2,char *s_3,char *s_4,char *s_5)
{
	printf("|"); 
	_cmcc_interface_f(" ",_ci_call_table_left); printf("|");
	_ci_field_latin(s_1,_ci_call_td_1-2);
	_ci_field_latin(s_2,_ci_call_td_2-2);
	_ci_field_gbk(s_3,_ci_call_td_3-2);
	_ci_field_latin(s_4,_ci_call_td_4-2);
	_ci_field_latin(s_5,_ci_call_td_5-2); 
	_cmcc_interface_f(" ",_ci_call_table_right); printf("|\n");
}

void _ci_call_border()
{
	printf("|"); _cmcc_interface_f(" ",_ci_call_table_left);
	printf("+"); _cmcc_interface_f("-",_ci_call_td_1); 
	printf("+"); _cmcc_interface_f("-",_ci_call_td_2); 
	printf("+"); _cmcc_interface_f("-",_ci_call_td_3);
	printf("+"); _cmcc_interface_f("-",_ci_call_td_4);
	printf("+"); _cmcc_interface_f("-",_ci_call_td_5);
	printf("+"); _cmcc_interface_f(" ",_ci_call_table_right); 
	printf("|\n");
}

void _ci_call_header()
{
	system("clear");
	_cmcc_interface_header();
	_ci_call_border();
	printf("|"); _cmcc_interface_f(" ",_ci_call_table_left);
	printf("| %s",_ci_call_str_1); 
	_cmcc_interface_f(" ",_ci_call_td_1-_ci_call_len_1-1);
	printf("| %s",_ci_call_str_2); 
	_cmcc_interface_f(" ",_ci_call_td_2-_ci_call_len_2-1);
	printf("| %s",_ci_call_str_3); 
	_cmcc_interface_f(" ",_ci_call_td_3-_ci_call_len_3-1);
	printf("| %s",_ci_call_str_4); 
	_cmcc_interface_f(" ",_ci_call_td_4-_ci_call_len_4-1);
	printf("| %s",_ci_call_str_5); 
	_cmcc_interface_f(" ",_ci_call_td_5-_ci_call_len_5-1);
	printf("|"); 
	_cmcc_interface_f(" ",_ci_call_table_right); printf("|\n");
	_ci_call_border();
}

void _cmcc_interface_call()		// test case...
{
	system("clear");
	_cmcc_interface_header();
	_ci_call_header();
	_ci_call_row("2011-04-02 18:31:17","19","主叫","15536888714","3");
	_ci_call_border();
	_cmcc_interface_footer();
}

/*--------------------------------短信记录查询-----------------------------------
 *
 +------------------------------------------------------------------------------+
 |           +---------------------+----------+-------------+------+            |
 |           |            发送时间 | 传送方式 |    对方号码 | 费用 |            |
 |           | 2011-04-02 18:31:17 | 1        | 15536888714 | 3    |            |
 |           +---------------------+----------+-------------+------+            |
 +------------------------------------------------------------------------------+
 *
 *-----------------------------------------------------------------------------*/

void _ci_hsms_row(char *s_1,char *s_2,char *s_3,char *s_4)
{
	printf("|"); _cmcc_interface_f(" ",_ci_hsms_table_left); printf("|"); 
	_ci_field_latin(s_1,_ci_hsms_td_1-2); _ci_field_gbk(s_2,_ci_hsms_td_2-2);
	_ci_field_latin(s_3,_ci_hsms_td_3-2); _ci_field_latin(s_4,_ci_hsms_td_4-2);
	_cmcc_interface_f(" ",_ci_hsms_table_right); printf("|\n");
}

void _ci_hsms_border()
{
	printf("|"); _cmcc_interface_f(" ",_ci_hsms_table_left);
	printf("+"); _cmcc_interface_f("-",_ci_hsms_td_1); 
	printf("+"); _cmcc_interface_f("-",_ci_hsms_td_2);
	printf("+"); _cmcc_interface_f("-",_ci_hsms_td_3);
	printf("+"); _cmcc_interface_f("-",_ci_hsms_td_4);
	printf("+");
	_cmcc_interface_f(" ",_ci_hsms_table_right); printf("|\n");
}


void _ci_hsms_header()
{
	system("clear");
	_cmcc_interface_header();
	_ci_hsms_border();
	printf("|"); 
	_cmcc_interface_f(" ",_ci_hsms_table_left);
	printf("| %s",_ci_hsms_str_1); 
	_cmcc_interface_f(" ",_ci_hsms_td_1-_ci_hsms_len_1-1);
	printf("| %s",_ci_hsms_str_2); 
	_cmcc_interface_f(" ",_ci_hsms_td_2-_ci_hsms_len_2-1);
	printf("| %s",_ci_hsms_str_3); 
	_cmcc_interface_f(" ",_ci_hsms_td_3-_ci_hsms_len_3-1);
	printf("| %s",_ci_hsms_str_4); 
	_cmcc_interface_f(" ",_ci_hsms_td_4-_ci_hsms_len_4-1);
	printf("|");
	_cmcc_interface_f(" ",_ci_hsms_table_right); printf("|\n");
	_ci_hsms_border();
}


//void _ci_hsms_table


void _cmcc_interface_hsms()		// test case...
{
	system("clear");
	_cmcc_interface_header();
	_ci_hsms_header();
	_ci_hsms_row("2011-04-02 18:31:17","发送","15536888714","3");
	_ci_hsms_border();
	_cmcc_interface_footer();
}

/*---------------------------------历史充值记录----------------------------------
 *
 +------------------------------------------------------------------------------+
 |                     +----------+----------+------------+                     |
 |                     | 缴费方式 | 缴费金额 |   缴费时间 |                     |
 |                     +----------+----------+------------+                     |
 |                     | 充值     | 50       | 2011-01-02 |                     |
 |                     +----------+----------+------------+                     |
 +------------------------------------------------------------------------------+
 *
 */

void _ci_hpay_row(char *s_1,char *s_2,char *s_3)
{
	printf("|"); _cmcc_interface_f(" ",_ci_hapy_table_left); printf("|");
	_ci_field_gbk(s_1,_ci_hpay_td_1-2);
	_ci_field_latin(s_2,_ci_hpay_td_2-2);
	_ci_field_latin(s_3,_ci_hpay_td_3-2);
	_cmcc_interface_f(" ",_ci_hapy_table_right); printf("|\n");
	_ci_hpay_border();
}

void _ci_hpay_border()
{
	printf("|"); _cmcc_interface_f(" ",_ci_hapy_table_left);
	printf("+"); _cmcc_interface_f("-",_ci_hpay_td_1);
	printf("+"); _cmcc_interface_f("-",_ci_hpay_td_2);
	printf("+"); _cmcc_interface_f("-",_ci_hpay_td_3);
	printf("+"); _cmcc_interface_f(" ",_ci_hapy_table_right); printf("|\n");
}


void _ci_hpay_header()
{	
	system("clear");
	_cmcc_interface_header();
	_ci_hpay_border();
	printf("|"); 
	_cmcc_interface_f(" ",_ci_hapy_table_left); 
	printf("| %s",_ci_hpay_str_1); 
	_cmcc_interface_f(" ",_ci_hpay_td_1-_ci_hpay_len_1-1);
	printf("| %s",_ci_hpay_str_2); 
	_cmcc_interface_f(" ",_ci_hpay_td_2-_ci_hpay_len_2-1);
	printf("| %s",_ci_hpay_str_3); 
	_cmcc_interface_f(" ",_ci_hpay_td_3-_ci_hpay_len_3-1);
	printf("|");
	_cmcc_interface_f(" ",_ci_hapy_table_right); printf("|\n");
	_ci_hpay_border();
}

void _cmcc_interface_hpay()		// test case...
{
	system("clear");
	_ci_hpay_header();
	_ci_hpay_row("充值","50","2011-01-02");
	_cmcc_interface_footer();
}
