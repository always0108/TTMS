#include "Play_UI.h"
#include "Schedule_UI.h"
#include "../Common/list.h"
#include "../Service/Play.h"
#include "../Service/EntityKey.h"
#include <stdio.h>
#include <stdlib.h>
#include "Studio_UI.h"

extern  gl_CurUser;

	/*
	 * Function:    Play_UI_ShowList
	 * Function ID:	TTMS_SCU_Play_UI_Show
	 * Description: 以列表模式显示剧目信息
	 * Input:       list剧目信息链表的头指针，paging分页设置参数
	 * Output:      所有剧目的信息
	 * Return:      返回查找到的记录数目
	 */
void Play_UI_ShowList(play_list_t list, Pagination_t paging) {
	int i;
	play_node_t *pos;
	printf("\n\t\t\t剧目类型定义，1表示电影，2表示歌剧，3表示音乐会\n\t\t\t演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看\n");
	printf("\n\t\t\t==================================================================================================\n");
	printf("\t\t\t******************************    Projection    play    List     *********************************\n");
	printf("\t\t\t--------------------------------------------------------------------------------------------------\n");
	printf("\t\t\t%-5s  %-16s  %-4s  %8s  %8s %8s  %10s  %10s  %5s\n",
		"ID", "    Name", "type", "area", "rating", "duration", "start_date", "end_date", "price");
	printf("\t\t\t--------------------------------------------------------------------------------------------------\n");
	//显示数据
	for (i = 0, pos = (play_node_t *)(paging.curPos);
		pos != list && i < paging.pageSize; i++) {
		printf("\t\t\t%-5d  %-16s  %-4s  %8s   %8s  %5d   %4d-%02d-%02d   %4d-%02d-%02d   %-5d\n", pos->data.id,
			pos->data.name, type(pos->data.type), pos->data.area,
			rating(pos->data.rating), pos->data.duration, pos->data.start_date.year, pos->data.start_date.month, pos->data.start_date.day,
			pos->data.end_date.year, pos->data.end_date.month, pos->data.end_date.day, pos->data.price);
		pos = pos->next;
	}
	//剧目类型定义，1表示电影，2表示歌剧，3表示音乐会
	//演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看

}

/*
 * Function:    Play_UI_MgtEntry
 * Function ID:	TTMS_SCU_Play_UI_MgtEnt
 * Description: 剧目信息管理界面
 * Input:       flag为0，进入管理页面，flag为1进入查询界面
 * Output:      剧目管理界面
 * Return:      无
 */
void  Play_UI_MgtEntry(int flag){  
	int i, id;
	char choice;
	play_list_t head;
	play_node_t *pos;
	Pagination_t paging;
	List_Init(head, play_node_t);
	paging.offset = 0;
	paging.pageSize = PLAY_PAGE_SIZE;
	//载入数据
	paging.totalRecords = Play_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head, paging);

	if (flag)
	{
		do {
			system("clear");
			Play_UI_ShowList(head, paging);
			printf("\t\t\t------- Total Records:%2d ----------------------- Page %2d/%2d ----\n",
				paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t******************************************************************\n");
			printf("\t\t\t [P]revPage  |  [N]extPage |  [Q]uery |  [R]eturn");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\tinput your choice:");
			choice = getche();
			switch (choice)
			{
			case 'q':
			case 'Q':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();
				if (Play_UI_Query(id)) {	//从新载入数据
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head, paging, play_node_t);
				}
				break;
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, -1, play_node_t);
				}
				break;
			
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, 1, play_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
	else
	{
		do {
			system("clear");
			Play_UI_ShowList(head, paging);
			printf("\t\t\t------- Total Records:%2d ----------------------- Page %2d/%2d ----\n",
				paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t**************************************************************************\n");
			printf("\t\t\t[P]revPage | [N]extPage | [A]dd | [D]elete | [U]pdate | [Q]uery| [S]chedule | [R]eturn");
			printf("\n\t\t\t==========================================================================\n");
			printf("\t\t\tinput your choice:");
			choice = getche();
			switch (choice)
			{
			case 's':
			case 'S':
				/*printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();*/
				Schedule_UI_MgtEntry();
				break;
			case 'a':
			case 'A':
				if (Play_UI_Add()) //新添加成功，跳到最后一页显示
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					Paging_Locate_LastPage(head, paging, play_node_t);
				}
				break;
			case 'd':
			case 'D':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();
				if (Play_UI_Delete(id)) {	//从新载入数据
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head, paging, play_node_t);
				}
				break;
			case 'u':
			case 'U':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();
				if (Play_UI_Modify(id)) {	//从新载入数据
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head, paging, play_node_t);
				}
				break;
			case 'q':
			case 'Q':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();
				if (Play_UI_Query(id)) {	//从新载入数据
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head, paging, play_node_t);
				}
				break;
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, -1, play_node_t);
				}
				break;
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, 1, play_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
}


