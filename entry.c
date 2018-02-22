#ifndef INC_NCURSESW_LIBS
#define INC_NCURSESW_LIBS 1
#include <ncursesw/curses.h>
#endif


#include <locale.h>
#include <stdlib.h>
#include <stdio.h>

struct editor {
	WINDOW *mainwin;
	WINDOW *asciiwin;
	WINDOW *offsetwin;
	WINDOW *cmdwin;	
};

void editor_entry();
void print_file(struct editor, char *);
void input_loop(struct editor);

void init_ncurses(){
	//TODO grab system locale (...?)
	setlocale(LC_ALL, "en_us.UTF-8");
	initscr();
	raw();
	noecho();
}

void end_ncurses(){
	endwin();
}
int main(int argc, char **argv){
	if (argc != 2){
		exit(0);
	}
	init_ncurses();
	editor_entry(argv[1]);
	end_ncurses();	
	return 0;
}

//ready for ncurses
void editor_entry(char *filename){
	struct editor win;
	win.mainwin = newwin(LINES-1, (COLS-8)*3/4, 0, 8);
	win.asciiwin = newwin(LINES-1, ((COLS-8)*1/4), 0, ((COLS-8)*3/4)+8);
	win.offsetwin = newwin(LINES-1, 8, 0, 0);
	win.cmdwin = newwin(1, COLS, LINES-1, 0);
	keypad(win.mainwin, TRUE);
	keypad(win.asciiwin, TRUE);
	keypad(win.offsetwin, TRUE);
	keypad(win.cmdwin, TRUE);
	wmove(win.mainwin, 1, 1);	
	box(win.mainwin, '|', '-');
	box(win.asciiwin, '-', '|');
	box(win.offsetwin, '[', ']');
	box(win.cmdwin, '*', '+');
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
	wrefresh(win.cmdwin);
	wrefresh(win.offsetwin);
	
	print_file(win, filename);
	input_loop(win);
}

void print_file(struct editor win, char *file){
	int x, y;
	getmaxyx(win.mainwin, y, x);
	int fsize;
	FILE *f = fopen(file, "r");
	fseek(f, 0L, SEEK_END);
	fsize = ftell(f);
	rewind(f);
	char *mwinbuff = malloc(fsize);
	int r = fsize > (x*y) ? x*y : fsize;
	fread(mwinbuff, 1, fsize, f);
	int c, l=2,q=2, pos=0;
	wmove(win.mainwin, 1, 0);
	wmove(win.offsetwin, 1, 0);
	wmove(win.asciiwin, 1, 0);
	for (c = 0; c < r; c++){
		if (pos == 0){
			wprintw(win.offsetwin, "%07x ", c);
			wmove(win.offsetwin, l, 0);
			l++;
		}
		wprintw(win.mainwin, "%02x ", mwinbuff[c]);
		wprintw(win.asciiwin, "%c", 
			mwinbuff[c] < 32 || mwinbuff[c] == 127 ? '.' : mwinbuff[c]);
		pos += 3;
		if (pos + 3 > x){
			pos = 0;
			wmove(win.mainwin, q, 0);
			wmove(win.asciiwin, q, 0);
			q++;
		}
	}
	wrefresh(win.offsetwin);
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
}

void input_loop(struct editor win){
	WINDOW *activewin = win.mainwin;
	wmove(activewin, 1, 1);
	int xpos = 1, ypos = 1, key;
	while(true){
		key = wgetch(activewin);
		switch(key){
			case KEY_RIGHT:
				xpos++;
				goto move;
			case KEY_LEFT:
				xpos--;
				goto move;
			case KEY_UP:
				ypos--;
				goto move;
			case KEY_DOWN:
				ypos++;
				goto move;
move:
				wmove(activewin, ypos, xpos);
			break;
			case 'q':
				return;
			case 'd':
				break;
			//default:
			//	if (!strcmp(key_name(key), "^C")){
			//		return;
			//	}	
		}
		wrefresh(activewin);
	}
}

