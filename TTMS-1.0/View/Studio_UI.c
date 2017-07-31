#include "Studio_UI.h"

#include "./Main_Menu.h"
#include "../Common/list.h"
#include "../Service/Studio.h"
#include "../Service/Seat.h"
#include "../Service/EntityKey.h"
#include "Seat_UI.h"
#include "conio.h"

extern account_t gl_CurUser;

static const int STUDIO_PAGE_SIZE = 5;

#include <stdio.h>

void Studio_UI_MgtEntry(account_type_t type) {
	int i, id;
	char choice;
	
	studio_list_t head;
	studio_node_t *pos;
	Pagination_t paging;
	List_Init(head, studio_node_t);
	paging.offset = 0;
	paging.pageSize = STUDIO_PAGE_SIZE;
	//载入数据
	paging.totalRecords = Studio_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head, paging);
	if (type==9) {
		do {
		    system("clear");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t********************** Projection Room List **********************\n");
			printf("\t\t\t%-5s  %-18s  %-12s  %-12s  %-12s\n", "ID", "    Name", "Rows Count",
				"Columns Count", "Seats Count");
			printf("\t\t\t------------------------------------------------------------------\n");
			//显示数据
			for (i = 0, pos = (studio_node_t *)(paging.curPos);
				pos != head && i < paging.pageSize; i++) {
				printf("\t\t\t%-5d  %-18s    %-12d  %-12d  %-12d\n", pos->data.id,
					pos->data.name, pos->data.rowsCount, pos->data.colsCount,
					pos->data.seatsCount);
				pos = pos->next;
			}
			printf("\t\t\t------- Total Records:%2d ----------------------- Page %2d/%2d ----\n",
				paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t******************************************************************\n");
			printf("\t\t\t[P]revPage|[N]extPage | [A]dd |[D]elete |[U]pdate | [S]eat | [R]eturn");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\tinput your choice:");
			choice = getche();
			switch (choice)
			{
			case 'a':
			case 'A':
				if (Studio_UI_Add()) //新添加成功，跳到最后一页显示
				{
					paging.totalRecords = Studio_Srv_FetchAll(head);
					Paging_Locate_LastPage(head, paging, studio_node_t);
				}
				break;
			case 'd':
			case 'D':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				getchar();
				if (Studio_UI_Delete(id)) {	//从新载入数据
					paging.totalRecords = Studio_Srv_FetchAll(head);
					List_Paging(head, paging, studio_node_t);
				}
				break;
			case 'u':
			case 'U':
				printf("\n");
				printf("\t\t\tInput the ID:");
				scanf("%d", &id);
				if (Studio_UI_Modify(id)) {	//从新载入数据
					paging.totalRecords = Studio_Srv_FetchAll(head);
					List_Paging(head, paging, studio_node_t);
				}
				break;
			case 's':
			case 'S':
				printf("\n");
				printf("\t\t\tInnput the ID:");
				scanf("%d", &id);
				getchar();
				Seat_UI_MgtEntry(id);
				paging.totalRecords = Studio_Srv_FetchAll(head);
				List_Paging(head, paging, studio_node_t);
				break;
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, -1, studio_node_t);
				}
				break;
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, 1, studio_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
	else
	{
		do {
			system("clear");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\t********************** Projection Room List **********************\n");
			printf("\t\t\t%-5s  %-18s  %-12s  %-12s  %-12s\n", "ID", "    Name", "Rows Count",
				"Columns Count", "Seats Count");
			printf("\t\t\t------------------------------------------------------------------\n");
			//显示数据
			for (i = 0, pos = (studio_node_t *)(paging.curPos);
				pos != head && i < paging.pageSize; i++) {
				printf("\t\t\t%-5d  %-18s    %-12d  %-12d  %-12d\n", pos->data.id,
					pos->data.name, pos->data.rowsCount, pos->data.colsCount,
					pos->data.seatsCount);
				pos = pos->next;
			}
			printf("\t\t\t------- Total Records:%2d ----------------------- Page %2d/%2d ----\n",
				paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
			printf("\t\t\t******************************************************************\n");
			printf("\t\t\t[P]revPage | [N]extPage | [R]eturn");
			printf("\n\t\t\t==================================================================\n");
			printf("\t\t\tinput your choice:");
			choice = getche();
			switch (choice)
			{
			case 'p':
			case 'P':
				if (1 < Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, -1, studio_node_t);
				}
				break;
			case 'n':
			case 'N':
				if (Pageing_TotalPages(paging) > Pageing_CurPage(paging)) {
					Paging_Locate_OffsetPage(head, paging, 1, studio_node_t);
				}
				break;
			}
		} while (choice != 'r' && choice != 'R');
	}
	//释放链表空间
	List_Destroy(head, studio_node_t);
}

int Studio_UI_Add(void) {
	studio_t rec;
	int newRecCount = 0;
	char choice;
	do {
		/*system("cls");*/
		printf("\n\n\t\t\t=======================================================\n");
		printf("\t\t\t****************  Add New Projection Room  ****************\n");
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\tRoom Name:");
		//getchar();
		gets(rec.name);
		printf("\t\t\tRow Count of Seats:");
		scanf("\t\t\t%d", &rec.rowsCount);
		printf("\t\t\tColumn Count of Seats:");
		scanf("%d", &rec.colsCount);
		getchar();
		rec.seatsCount = 0;
		printf("\t\t\t=======================================================\n");

		//获取主键
		rec.id = EntKey_Srv_CompNewKey("Studio");  //主键的获取有问题，已经解决

		if (Studio_Srv_Add(&rec)) {
			newRecCount += 1;
			printf("\t\t\tThe new room added successfully!\n");
		}
		else
			printf("\t\t\tThe new room added failed!\n");
		printf("\t\t\t-------------------------------------------------------\n");
		printf("\t\t\t[A]dd more, [R]eturn:");
		choice = getche();
	} while ('a' == choice || 'A' == choice);
	return newRecCount;
}

int Studio_UI_Modify(int id) {
	studio_t rec;
	int rtn = 0;
	int newrow, newcolumn;
	seat_list_t list;
	int seatcount;

	/*Load record*/
	if (!Studio_Srv_FetchByID(id, &rec)) {
		printf("\t\t\tThe room does not exist!\n\t\t\tPress [Enter] key to return!\n");
		getchar();
		return 0;
	}

	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  Update Projection Room  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\tRoom ID:%d\n", rec.id);
	printf("\t\t\tRoom Name[%s]:", rec.name);
	getchar(); 
	gets(rec.name);

	List_Init(list, seat_node_t);
	seatcount = Seat_Srv_FetchByRoomID(list, rec.id);
	if (seatcount) {
		do {			//如果座位文件中已有座位信息，则更新的行列必须比以前大，否则不允许更改
			printf("\t\t\tRow Count of Seats should >= [%d]:", rec.rowsCount);
			scanf("\t\t\t%d", &(newrow));
			printf("\t\t\tColumn Count of Seats should >= [%d]:", rec.colsCount);
			scanf("%d", &(newcolumn));
		} while (newrow<rec.rowsCount || newcolumn<rec.colsCount);
		getchar();
		rec.rowsCount = newrow;
		rec.colsCount = newcolumn;
		rec.seatsCount = seatcount;
	}
	else {
		printf("\t\t\tRow Count of Seats:");
		scanf("%d", &rec.rowsCount);
		printf("\t\t\tColumn Count of Seats:");
		scanf("%d", &rec.colsCount);
		rec.seatsCount = 0;
	}

	printf("\t\t\t-------------------------------------------------------\n");

	if (Studio_Srv_Modify(&rec)) {
		rtn = 1;
		printf("\t\t\tThe room data updated successfully!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	}
	else
		printf("\t\t\tThe room data updated failed!\n\t\t\tPress [Enter] key to return!\n\t\t\t");
	getchar();
	getchar();
	return rtn;
}

int Studio_UI_Delete(int id) {

	int rtn = 0;

	if (Studio_Srv_DeleteByID(id)) {
		//在删除放映厅时，同时根据放映厅id删除座位文件中的座位
		if (Seat_Srv_DeleteAllByRoomID(id))
			printf("\t\t\tThe seats of the room deleted successfully!\n");
		printf(
			"\t\t\tThe room deleted successfully!\n\t\t\tPress [Enter] key to return!\n");
		rtn = 1;
	}
	else {
		printf("\t\t\tThe room does not exist!\n\t\t\tPress [Enter] key to return!\n");
	}
	printf("\t\t\t");
	getchar();
	return rtn;
}
