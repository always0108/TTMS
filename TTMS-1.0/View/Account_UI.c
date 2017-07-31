#include "Account_UI.h"
#include "../Common/list.h"
#include "../Service/Account.h"
#include "../Service/EntityKey.h"
#include "Account_UI.h"
//#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>  //linux用sys/time.h
#include "conio.h"
#include <unistd.h>
extern account_t gl_CurUser;
char *usr_type(int i)
{
    switch(i)
    {
        case 1:
            return "cherk";
            break;
            
        case 2:
            return "manager";
            break;
        
        case 9:
            return "administrator";
            break;
    
        case 0:
            return "others";
            break;
     
    }
}
//输入密码的加密
//用法：将需要被赋值的字符数组作为参数传入函数
//函数中未对字符数组长度做限制
//返回值：输入的字符数量（不包括'\0'）
int GetPassword(char password[])
{
	char ch ;
	int i = 0;
    do
    {
        ch = getch();
        if(ch != '\n' && ch != '\r' && ch!=127)
        {
            password[i]=ch;
            printf("*");
            i++;
        }
        else if(((ch!='\n')|(ch!='\r'))&&(ch==127))
        {
            if(i>0)
            {
                i--;
                printf("\b \b");
            }
        }
    }while(ch!='\n'&&ch!='\r');
    password[i]='\0';
	return i;
}

