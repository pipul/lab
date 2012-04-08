#include <stdio.h>
#include <string.h>
#include "_cmcc_interface.h"
#include "_cmcc_function.h"

int main(void)
{
	int ftnumber;		// functino number...
	
	while(1)
	{
		_cmcc_function_init();
		_cmcc_interface_init();
		_cmcc_interface_main("请输入服务编号：");
		scanf("%d",&ftnumber);
		if(ftnumber==13221)	// 关机密码
			break;
		while(ftnumber>5 || ftnumber<1){
			_cmcc_interface_main("非法输入，请重新输入：");
			scanf("%d",&ftnumber);
		}
		if(ftnumber==1)
			_cmcc_function_paym();
		else if(ftnumber==2)
			_cmcc_function_info();
		else if(ftnumber==3)
			_cmcc_function_call();
		else if(ftnumber==4)
			_cmcc_function_hsms();
		else if(ftnumber==5)
			_cmcc_function_hpay();
	}
	return(0);
}