/*
 * Function:    Play_UI_Add
 * Function ID:	TTMS_SCU_Play_UI_Add
 * Description: 添加一条剧目信息
 * Input:       无
 * Output:      输入信息时的各种提示
 * Return:      添加的记录数
 */
int Play_UI_Add(void){
	
	play_t rec;
	char choice;
	int newRecCount = 0;

	do {
		/*system("cls");*/
		printf("\n\t\t\t=======================================================\n");
		printf("\t\t\t****************  Add New Projection Play  ****************\n");
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\tPlay Name:");
		fflush(stdin);
		//getchar();
		gets(rec.name);
		printf("\t\t\tPlay type:");
		scanf("%d", &rec.type);//剧目类型定义，1表示电影，2表示歌剧，3表示音乐会
		printf("\t\t\tPlay area:");
		getchar();
		gets(rec.area);
		printf("\t\t\tPlay rating:");
		scanf("%d", &rec.rating);//演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看
		printf("\t\t\tPlay duration:");
		scanf("%d",&rec.duration);
		printf("\t\t\tPlay start_date:");
		scanf("%d%d%d", &rec.start_date.year, &rec.start_date.month, &rec.start_date.day);
		printf("\t\t\tPlay end_date:");
		scanf("%d%d%d",&rec.end_date.year, &rec.end_date.month, &rec.end_date.day);
		printf("\t\t\tPlay price:");
		scanf("%d", &rec.price);
		getchar();
		printf("\t\t\t=======================================================\n");

		//获取主键
		rec.id = EntKey_Srv_CompNewKey("Play");

		if (Play_Srv_Add(&rec)) {
			newRecCount += 1;
			printf("\t\t\tThe new play added successfully!\n");
		}
		else
			printf("\t\t\tThe new play added failed!\n");
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\t[A]dd more, [R]eturn:");
		fflush(stdin);
		choice = getche();
	}while ('a' == choice || 'A' == choice);
	return newRecCount;
}

/*
 * Function:    Play_UI_Modify
 * Function ID:	TTMS_SCU_Play_UI_Mod
 * Description: 更新剧目信息
 * Input:       待更新的剧目ID号
 * Output:      输入信息时的各种提示
 * Return:      更新的剧目信息数，0表示未找到，1表示找到并更新
 */