//登录函数，提示用户输入用户名和密码，登录成功return 1，否则提示重新登录，超过3次，登录失败
int SysLogin() {
	Account_Srv_InitSys();
	char usrname[20];
	char passwd[20];
	int i=0,j;
	printf("\t\t\t==================================================================\n");
	printf("\t\t\t    ****************    欢迎来到票务管理系统   *****************\n");
	printf("\t\t\t      **************           请登录          *************\n");
	printf("\t\t\t==================================================================\n");
	for (i = 0; i < 3; i++) {
		printf("\t\t\t******** 请输入你的用户名: ");
		gets(usrname);
		printf("\t\t\t******** 请输入你的密码: ");
		GetPassword(passwd);
		printf("\n\t\t\t====================================\n");
		if (Account_Srv_Verify(usrname, passwd))
		{
			printf("\t\t\t");
			for (j = 0; j < 12; j++)
			{
				usleep(200000);
				printf("=> ");
			}
			usleep(300000);
			printf("\n");
			printf("\t\t\t欢迎");
			return 1;
		}
		printf("\t\t\t");
		for (j = 0; j < 12; j++)
		{
			usleep(200000);
			printf("=> ");
		}
		usleep(300000);
		printf("\n");
		printf("\t\t\t身份验证错误!!!\n");
	}
	return 0;
}
//管理系统用户功能入口函数，显示用户账号管理菜单
void Account_UI_MgtEntry() {
	int i;
	char choice;
	account_t usr;

	account_list_t head;
	account_node_t *pos;
	Pagination_t paging;

	List_Init(head, account_node_t);
	paging.offset = 0;
	paging.pageSize = ACCOUNT_PAGE_SIZE;

	//载入数据
	paging.totalRecords = Account_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head, paging);
	if (gl_CurUser.type == 9)
	{
		do {
			system("clear");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t********************* 用户管理界面  *********************\n");
			printf("\t\t\t%-5s  %-16s  %-12s  %-12s   \n", "ID", "type", "Name", "password");
			printf("\t\t\t------------------------------------------------------------------\n");
			//显示数据
			for (i = 0, pos = (account_node_t *)(paging.curPos);
				pos != head && i < paging.pageSize; i++) {
				printf("\t\t\t%-5d  %-16s  %-12s  %-12s  \n",pos->data.id,usr_type(pos->data.type), pos->data.username,"********");
				pos = pos->next;
			}
			printf("\t\t\t------- Total Records:%2d ----------------------- Page %2d/%2d ----\n",
				paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t******************************************************************\n");
			printf("\t\t\t[P]上一页|[N]下一页|[A]添加|[D]删除|[M]修改|[Q]查询|[R]返回");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t你的选择:");
			fflush(stdin);
			choice = getche();
			//getchar();
			fflush(stdin);
			switch (choice) {
			case 'a':
			case 'A':
				if (Account_UI_Add(head)) //新添加成功，跳到最后一页显示
				{
					paging.totalRecords = Account_Srv_FetchAll(head);
					Paging_Locate_LastPage(head, paging, account_node_t);
				}
				break;
			case 'd':
			case 'D':
				//getchar();
				printf("\n\t\t\t输入用户名:");
				gets(usr.username);
				if (Account_UI_Delete(head, usr.username)) {	//从新载入数据
					paging.totalRecords = Account_Srv_FetchAll(head);
					List_Paging(head, paging, account_node_t);
				}
				break;
			case 'm':
			case 'M':
				//getchar();
				printf("\n\t\t\t输入用户名:");
				gets(usr.username);
				if (Account_UI_Modify(head, usr.username)) {	//从新载入数据
					paging.totalRecords = Account_Srv_FetchAll(head);
					List_Paging(head, paging, account_node_t);
				}
				break;
			case 'q':
			case 'Q':
				//getchar();
				printf("\n\t\t\t输入用户名:");
				gets(usr.username);
				if (Account_UI_Query(head, usr.username)) {
					//从新载入数据
					paging.totalRecords = Account_Srv_FetchAll(head);
					List_Paging(head, paging, account_node_t);
				}
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, -1, account_node_t);
				}
				break;
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, 1, account_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
	else
	{
		do {
			system("clear");
			//显示数据
			printf("\n\t\t\t******************************************************************\n");
			printf("\t\t\t      **************   修改自己的密码  **************\n");
			printf("\t\t\t******************************************************************\n");
			printf("\t\t\t[M]修改\t\t[R]返回");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t你的选择:");
			fflush(stdin);
			choice = getche();
			//getchar();
			fflush(stdin);
			switch (choice)
			{
			case 'm':
			case 'M':
				//getchar();
				strcpy(usr.username , gl_CurUser.username);
				if (Account_UI_Modify(head, usr.username)) {	//从新载入数据
					paging.totalRecords = Account_Srv_FetchAll(head);
					List_Paging(head, paging, account_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');

	}
	//getchar();
	//释放链表空间
	List_Destroy(head, account_node_t);
}




//添加一个用户账号信息，如果账号名存在，提示出错信息
int Account_UI_Add(account_list_t list) {
	account_t usr;
	int newUsrCount = 0;
	char choice;
	char passwd[20];
	int l;
    List_Init(list,account_node_t);
    Account_Srv_FetchAll(list);
	do
	{
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  添加一个新用户  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\t请输入新的用户名:");
	gets(usr.username);
	printf("\t\t\t请输入新的密码:");
	GetPassword(usr.password);
	l = strlen(usr.password);
	while (l < 6)
	{
		printf("\n\t\t\t密码不足6位,请重新输入!\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
	}
	printf("\n\t\t\t请再次输入上面的密码:");
	GetPassword(passwd);
	while (strcmp(usr.password, passwd) != 0)
	{
		printf("\n\t\t\t两次密码不一致，请重新输入！\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
		while (l < 6)
		{
			printf("\n\t\t\t密码不足6位,请重新输入!\n");
			printf("\n\t\t\t请输入新的密码:");
			GetPassword(usr.password);
			l = strlen(usr.password);
		}
		printf("\n\t\t\t请再次输入上面的密码:");
		GetPassword(passwd);
	}
	printf("\n\t\t\t请输入用户类型:");
	scanf("%d",&usr.type);
	getchar();
	if (NULL!=Account_Srv_FindByUsrName(list, usr.username))
	{
		printf("\t\t\t该用户已经存在!!!\n");
        printf("\t\t\t按 [Enter] 来退出!!!");
        getchar();
		return 0;
	}
	else {
		//获取主键
		usr.id = EntKey_Srv_CompNewKey("Account");
		if (Account_Srv_Add(&usr))
		{
			newUsrCount += 1;
			printf("\t\t\t这个新用户添加成功!\n");
		}
		else
			printf("\t\t\t这个新用户添加失败!\n");
	    }
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\t[A]继续添加, [R]返回:");
		choice = getche();
		//getchar();
	} while ('a' == choice || 'A' == choice);
	return newUsrCount;
}
	
//根据用户账号名修改用户账号密码，不存在这个用户账号名，提示出错信息
int Account_UI_Modify(account_list_t list ,char usrName[]) 
{
	account_t usr;
	int rtn = 0;
	int l;
	char passwd[20];
	if (!Account_UI_Query(list, usrName))
		return 0;
	Account_Srv_FetchByName(usrName, &usr);
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  修改用户信息  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");

	printf("\t\t\t请输入新的密码:");
	GetPassword(usr.password);
	l = strlen(usr.password);
	while (l < 6)
	{
		printf("\n\t\t\t密码不足6位,请重新输入!\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
	}
	printf("\n\t\t\t请再次输入上面的密码:");
	GetPassword(passwd);
	while (strcmp(usr.password, passwd) != 0)
	{
		printf("\n\t\t\t两次密码不一致，请重新输入！\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
		while (l < 6)
		{
			printf("\n\t\t\t密码不足6位,请重新输入!\n");
			printf("\n\t\t\t请输入新的密码:");
			GetPassword(usr.password);
			l = strlen(usr.password);
		}
		printf("\n\t\t\t请再次输入上面的密码:");
		GetPassword(passwd);
	}
	if (Account_Srv_Modify(&usr)) {
		rtn = 1;
		printf("\t\t\t这个用户信息更新成功!\n\t\t\t按 [Enter] 来返回!\n\t\t\t");
	}
	else
		printf("\t\t\t这个用户信息更新失败!\n\t\t\t按 [Enter] 来返回!\n\t\t\t");
	getchar();
	return rtn;
}

//根据用户账号名删除一个已经存在的用户账号信息，如果不存在这个用户账号名，提示出错信息
int Account_UI_Delete(account_list_t list ,char usrName[]) {
	account_t usr;
	int rtn = 0;

	if (!Account_UI_Query(list, usrName))
		return 0;
	Account_Srv_FetchByName(usrName, &usr);
	if (Account_Srv_DeleteByID(usr.id))
	{
			rtn = 1;
			printf("\t\t\t这个用户信息删除成功!\n\t\t\t按 [Enter] 来返回!\n\t\t\t");
	}
	else
		    printf("\t\t\t这个用户信息删除失败!\n\t\t\t按 [Enter] 来返回!\n\t\t\t");
	getchar();
	return rtn;

}

//根据用户账号名查找该用户账号是否存在，存在返回1，否则返回0，并提示错误信息
int Account_UI_Query(account_list_t  list ,char usrName[]) {
	account_t usr;

	if (!Account_Srv_FindByUsrName(list, usrName))
	{
		printf("\t\t\t这个用户不存在!");
		return 0;
	}
	Account_Srv_FetchByName(usrName, &usr);
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t**************** 你所查找的用户 ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\t用户ID: %d\n",usr.id);
	printf("\t\t\t用户种类:  %d\n", usr.type);
	printf("\t\t\t用户名称: ");
	puts(usr.username);
	printf("\t\t\t用户密码: ");
	puts(usr.password);
	printf("\t\t\t=======================================================\n");
	printf("\t\t\t按 [Enter] 来返回!\n\t\t\t");
	getchar();
	return 1;
}






