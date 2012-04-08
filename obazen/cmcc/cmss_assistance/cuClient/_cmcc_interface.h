#ifndef	_CMCC_INTERFACE_H_INCLUDE
#define	_CMCC_INTERFACE_H_INCLUDE

#define _terminal_width 80

static const char _ci_head_str_1[]={"欢迎使用中国移动自助终端服务系统"};
static const char _ci_pawd_str_1[]={"您输入的密码有误<<====>>系统正在返回："};
static const char _ci_stby_str_1[]={"系统繁忙出现异常<<====>>系统正在返回："};


static const char _ci_main_str_1[]={"(1)缴费充值"};
static const char _ci_main_str_2[]={"(2)账户查询"};
static const char _ci_main_str_3[]={"(3)通话详单"};
static const char _ci_main_str_4[]={"(4)短信详单"};
static const char _ci_main_str_5[]={"(5)充值历史"};

static const char _ci_main_str_6[]={"联通中国移动自助服务终端设备"};
static const char _ci_main_str_7[]={"联通太原理工大学站"};

static const char _ci_main_str_8[]={"欢迎使用中国移动自助终端服务系统"};
static const char _ci_main_str_9[]={"在这里我们将为你提供方便快捷"};
static const char _ci_main_str_10[]={"的缴费，查询，打印等各种服务"};

static const char _ci_paym_str_1[]={"填写缴费信息"}; 
static const char _ci_paym_str_2[]={"确认缴费"};
static const char _ci_paym_str_3[]={"现金支付"};
static const char _ci_paym_str_4[]={"缴费完成"};

static const char _ci_pst2_str_1[]={"充值信息确认"};
static const char _ci_pst2_str_2[]={"用户姓名"};
static const char _ci_pst2_str_3[]={"缴费号码"};
static const char _ci_pst2_str_4[]={"账户余额"};

static const char _ci_pst3_str_1[]={"请将钞票放入接收器"};

static const char _ci_pst4_str_1[]={"缴费结果确认"};
static const char _ci_pst4_str_2[]={"缴费号码"};
static const char _ci_pst4_str_3[]={"缴费时间"};
static const char _ci_pst4_str_4[]={"成交金额"};
static const char _ci_pst4_str_5[]={"流水线号"};

static const char _ci_info_str_1[]={"客户姓名"};
static const char _ci_info_str_2[]={"手机号码"};
static const char _ci_info_str_3[]={"账户余额"};
static const char _ci_info_str_4[]={"手机归属地"};

static const char _ci_call_str_1[]={"通话时间"};
static const char _ci_call_str_2[]={"时长"};
static const char _ci_call_str_3[]={"呼叫类型"};
static const char _ci_call_str_4[]={"对方号码"};
static const char _ci_call_str_5[]={"费用"};

static const char _ci_hsms_str_1[]={"发送时间"};
static const char _ci_hsms_str_2[]={"传送方式"};
static const char _ci_hsms_str_3[]={"对方号码"};
static const char _ci_hsms_str_4[]={"费用"};

static const char _ci_hpay_str_1[]={"缴费方式"};
static const char _ci_hpay_str_2[]={"缴费金额"};
static const char _ci_hpay_str_3[]={"缴费时间"};

static unsigned _ci_head_len_1;

static unsigned _ci_pawd_len_1=38,_ci_pawd_table_left,_ci_pawd_table_right;
static unsigned _ci_stby_len_1=38,_ci_stby_table_left,_ci_stby_table_right;

static unsigned _ci_pst2_len_1,_ci_pst2_len_2,_ci_pst2_len_3,_ci_pst2_len_4;
static unsigned _ci_pst2_th_left,_ci_pst2_th_right;

static unsigned _ci_pst3_len_1,_ci_pst3_table_left,_ci_pst3_table_right;

static unsigned _ci_paym_table_width,_ci_paym_table_left,_ci_paym_table_right;

static unsigned _ci_pst4_th_left,_ci_pst4_th_right,_ci_pst4_len_1;

static unsigned _ci_info_td_1=20,_ci_info_td_2=41;
static unsigned _ci_info_len_1,_ci_info_len_2,_ci_info_len_3,_ci_info_len_4;
static unsigned _ci_info_table_width,_ci_info_table_left,_ci_info_table_right;

static unsigned _ci_call_len_1,_ci_call_len_2,\
			_ci_call_len_3,_ci_call_len_4,_ci_call_len_5;
static unsigned _ci_call_td_1,_ci_call_td_2,\
			_ci_call_td_3,_ci_call_td_4,_ci_call_td_5;
static unsigned _ci_call_table_width,_ci_call_table_left,_ci_call_table_right;

static unsigned _ci_hsms_len_1,_ci_hsms_len_2,_ci_hsms_len_3,_ci_hsms_len_4;
static unsigned _ci_hsms_td_1,_ci_hsms_td_2,_ci_hsms_td_3,_ci_hsms_td_4;
static unsigned _ci_hsms_table_width,_ci_hsms_table_left,_ci_hsms_table_right;

static unsigned _ci_hpay_len_1,_ci_hpay_len_2,_ci_hpay_len_3;
static unsigned _ci_hpay_td_1,_ci_hpay_td_2,_ci_hpay_td_3;
static unsigned _ci_hpay_table_width,_ci_hapy_table_left,_ci_hapy_table_right;


void _cmcc_interface_init(void );
void _ci_passwd_wrong(int );
void _ci_system_busy(int );

void _cmcc_interface_paym(void );  // payment..
void _cmcc_interface_info(char *,char *,char *,char * );  // infomation..
void _cmcc_interface_call(void );  // call history..
void _cmcc_interface_hsms(void );  // sms history..
void _cmcc_interface_hpay(void );  // payment history..

void _ci_header_output(int ,char *);
void _cmcc_interface_header(void );
void _cmcc_interface_footer(void );
void _ci_main_output(char *,char *,char *,int ,int );
void _ci_main_opline(char *,char *,char *,char *,char *,int );
void _cmcc_interface_main(char *);

void _cmcc_interface_f(char *,unsigned );
void _ci_paym_latin(char *,char *,char *,char *);
void _ci_paym_gbk(char *,char *,char *,char *);
void _ci_paym_step_1(void );
void _ci_paym_step_2(char *,char *,char *);
void _ci_paym_step_3(void );
void _ci_paym_step_4(char *,char *,char *);
void _ci_paym_step_menu(unsigned );

void _ci_field_latin(char *,unsigned);
void _ci_field_gbk(char *,unsigned);
void _ci_call_row(char *,char *,char *,char *,char *);
void _ci_call_header();
void _ci_call_border();

void _ci_hsms_row(char *,char *,char *,char *);
void _ci_hsms_border(void );
void _ci_hsms_header(void );

void _ci_hpay_row(char *,char *,char *);
void _ci_hpay_border(void );
void _ci_hpay_header(void );

#endif	/* _CMCC_INTERFACE_H_INCLUDE */
