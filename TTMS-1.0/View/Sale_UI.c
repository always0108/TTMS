#include "Play_UI.h"
#include "Schedule_UI.h"
#include "Sale_UI.h"
#include "../Common/list.h"
#include "../Service/Ticket.h"
#include "../Service/Seat.h"
#include "../Service/Studio.h"
#include "../Service/EntityKey.h"
#include "../Service/Sale.h"
#include "../Service/Schedule.h"
#define PAGESIZE 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conio.h"

//引用登陆用户的全局数据结构
extern account_t gl_CurUser;

//根据计划ID，显示演出票
void Sale_UI_ShowTicket(int schID)
{
	int pageSize = 10, i;
	int listSize;
	int ticket_id;
	char choice;
	ticket_list_t list, s;
	seat_t buf;
	Pagination_t paging;
	List_Init(list, ticket_node_t);
	listSize = Ticket_Srv_FetchBySchID(list, schID);
	paging.pageSize = pageSize;
	paging.totalRecords = listSize;
	Paging_Locate_FirstPage(list, paging);
	do
	{
		printf("\n\t\t\t=====================影票列表======================\n");
		printf("\t\t\t%-4s %10s   %-8s %4s %4s %8s\n", "id", "演出计划id", "座位id", "行", "列", "状态");
		printf("\t\t\t---------------------------------------------------\n");
		for (i = 0, s = (ticket_node_t *)(paging.curPos); s != list && i < paging.pageSize; i++, s = s->next)
		{
			Seat_Srv_FetchByID(s->data.seat_id, &buf);
			if (buf.status == 1)
				printf("\t\t\t%-4d       %-6d   %-6d %4d %4d %8s\n", s->data.id, s->data.schedule_id, s->data.seat_id, buf.row + 1, buf.column + 1, Status(s->data.status));
		}
		printf("\t\t\t---------------------------------------------------\n");
		printf("\t\t\t------总行数 :%2d --------------页码： %2d/%2d -------\n", paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
		printf("\t\t\t===================================================\n");
		printf("\t\t\t**    【R】返回上一级　　　  【B】订票　　　   　**\n");
		printf("\t\t\t**    【P】上一页　　    　　【N】下一页　       **\n");
		printf("\t\t\t**    【T】 退票　　    　　            　       **\n");
		printf("\t\t\t===================================================\n");
		printf("\t\t\tYour Choice:");
		choice = getche();
		switch (choice)
		{
		case 'B':
		case 'b':
			printf("\n\t\t\t请输入你要订购的票的id：");
			scanf("%d",&ticket_id);
            getchar();
            Sale_UI_SellTicket(ticket_id);
			paging.totalRecords = Ticket_Srv_FetchBySchID(list, schID);
			List_Paging(list, paging, ticket_node_t);
			break;
		case 'r':
		case 'R':
			break;
		case 't':
		case 'T':
			printf("\n\t\t\t请输入你要退的票的id：");
			scanf("%d", &ticket_id);
			getchar();
			Sale_UI_ReturnTicket(ticket_id);
			paging.totalRecords = Ticket_Srv_FetchBySchID(list, schID);
			List_Paging(list, paging, ticket_node_t);
			break;
		case 'p':
		case 'P':
			if (1 < Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(list, paging, -1, ticket_node_t);
			}
			break;
		case 'n':
		case 'N':
			if (Pageing_TotalPages(paging) > Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(list, paging, 1, ticket_node_t);
			}
			break;
		}
	} while (choice != 'r' && choice != 'R');
}


void Sale_UI_ReturnTicket(int ticket_id)
{
	ticket_t buf, data;
	sale_t a;
	if (Ticket_Srv_FetchByID(ticket_id, &buf))
	{
		if (buf.status == 0)
		{
			printf("\t\t\t此票未售出！回车继续");
			getchar();
		}
		else
		{
			//根据票的id获得票售票的基本信息
			Select_Price_By_Playid(buf.id, &a);
			
			data.id = buf.id;
			data.schedule_id = buf.schedule_id;
			data.seat_id = buf.seat_id;
			data.price = buf.price;
			data.status = 0;
			
			if (Ticket_Srv_Modify(&data)&& Sale_Srv_DeleteByID(a.id))
			{
				printf("\t\t\t退订成功！回车继续");
				getchar();
			}
			else
			{
				printf("\t\t\t退订失败！回车继续");
				getchar();
			}
		}
	}
	else
	{
		printf("\t\t\t找不到此票！回车继续");
		getchar();
	}
}

int Sale_UI_SellTicket(int ticket_id)
{
	ticket_t buf, data;
	sale_t sale;

	if (Ticket_Srv_FetchByID(ticket_id, &buf))
	{
		if (buf.status == 1)
		{
			printf("\t\t\t此票已售出！回车继续");
			getchar();
		}
		else
		{
			sale.id = EntKey_Srv_CompNewKey("Sale");
			sale.user_id = gl_CurUser.id;
			sale.date = DateNow();
			sale.time = TimeNow();
			sale.ticket_id = buf.id;
			data.id = buf.id;
			data.schedule_id = buf.schedule_id;
			data.seat_id = buf.seat_id;
			sale.value=data.price = buf.price;
			data.status = 1;
			if (Ticket_Srv_Modify(&data) && Sale_Srv_Add(&sale))
			{
				printf("\t\t\t订单生成时间:%d年-%02d月-%02d日   %d时-%02d分-%02d秒\n", sale.date.year,sale.date.month,sale.date.day,sale.time.hour,sale.time.minute,sale.time.second);
				printf("\t\t\t售票员ID: %d\n",sale.user_id);
				Ticket_Info(&data);
				printf("\t\t\t订购成功！回车继续");
				getchar();
			}
			else
			{
				printf("\t\t\t订购失败！回车继续");
				getchar();
			}
		}
	}
	else
	{
		printf("\t\t\t找不到此票！回车继续");
		getchar();
	}
    return 0;
}

void Ticket_Info(ticket_t *data)
{
	schedule_t p;
	play_t buf;
	seat_t s;
	studio_t studio;
	
	Seat_Srv_FetchByID(data->seat_id, &s);
	Schedule_Srv_FetchByID(data->schedule_id, &p);
	if (Play_Srv_FetchByID(p.play_id, &buf))
	{
		Studio_Srv_FetchByID(p.studio_id,&studio);
		printf("\n\t\t\t====================影票信息======================\n");
		printf("\t\t\t剧目名称: %s\n", buf.name);
		printf("\t\t\t类型：%s\n", type(buf.type));
		printf("\t\t\t出品地区：%s\n", buf.area);
		printf("\t\t\t剧目等级：%s\n", rating(buf.rating));
		printf("\t\t\t时长: %d 分钟\n", buf.duration);
		//printf("上映时间: %d年%d月 %d日\n", buf.start_date.year, buf.start_date.month, buf.start_date.day);
		printf("\t\t\t放映时间: %4d年%02d月%02d日\n",p.date.year,p.date.month,p.date.day);
		printf("\t\t\t放映时间: %2d时%02d分%02d秒\n", p.time.hour, p.time.minute, p.time.second);
		//printf("结束放映时间：%d年%d月%d日\n", buf.end_date.year, buf.end_date.month, buf.end_date.day);
		printf("\t\t\t票价：%d 元\n", buf.price);
		printf("\t\t\t演出厅:%s\n", studio.name);
		printf("\t\t\t位置: %d行%d列\n", s.row + 1, s.column + 1);
		printf("\t\t\t======================================================\n");
		printf("\t\t\tPress Any Key To Continue......");
		getchar();
	}
}
//根据剧目ID显示演出计划
void Sale_UI_ShowScheduler(int playID)
{
	play_t buf;
	if (Play_Srv_FetchByID(playID, &buf))
	{
		int listSize, sal_id, i;
		char choice;
		Pagination_t paging;
		schedule_list_t list, pos,p;
		schedule_t buf;
		List_Init(list, schedule_node_t);
		listSize = Schedule_Srv_FetchByPlay(list, playID);
		paging.pageSize = PAGESIZE;
		paging.totalRecords = listSize;
		Paging_Locate_FirstPage(list, paging);
		do {
			printf("\n\t\t\t************************* 计划表 **************************\n");
			printf("\t\t\t%-3s  %-6s  %8s   %12s  %8s   %6s\n", "ID", "剧目ID", "演出厅ID", "放映日期", "放映时间", "座位数");
			printf("\t\t\t***********************************************************\n");
			//Paging_ViewPage_ForEach(list, paging, schedule_node_t, pos, i)
			for (i = 0, pos = (schedule_node_t *)(paging.curPos); pos != list && i < paging.pageSize; i++, pos = pos->next)
			{
				printf("\t\t\t%-3d     %-3d     %4d    %6d-%2d-%2d  %2d:%02d:%02d  %6d\n", pos->data.id, pos->data.play_id, pos->data.studio_id, pos->data.date.year, pos->data.date.month, pos->data.date.day, pos->data.time.hour, pos->data.time.minute, pos->data.time.second, pos->data.seat_count);
			}
			printf("\t\t\t-----------------------------------------------------------\n");
			printf("\t\t\t-------总记录数:%2d ------------------- 页数： %2d/%2d -------\n", paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t-----------------------------------------------------------\n");
			printf("\t\t\t** 【P】上一页 【[N】下一页 【R】返回 【S】进入售票系统  **\n");
			printf("\t\t\t===========================================================\n");
			printf("\t\t\tYour Choice:");
			fflush(stdin);
			choice = getche();
			switch (choice)
			{
			case 'S':;
			case 's':
				printf("\t\t\t请输入演出计划id :");
				scanf("%d", &sal_id);
				getchar();
				if (!Schedule_Srv_FetchByID(sal_id, &buf))
				{
					printf("\t\t\t你输入的演出计划不存在！回车继续");
					getchar();
				}
				else
				{
					Sale_UI_ShowTicket(sal_id);
				}
				paging.totalRecords = Schedule_Srv_FetchByPlay(list, playID);
				List_Paging(list, paging, schedule_node_t);
				break;
			case 'r':
			case 'R':
				break;
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging))
				{
					Paging_Locate_OffsetPage(list, paging, -1, studio_node_t);
				}
				choice = 'r';
				break;
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging))
				{
					Paging_Locate_OffsetPage(list, paging, 1, studio_node_t);
				}
				choice = 'r';
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
	else
	{
		printf("Not Found !");
		getchar();
	}
}

void Sale_UI_MgtEntry()
{

	int listSize, i, id;
	char choice;
	char name[31];
	Pagination_t paging;
	play_list_t list, p, data, pos;
	List_Init(data, play_node_t);
	List_Init(list, play_node_t);
	listSize = Play_Srv_FetchAll(list);
	paging.pageSize = PAGESIZE;
	paging.totalRecords = listSize;
	Paging_Locate_FirstPage(list, paging);
	do {
		printf("\n\t\t\t========================================== 剧目列表 ========================================\n");
		printf("\t\t\t--------------------------------------------------------------------------------------------\n");
		printf("\t\t\t%-5s  %-16s  %-4s  %8s  %8s %8s  %10s  %10s  %5s\n",
			"ID", "    Name", "type", "area", "rating", "duration", "start_date", "end_date", "price");
		printf("\t\t\t--------------------------------------------------------------------------------------------\n");
		for (i = 0, p = (play_node_t *)(paging.curPos); p != list && i < paging.pageSize; i++, p = p->next)
		{
			printf("\t\t\t%-5d  %-16s  %-4s  %8s   %8s  %5d   %4d-%02d-%02d   %4d-%02d-%02d   %-5d\n", p->data.id, p->data.name, type(p->data.type),
				p->data.area,rating(p->data.rating), p->data.duration, p->data.start_date.year, p->data.start_date.month, p->data.start_date.day, p->data.end_date.year, p->data.end_date.month, p->data.end_date.day, p->data.price);
		}
		printf("\t\t\t--------------------------------------------------------------------------------------------\n");
		printf("\t\t\t------总行数 :%2d ----------------------------------------------------- 页码： %2d/%2d --------\n", paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
		printf("\t\t\t============================================================================================\n");
		printf("\t\t\t**   【P】上一页               【R】返回                     【S】查询剧目(By Name)       **\n");
		printf("\t\t\t**   【N】下一页               【C】显示演出计划	                                  **\n");
		printf("\t\t\t============================================================================================\n");
		printf("\t\t\tYour Choice:");
		fflush(stdin);
		choice = getche();
		switch (choice)
		{
		case 'C':;
		case 'c':
			printf("\t\t请输入您要查询的剧目id:");
			scanf("%d", &id);
			getchar();
			Sale_UI_ShowScheduler(id);
			paging.totalRecords = Play_Srv_FetchAll(list);
			List_Paging(list, paging, play_node_t);
			break;
		case 'S':
		case 's':
			printf("\t\t请输入您要查询的剧目名称:");
			scanf("%s", name);
			getchar();
			if (Play_Srv_FetchByName(data, name))
			{
				Print_Play(data);
			}
			else
			{
				printf("Not Found!");
				getchar();
			}
			paging.totalRecords = Play_Srv_FetchAll(list);
			List_Paging(list, paging, play_node_t);
			break;
		case 'r':
		case 'R':
			break;
		case 'p':
		case 'P':
			if (1 < Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(list, paging, -1, studio_node_t);
			}
			break;
		case 'n':
		case 'N':
			if (Pageing_TotalPages(paging) > Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(list, paging, 1, studio_node_t);
			}
			break;
		}
	} while (choice != 'r'&&choice != 'R');
	List_Destroy(list, play_node_t);
}

void Print_Play(play_list_t list)
{
	play_list_t p;
	p = list->next;
	do
	{
		printf("\n\t\t\t=================剧目信息============================\n");
		printf("\t\t\t剧目id: %d\n", p->data.id);
		printf("\t\t\t剧目名称: %s\n", p->data.name);
		printf("\t\t\t类型：%s\n", type(p->data.type));
		printf("\t\t\t出品地区：%s\n", p->data.area);
		printf("\t\t\t剧目等级：%s\n",rating(p->data.rating));
		printf("\t\t\t时长: %d 分钟\n", p->data.duration);
		printf("\t\t\t上映时间: %d年%d月 %d日\n", p->data.start_date.year, p->data.start_date.month, p->data.start_date.day);
		printf("\t\t\t结束放映时间：%d年%d月%d日\n", p->data.end_date.year, p->data.end_date.month, p->data.end_date.day);
		printf("\t\t\t票价：%d 元\n", p->data.price);
		printf("\t\t\t======================================================\n");
		p = p->next;
	} while (p != list);
	printf("\t\t\tPress Any Key To Continue......");
	getchar();
}


char * Status(int i)
{
	if (i == 1)
	{
		return "已定";
	}
	else if (i == 0)
	{
		return "未定";
	}
	else if (i == -1)
	{
		return "损坏";
	}
	else
	{
		return "其他";
	}
}
