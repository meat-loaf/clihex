#include <ncurses.h>
#include <stdio.h>

int main(){
	initscr();
	char g_ch = getch();
	endwin();
	return printf("key name : %s - %d\n", keyname(g_ch), g_ch);
}
	
