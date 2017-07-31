/*
* File name:  Schedule_UI.c
* File ID:	  TTMS_SSF_Schedule_UI_Sour
* Author:     Huang Ru
* Version:    V1.0
* Date: 	  2015-04-25
*/


#include "Schedule_UI.h"
#include "Play_UI.h"

#include "../Common/list.h"
#include "../Service/Schedule.h"
#include "../Service/Play.h"
#include "../Service/Studio.h"
#include "../Service/EntityKey.h"
#include "../Service/Seat.h"
#include "../Service/Ticket.h"

#include "conio.h"
#include <stdio.h>
#include <stdlib.h>

	/*
	 * Function:    Schedule_UI_ListAll
	 * Function ID:	TTMS_SCU_Schedule_UI_List
	 * Description: 显示所有的演出计划信息,进行查询。
	 * Input:       无
	 * Output:      所有剧目的信息
	 * Return:      无
	 */
void Schedule_UI_ListAll(void) {
	int i;
	schedule_list_t head;          //双向链表的定义  
	schedule_node_t *pos;          //双向链表的一个结点  
	char choice;
	Pagination_t paging;
	List_Init(head, schedule_node_t);        //链表
	paging.offset = 0;       //初始化  记录 
	paging.pageSize = SCHEDULE_PAGE_SIZE;
	//载入数据
	paging.totalRecords = Schedule_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head, paging);
		//显示数据
		do {
			system("clear");
			printf("\n\n\n");
			printf("\t\t\t╔ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╗\n");
			printf("\t\t\t╬ぷぷぷぷぷぷぷぷぷぷぷぷぷ    演出计划菜单   ぷぷぷぷぷぷぷぷぷぷぷぷぷ╬\n");
			printf("\t\t\t╚ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╝\n");
			printf("\n");
			printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n");
			printf("\t\t\t%8s %8s %10s %15s    %14s %12s\n", "计划ID号", "剧目ID号", "放映厅ID号", "演出日期", "演出时间", "座位数量");
			printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n\n");
			//显示数据
			for (i = 0, pos = (schedule_node_t *)(paging.curPos);
				pos != head && i < paging.pageSize; i++) {
				printf("\t\t\t%5d %8d %8d    %8d年%02d月%02d日   %8d时%02d分%02d秒  %3d\n", pos->data.id,
					pos->data.play_id, pos->data.studio_id, pos->data.date.year, pos->data.date.month, pos->data.date.day,
					pos->data.time.hour, pos->data.time.minute, pos->data.time.second, pos->data.seat_count);
				pos = pos->next;
			}
			printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n");
			printf("\n\t\t\t------- 总共%2d条记录 ------------- 页数： %2d/%2d --------------------\n",
				paging.totalRecords, Pageing_CurPage(paging),
				Pageing_TotalPages(paging));
			printf("\t\t\t[F]第一页|[p]上一页|[N]下一页|[L]最后一页|[e]退出\n");
			printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n");
			printf("\t\t\t请输入选项：");
			fflush(stdin);
			choice = getche();
		    switch(choice)
			{
					case 'f':
					case 'F':
					Paging_Locate_FirstPage(head,paging);
					break;
					case 'p':
				    case 'P':
			        if(1<Pageing_CurPage(paging))
					{
			   		    Paging_Locate_OffsetPage(head,paging,-1, schedule_node_t);
			    	 }
			    	break;	
				    case 'n':   
					case 'N':
						if(Pageing_TotalPages(paging)>Pageing_CurPage(paging))
						{
							Paging_Locate_OffsetPage(head,paging,1, schedule_node_t);
						}
						break;
					case 'l':
					case 'L':
					Paging_Locate_LastPage(head,paging, schedule_node_t);
						break;
				}
		} while (choice != 'e'&&choice != 'E');
		//getchar();
}

/*
 * Function:    Schedule_UI_Add
 * Function ID:	TTMS_SCU_Schedule_UI_Add
 * Description: 添加一条演出计划信息
 * Input:       相关剧目的ID号
 * Output:      输入信息时的各种提示
 * Return:      添加的记录数
 */
int Schedule_UI_Add(void) {
	schedule_t rec;
	seat_list_t list;
	List_Init(list, seat_node_t);
	char choice;
	int newRecCount = 0;
	//rec.play_id = play_id;
	do {
		/*system("cls");*/
		printf("\n\t\t\t===============================================================\n");
		printf("\t\t\t****************  Add New Projection Schedule  ****************\n");
		printf("\t\t\t---------------------------------------------------------------\n");
		
		fflush(stdin);
		printf("\t\t\tPlay ID:");
		scanf("%d", &rec.play_id);
		printf("\t\t\tStudio ID:");
		scanf("%d", &rec.studio_id);
		printf("\t\t\tschedule date:");
		scanf("%d%d%d", &rec.date.year, &rec.date.month, &rec.date.day);
		printf("\t\t\tSchedule time:");
		scanf("%d%d%d", &rec.time.hour, &rec.time.minute, &rec.time.second);
		getchar();

		printf("\t\t\t=======================================================\n");

		//获取主键
		rec.id = EntKey_Srv_CompNewKey("Schedule");
		rec.seat_count = Seat_Srv_FetchValidByRoomID(list, rec.studio_id);

		if (Schedule_Srv_Add(&rec)&&Ticket_Srv_AddBatch(rec.id,rec.studio_id)) {
			newRecCount += 1;
			printf("\t\t\tThe new play  schedule added successfully!\n");
		}
		else {
			printf("\t\t\tThe new play schedule added failed!\n");
			Schedule_Srv_DeleteByID(rec.id);
		}
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\t[A]dd more, [R]eturn:");
		fflush(stdin);
		choice = getche();
	} while ('a' == choice || 'A' == choice);
	return newRecCount;
}

