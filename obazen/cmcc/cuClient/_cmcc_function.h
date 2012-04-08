#ifndef	_CMCC_FUNCTION_H_INCLUDE
#define	_CMCC_FUNCTION_H_INCLUDE

int _cmcc_function_init(void );

int _cmcc_function_paym(void );
int _cmcc_function_info(void );
int _cmcc_function_call(void );
int _cmcc_function_hsms(void );
int _cmcc_function_hpay(void );

int _cmcc_function_identify(char *,char *,char *);
int _cmcc_function_sqlCom(char *);

#endif	/* _CMCC_FUNCTION_H_INCLUDE */
