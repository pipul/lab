#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXSIZE 1000
#define databaseName "cmcc2"
#define field_width 15

void getComm(void );
void proComm(void );

MYSQL mysql,*Conn;
MYSQL_FIELD *fields;
MYSQL_RES *res;
MYSQL_ROW row;
char *sqlCom;

int main()
{
        char usname[20]={"root"},passwd[20]={"136136"};
        mysql_init(&mysql);

        do{
                /*printf("Enter UserName : ");
                scanf("%s",usname);
                printf("Enter PassWord : ");
                scanf("%s",passwd);*/

                if(!(Conn=mysql_real_connect(&mysql,"localhost",
                                usname,passwd,databaseName,0,NULL,0)))
                {
                        fprintf(stderr,"%s\n",mysql_error(&mysql));
                }
        }while(!Conn);

        system("clear");
	mysql_set_character_set(Conn,"utf8");
        while(1)
        {
                getComm();
                proComm();
                printf("\n");
        }
        return(0);
}

void field_output(char *border,char *str,unsigned token)
{
        int i;
        printf("%s",border);

        if(token==1){
                for(i=0;i<field_width;i++)
                        printf("-");
        }else{
                printf(" ");
                for(i=0;(i<field_width-strlen(str)-2)&&(i>=0);i++){
                        printf(" ");
                }
                printf("%s ",str);
        }

}

void getComm()
{
        char token[4]="";
        char ch,buffer[MAXSIZE]="";
        int i=0;
        free(sqlCom);
        printf("mysql>");
        ch=getchar();
        while (ch!=';')
        {
                buffer[i++]=ch;
                ch=getchar();
        }
        sqlCom=(char *)malloc(strlen(buffer));
        strcpy(sqlCom,buffer);

        for(i=0;i<sizeof(sqlCom);i++)
                 sqlCom[i] = tolower(sqlCom[i]);

        sscanf(sqlCom,"%4s",token);
        if(!strcmp(token,"exit")||!strcmp(token,"quit"))
        {
                mysql_close(Conn);
                exit(0);
        }
}

void proComm()
{
        char token[6],buffer[MAXSIZE];
        sscanf(sqlCom,"%6s",token);
        int key=mysql_query(Conn,sqlCom),i;
        if(key==0){
                if(!strcmp(token,"select")){

                        if(!(res=mysql_store_result(Conn))){
                                fprintf(stderr,"%s\n",mysql_error(Conn));
                        }else{
                                fields = mysql_fetch_fields(res);

                                for(i=0;i<mysql_num_fields(res);i++){
                                        field_output("+",NULL,1);

                                }
                                printf("+\n");
                                for(i=0;i<mysql_num_fields(res);i++)
                                {
                                        field_output("|",fields[i].name,0);
                                }
                                printf("|\n");
                                for(i=0;i<mysql_num_fields(res);i++){
                                        field_output("+",NULL,1);

                                }
                                printf("+\n");

                                while((row=mysql_fetch_row(res))!=NULL){
                                        for(i=0;i<mysql_num_fields(res);i++){
                                        //sprintf(buffer,"%s",row[i]);
                                                field_output("|",row[i],0);
                                        }
                                        printf("|\n");
                                }
                                for(i=0;i<mysql_num_fields(res);i++){
                                        field_output("+",NULL,1);
                                }
                                printf("|\n");
                        }
                        mysql_free_result(res);
                }else if(!strcmp(token,"insert")){
                        printf("Insert OK,Datas affected!\n");
                }else if(!strcmp(token,"update")){
                        printf("Row Matched OK,Good luck!\n");
                }else{
                        printf("Delete Rows Successfully!\n");
                }
        }else{
                fprintf(stderr,"%s\n",mysql_error(Conn));
                printf("\n");
        }
} 
