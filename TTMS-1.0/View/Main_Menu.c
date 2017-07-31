#include <stdio.h>
#include "Main_Menu.h"
#include "Studio_UI.h"
#include "../Service/Account.h"	
#include "Queries_Menu.h"
#include "Query_Play_UI.h"
#include "Sale_UI.h"
#include "Play_UI.h"
#include "SalesAnalysis_UI.h"
#include "Play_UI.h"	
#include "conio.h"

void Main_Menu(account_t usr)
{

	char choice;
	printf("!!!\n\t\t\t你是一个");
	switch (usr.type)
	{
	case 1:
		printf("售票员!\n");
		do {
            system("clear");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t**************** 剧院票务管理系统 ****************\n");
			printf("\t\t\t[P]剧目信息\n");
			printf("\t\t\t[T]售票与退票\n");
			printf("\t\t\t[Q]查询\n");
			printf("\t\t\t[n]销售统计与分析\n");
			printf("\t\t\t[A]修改密码\n");
			printf("\t\t\t[E]退出\n");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t请输入你的选择:");
			choice = getche();
			switch (choice) {
			case 'A':
			case 'a':
				Account_UI_MgtEntry();
				break;
			case 'P':
			case 'p':
				Play_UI_MgtEntry(1);
				break;
			case 'Q':
			case 'q':
				Queries_Menu();
				break;
			case 'T':
			case 't':
				Sale_UI_MgtEntry();
				break;
			case 'N':
			case 'n':
				SalesAanalysis_UI_MgtEntry();
				break;
			}
		} while ('E' != choice && 'e' != choice);
	
		break;
	case 2:
		printf("经理!\n");
		do {
            
            system("clear");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t**************** 剧院票务管理系统 ****************\n");
			printf("\t\t\t[P]剧目管理\n");
			printf("\t\t\t[Q]查询\n");
			printf("\t\t\t[n]销售统计与分析\n");
			printf("\t\t\t[A]修改密码\n");
			printf("\t\t\t[E]退出\n");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t请输入你的选择:");
			choice = getche();
			switch (choice) {
			case 'A':
			case 'a':
				Account_UI_MgtEntry();
				break;
			case 'P':
			case 'p':
				Play_UI_MgtEntry(0);
				break;
			case 'Q':
			case 'q':
				Queries_Menu();
				break;
			case 'N':
			case 'n':
				SalesAanalysis_UI_MgtEntry();
				break;
			}
		} while ('E' != choice && 'e' != choice);
		break;
	case 9:
		printf("管理员!\n");
		do {
            system("clear");
		printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t**************** 剧院票务管理系统 ****************\n");
			printf("\t\t\t[S]演出厅管理\n");
			printf("\t\t\t[A]账户管理\n");
			printf("\t\t\t[E]退出\n");
		printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t请输入你的选择:");
			choice = getche();
			switch (choice) {
			case 'S':
			case 's':
				Studio_UI_MgtEntry(usr.type);
				break;
			case 'A':
			case 'a':
				Account_UI_MgtEntry();
				break;
			}
		} while ('E' != choice && 'e' != choice);
		break;
	case 0:
	default:
		do {
            system("clear");
		printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t**************** 剧院票务管理系统 ****************\n");
			printf("\t\t\t[P]演出厅管理\n");
			printf("\t\t\t[Q]查询\n");
			printf("\t\t\t[E]退出\n");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t请输入你的选择:");
			choice = getche();
			switch (choice) {
			case 'Q':
			case 'q':
				Queries_Menu();
				break;
			case 'P':
			case 'p':
				Play_UI_MgtEntry(1);
				break;
			}
		} while ('E' != choice && 'e' != choice);
		break;
	}
	printf("\n\t\t\t");
	getchar();
}