/*
 * Function:    Schedule_UI_Modify
 * Function ID:	TTMS_SCU_Schedule_UI_Mod
 * Description: 按照ID号更新演出计划信息
 * Input:       待更新的剧目ID号
 * Output:      输入信息时的各种提示
 * Return:      更新的剧目信息数，0表示未找到，1表示找到并更新
 */
int Schedule_UI_Modify(int id){

	schedule_t rec;
	int rtn = 0;

	/*Load record*/
	if (!Schedule_Srv_FetchByID(id, &rec)) {
		printf("\t\t\tThe schdeule does not exist!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
		getchar();
		return 0;
	}
	//界面	
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  Update Projection Schedule  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\tPlay ID:");
	fflush(stdin);
	printf("%d\n", rec.play_id);
	printf("\t\t\tStudio ID:");
	printf("%d\n", rec.studio_id);
	printf("\t\t\tschedule date:");
	printf("%d年%d月%d日\n", rec.date.year, rec.date.month, rec.date.day);
	printf("\t\t\tSchedule time:");
	printf("%d时%d分%d秒\n", rec.time.hour, rec.time.minute, rec.time.second);
	Ticket_Srv_DeleteBatch(rec.id);

	printf("\t\t\t输入你修改后的信息:\n");//如果只改某一项，这就比较麻烦，后期需要优化
	printf("\t\t\tPlay ID:");
	fflush(stdin);
	scanf("%d", &rec.play_id);
	printf("\t\t\tStudio ID:");
	scanf("%d", &rec.studio_id);
	printf("\t\t\tSchedule date:");
	scanf("%d%d%d", &rec.date.year, &rec.date.month, &rec.date.day);
	printf("\t\t\tSchedule time:");
	scanf("%d%d%d", &rec.time.hour, &rec.time.minute, &rec.time.second);

	printf("\t\t\t-------------------------------------------------------\n");
	rec.seat_count=Ticket_Srv_AddBatch(rec.id, rec.studio_id);
	if (Schedule_Srv_Modify(&rec)) {
		rtn = 1;
		printf(
			"\t\t\tThe schedule data updated successfully!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	}
	else
		printf("\t\t\tThe schedule data updated failed!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	getchar();
	getchar();
	return rtn;
}


/*
 * Function:    Schedule_UI_Delete
 * Function ID:	TTMS_SCU_Schedule_UI_Del
 * Description: 按照ID号删除演出计划信息
 * Input:       待删除的剧目ID号
 * Output:      提示删除是否成功
 * Return:      0表示删除失败，1表示删除成功
 */
int Schedule_UI_Delete(int id){
	int rtn = 0;
	schedule_t rec;

	if (!Schedule_Srv_FetchByID(id, &rec)) {
		printf("\t\t\tThe play does not exist!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
		getchar();
		return 0;
	}
	if (Schedule_Srv_DeleteByID(id)&& Ticket_Srv_DeleteBatch(rec.studio_id)) {
		printf("\t\t\tThe schedule was deleted successfully!\n\t\t\t");
		rtn = 1;
	}
	else {
		printf("\t\t\tThe schedule does not exist!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	}
	getchar();
	getchar();
	return rtn;

	return 0;
}

/*
 * Function:    Schedule_UI_Query
 * Function ID:	TTMS_SCU_Schedule_UI_Qry
 * Description: 按照ID号查询演出计划信息
 * Input:       待查找的剧目ID号
 * Output:      查找到的剧目信息
 * Return:      0表示未找到，1表示找到了
 */
int Schedule_UI_Query(int id){
	int rtn = 1;
	schedule_t  buf;

	if (Schedule_Srv_FetchByID(id, &buf)) {
		system("cls");
		printf("\n\n\n");
		printf("\t\t\t╔ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╗\n");
		printf("\t\t\t╬ぷぷぷぷぷぷぷぷぷぷ    ID为%3d的演出计划    ぷぷぷぷぷぷぷぷぷ╬\n", buf.id);
		printf("\t\t\t╚ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╝\n");
		printf("\n\n\n\t\t\t");
		rtn = 1;
		printf("\t\t\t计划ID: %d\n", buf.id);
		printf("\t\t\t剧目ID: %d\n", buf.play_id);
		printf("\t\t\t放映厅ID: %d\n", buf.studio_id);
		printf("\t\t\t演出日期:%d年%d月%d日\n", buf.date.year, buf.date.month, buf.date.day);
		printf("\t\t\t演出时间:%d时%d分%d秒\n", buf.time.hour, buf.time.minute, buf.time.second);
		printf("\t\t\t座位数量:%d\n", buf.seat_count);
		printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n");
		printf("\t\t\t输入回车键结束");
		while (getchar() != '\n');
	}
	else
	{
		printf("\t\t\t查找不到该演出计划！");
	}
	getchar();
	return rtn;
}

/*
 * Function:    Schedule_UI_ListByPlay
 * Function ID:	TTMS_SCU_Schedule_UI_ListByPlay
 * Description: 以列表模式显示给定剧目的演出计划信息
 * Input:       list为查找到的演出计划信息链表的头指针，play为剧目信息，paging为显示分页设置参数
 * Output:      无
 * Return:      返回查找到的记录数目
 */
void Schedule_UI_ListByPlay(const play_t *play, schedule_list_t list, Pagination_t paging){
}

/*
 * Function:    Schedule_UI_MgtEntry
 * Function ID:	TTMS_SCU_Schedule_UI_MgtEnt
 * Description: 演出计划管理界面，演出计划按照剧目进行管理，以剧目ID号为输入
 * Input:       相关剧目ID号
 * Output:      演出计划信息
 * Return:      无
 */
void Schedule_UI_MgtEntry(void){
	int id;
	char choice;
	char ch;
	schedule_list_t head;
	//schedule_node_t *pos;
	Pagination_t paging;
	List_Init(head, schedule_node_t);
	paging.offset = 0;
	paging.pageSize = SCHEDULE_PAGE_SIZE;
	//载入数据
	paging.totalRecords = Schedule_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head, paging);
	do {
		//system("cls");
		printf("\n\n\n");
		printf("\t\t\t╔ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╗\n");
		printf("\t\t\t╬  ぷぷぷぷぷぷぷぷぷぷぷ     演出计划管理     ぷぷぷぷぷぷぷぷぷぷ ╬\n");
		printf("\t\t\t╚ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ╝\n");
		printf("\n");
		printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n");
		printf("\t\t\t[Y]打印演出计划");
		printf("\t[A]添加演出计划");
		printf("\t[U]修改演出计划");
		printf("\t[D]删除演出计划\n");
		printf("\t\t\t[Q]查找演出计划");
		printf("\t[P]上一页\t[N]下一页\t[R]返回\n");
		printf("\t\t\t= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n");
		printf("\n\t\t\t请选择:");
		fflush(stdin);
		choice = getche();
		fflush(stdin);
		switch (choice)
		{
		case 'a':
		case 'A':
			//添加演出计划
			if (Schedule_UI_Add())     //新添加成功，跳到最后一页显示
			{
				paging.totalRecords = Schedule_Srv_FetchAll(head);
				Paging_Locate_LastPage(head, paging, schedule_node_t);
			}
			break;
		case 'u':
		case 'U':
			//修改演出计划
			printf("\t\t\t输入 ID:");
			while (scanf("%d", &id) != 1)
			{
				printf("\t\t\t您输入的不是数字\n");
				printf("\t\t\t请重新输入：");
				while ((ch = getchar()) != '\n')
					continue;
			}
			fflush(stdin);
			if (Schedule_UI_Modify(id)) {	//从新载入数据
				paging.totalRecords = Schedule_Srv_FetchAll(head);
				List_Paging(head, paging, schedule_node_t);
			}
			break;
		case 'd':
		case 'D':
			//删除演出计划
			printf("\t\t\t输入 ID:");
			while (scanf("%d", &id) != 1)
			{
	     		printf("\t\t\t您输入的不是数字\n");
				printf("\t\t\t请重新输入：");
				while ((ch = getchar()) != '\n')
					continue;
			}
			fflush(stdin);
			if (Schedule_UI_Delete(id)) {	//从新载入数据
				paging.totalRecords = Schedule_Srv_FetchAll(head);
				List_Paging(head, paging, schedule_node_t);
			}
			break;
		case 'y':
		case 'Y':
			//打印演出计划
			Schedule_UI_ListAll();
			break;
		case 'q':
		case 'Q':
			printf("\t\t\t输入要查找的ID:");
			while (scanf("%d", &id) != 1)
			{
				printf("\t\t\t您输入的不是数字\n");
				printf("\t\t\t请重新输入：");
				while ((ch = getchar()) != '\n')
					continue;
			}
			fflush(stdin);
			Schedule_UI_Query(id);
			break;
		case 'p':
		case 'P':
			if (1 < Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(head, paging, -1, schedule_node_t);
			}
			break;
		case 'n':
		case 'N':
			if (Pageing_TotalPages(paging) > Pageing_CurPage(paging))
			{
				Paging_Locate_OffsetPage(head, paging, 1, schedule_node_t);
			}
			break;
		}
	} while (choice != 'R' && choice != 'r');
	//释放链表空间
	List_Destroy(head, schedule_node_t);
}                     
