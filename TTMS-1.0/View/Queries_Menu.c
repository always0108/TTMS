#include <stdio.h>
#include <stdlib.h>

#include "Queries_Menu.h"
#include "Schedule_UI.h"
#include "Studio_UI.h"
#include "Play_UI.h"
#include "conio.h"

void Queries_Menu(void){
	char choice;
	do {
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t************************Information Queries*************************\n");
		printf("\t\t\t[S]tudio   Query.\n");
		printf("\t\t\t[P]lay     Query.\n");
		//printf("\t\t\t[T]icket   Query.\n");
		printf("\t\t\tS[c]hedule Queries\n");
		printf("\t\t\t[R]eturn.\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\tPlease input your choice:");
		fflush(stdin);
		choice = getche();
		switch (choice) {
		case 'S':
		case 's'://Studio_query;
			Studio_UI_MgtEntry(2);
			break;
		case 'P':
		case 'p':
			Play_UI_MgtEntry(1);
			break;
		//case 'T':
		//case 't': //Ticket_query;
			//Ticket_UI_MgtEntry();
			//break;
		case 'C':
		case 'c':
			Schedule_UI_ListAll();
			break;
		}
	} while ('R' != choice && 'r' != choice);
	}
