#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <io.h>
#include "./View/Main_Menu.h"
#include "./View/Account_UI.h"
//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, USR_ANOMY, "Anonymous","" };

int main(void)
{
    system("clear");
	setvbuf(stdout, NULL, _IONBF, 0);    //设置无缓冲输出;
	if (!SysLogin()) {
		printf("\n\t\t\t对不起您无权登录本系统请按任意键退出......\n");
		getchar();
		return EXIT_SUCCESS;
	}
	Main_Menu(gl_CurUser);
	printf("\t\t\t再见!!!\n");
    printf("\t\t\t");
	return EXIT_SUCCESS;
}
	
