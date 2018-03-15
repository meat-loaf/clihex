/*#ifndef INC_NCURSESW_LIBS
#define INC_NCURSESW_LIBS 1
//#include <ncursesw/curses.h>
#include <curses.h>
#endif
*/
#ifndef _LOCAL_LIB_NCURSES_DEF
#define _LOCAL_LIB_NCURSES_DEF 1
#include <curses.h>
#endif

#ifndef LOCAL_FILES_HEADER
#define LOCAL_FILES_HEADER 1
#include "files.h"
#endif


#ifndef STDLIB_DEFS
#define STDLIB_DEFS 1
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifndef LOCAL_GUI_HEADER
#define LOCAL_GUI_HEADER 1
#include "gui.h"
#endif

//TODO move structs to header file



void init_ncurses(){
	setlocale(LC_ALL, "");
	initscr();
	raw();
	noecho();
}

void
end_ncurses(){
	endwin();
}
int
main(int argc, char **argv){
	//TODO add file loading mechanism
	//and handling of multiple files
	if (argc != 2){
		exit(0);
	}
	init_ncurses();
	editor_entry(argv[1]);
	end_ncurses();	
	return 0;
}