int Play_UI_Modify(int id){
	play_t rec;
	int rtn = 0;

	/*Load record*/
	if (!Play_Srv_FetchByID(id, &rec)) {
		printf("\t\t\tThe play does not exist!\nPress [Enter] key to return!\n");
		getchar();
		return 0;
	}
	//界面	
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  Update Projection Play  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\tPlay ID:%d\n", rec.id);
	printf("\t\t\tPlay Name: [%s]\n", rec.name);
	printf("\t\t\tPlay type:%s\n", type(rec.type));//剧目类型定义，1表示电影，2表示歌剧，3表示音乐会
	printf("\t\t\tPlay area:%s\n", rec.area);
	printf("\t\t\tPlay rating:%s\n", rating(rec.rating));//演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看
	printf("\t\t\tPlay duration:%d\n", rec.duration);
	printf("\t\t\tPlay start_date:%d-%d-%d\n", rec.start_date.year,rec.start_date.month,rec.start_date.day);
	printf("\t\t\tPlay end_date:%d-%d-%d\n", rec.end_date.year, rec.end_date.month, rec.end_date.day);
	printf("\t\t\tPlay price:%d\n", rec.price);
	
	printf("\t\t\t输入你修改后的信息:\n");//如果只改某一项，这就比较麻烦，后期需要优化
	fflush(stdin);
	printf("\t\t\tPlay name:");
	gets(rec.name);
	printf("\t\t\tPlay type:");
	scanf("%d", &rec.type);//剧目类型定义，1表示电影，2表示歌剧，3表示音乐会
	getchar();
	printf("\t\t\tPlay area:");
	gets(rec.area);
	printf("\t\t\tPlay rating:");
	scanf("%d", &rec.rating);//演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看
	printf("\t\t\tPlay duration:");
	scanf("%d", &rec.duration);
	printf("\t\t\tPlay start_date:");
	scanf("%d%d%d", &rec.start_date.year, &rec.start_date.month, &rec.start_date.day);
	printf("\t\t\tPlay end_date:");
	scanf("%d%d%d", &rec.end_date.year, &rec.end_date.month, &rec.end_date.day);
	printf("\t\t\tPlay price:");
	scanf("%d", &rec.price);
	
	printf("\t\t\t-------------------------------------------------------\n");

	if (Play_Srv_Modify(&rec)) {
		rtn = 1;
		printf(
			"\t\t\tThe play data updated successfully!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	}
	else
		printf("\t\t\tThe play data updated failed!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	getchar();
	getchar();
	return rtn;
}

/*
 * Function:    Play_UI_Delete
 * Function ID:	TTMS_SCU_Play_UI_Del
 * Description: 按照ID号删除剧目信息
 * Input:       待删除的剧目ID号
 * Output:      提示删除是否成功
 * Return:      0表示删除失败，1表示删除成功
 */
int Play_UI_Delete(int id){

	int rtn = 0;

	if (Play_Srv_DeleteByID(id)) {
		printf("\t\t\tThe play was deleted successfully!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
		rtn = 1;
	}
	else {
		printf("\t\t\tThe play does not exist!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	}
	getchar();
	return rtn;
}

/*
 * Function:    Play_UI_Query
 * Function ID:	TTMS_SCU_Play_UI_Qry
 * Description: 按照ID号查询剧目信息
 * Input:       待查找的剧目ID号
 * Output:      查找到的剧目信息
 * Return:      0表示未找到，1表示找到了
 */
int Play_UI_Query(int id){
	int rtn = 0;
	play_t rec;
	if (!Play_Srv_FetchByID(id, &rec)) {
		printf("\t\t\tThe play does not exist!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
		getchar();
		return 0;
	}
	//界面	
	printf("\n\t\t\t======================================================\n");
	printf("\t\t\t****************   Play  Information   ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\tPlay ID:%d\n", rec.id);
	printf("\t\t\tPlay Name: %s\n", rec.name);
	printf("\t\t\tPlay type:%s\n", type(rec.type));//剧目类型定义，1表示电影，2表示歌剧，3表示音乐会
	printf("\t\t\tPlay area:%s\n", rec.area);
	printf("\t\t\tPlay rating:%s\n", rating(rec.rating));//演出级别类型定义，1表示儿童可观看，2表示青少年可观看，3表示成人可观看
	printf("\t\t\tPlay duration:%d\n", rec.duration);
	printf("\t\t\tPlay start_date:%d-%d-%d\n", rec.start_date.year, rec.start_date.month, rec.start_date.day);
	printf("\t\t\tPlay end_date:%d-%d-%d\n", rec.end_date.year, rec.end_date.month, rec.end_date.day);
	printf("\t\t\tPlay price:%d\n", rec.price);
	printf("\t\t\tPress[Enter] key to return!\n\t\t\t");
	getchar();
	return 1;
}

char *type(int type)
{
	if (type == 1)
	{
		return "电影";
	}
	else if (type == 2)
	{
		return "歌剧";
	}
	else if (type == 3)
	{
		return  "音乐";
	}
	else
		return "其他";
}
char *rating(int rating)
{
	if (rating == 1)
	{
		return "儿童剧";
	}
	else if (rating == 2)
	{
		return "青年剧";
	}
	else if (rating == 3)
	{
		return "成人剧";
	}
	else
		return "其它";
}

/*
typedef enum {
PLAY_TYPE_FILE=1,						//儿童剧
PLAY_TYPE_OPEAR=2,					//少儿剧
PLAY_TYPE_CONCERT=3				//成人剧
} play_rating_t;
typedef enum {
PLAY_RATE_CHILD = 1,						// 电影
PLAY_RATE_TEENAGE = 2,					//歌剧
PLAY_RATE_ADULT = 3							//音乐会
} play_type_t;
*/
