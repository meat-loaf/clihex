#ifndef INC_NCURSESW_LIBS
#define INC_NCURSESW_LIBS 1
#include <ncursesw/curses.h>
#endif


#include <locale.h>
#include <stdlib.h>
#include <stdio.h>

//TODO move structs to header file
struct editor {
	WINDOW *mainwin;
	WINDOW *asciiwin;
	WINDOW *offsetwin;
	WINDOW *cmdwin;	
};

struct file_buffer {
	char *fname;
	char *buf;
	FILE *file;
	int size;
};

struct file_buffer *alloc_file_buff(char*);

void editor_entry();
struct file_buffer *print_file(struct editor, char *);
void input_loop(struct editor);
void dealloc_file_buff(struct file_buffer *);
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

//ready for ncurses
void 
editor_entry(char *filename){
	struct editor win;
	//TODO make this mess nicer
	win.mainwin = newwin(LINES-1, (COLS-8)*3/4, 0, 8);
	win.asciiwin = newwin(LINES-1, ((COLS-8)*1/4), 0, ((COLS-8)*3/4)+8);
	win.offsetwin = newwin(LINES-1, 8, 0, 0);
	win.cmdwin = newwin(1, COLS, LINES-1, 0);
	keypad(win.mainwin, TRUE);
	keypad(win.asciiwin, TRUE);
	keypad(win.offsetwin, TRUE);
	keypad(win.cmdwin, TRUE);
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
	wrefresh(win.cmdwin);
	wrefresh(win.offsetwin);
	
	print_file(win, filename);
	input_loop(win);
}
struct file_buffer *alloc_file_buff(char *fn){
	struct file_buffer *f = malloc(sizeof(struct file_buffer));
	f->fname = fn;
	f->file = fopen(fn, "r");
	fseek(f->file, 0L, SEEK_END);
	f->size = ftell(f->file);
	//TODO probably an issue with large files
	f->buf = malloc(f->size);
	rewind(f->file);
	return f;
}
void full_dealloc_file_buff(struct file_buffer *fb){
	dealloc_file_buff(fb);
	free(fb);
}

void dealloc_file_buff(struct file_buffer* fb){
		fclose(fb->file);
		free(fb->buf);
		return;
}
struct file_buffer *
print_file(struct editor win, char *fname){
	//TODO move file alloc to own handler
	//shouldnt be done as side effect of printing
	struct file_buffer *file = alloc_file_buff(fname);
	int x, y;
	getmaxyx(win.mainwin, y, x);

	int r = file->size > (x*y) ? x*y : file->size;
	fread(file->buf, 1, file->size, file->file);
	int c, l=1,q=1, pos = 0, p=0;
	wmove(win.mainwin, 0, 0);
	wmove(win.offsetwin, 0, 0);
	wmove(win.asciiwin, 0, 0);
	for (c = 0; c < r; c++){
		if (pos == 0){
			wprintw(win.offsetwin, "%7x ", c);
			wmove(win.offsetwin, l, 0);
			l++;
		}

		wprintw(win.mainwin, "%02x", file->buf[c]);
		wprintw(win.asciiwin, "%c", 
			file->buf[c] < 32 || file->buf[c] == 127 ? '.' : file->buf[c]);
		p++;
		pos += 2;
		if (p == 4){
			wprintw(win.mainwin, "  ");
			pos += 2;
			p = 0;
		}
		if ((pos + 2) > x){ 
			pos = 0; 
			wmove(win.mainwin, q, 0);
			q++;
		}
	}
	wrefresh(win.offsetwin);
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
	return file;
}

void input_loop(struct editor win){
	WINDOW *activewin = win.mainwin;
	int xpos = 0, ypos = 0, key, bytepos=0;
	int oldx, oldy;
	mvwchgat(activewin, ypos, xpos, 2, A_STANDOUT, 0, NULL);
	while(true){
		key = wgetch(activewin);
		oldx=xpos;
		oldy=ypos;
		switch(key){
			case KEY_RIGHT:
				xpos++;
//				if (xpos%2 == 0) bytepos++;
				goto move;
			case KEY_LEFT:
				xpos--;
//				if (xpos%2 != 0) bytepos--;
				goto move;
			case KEY_UP:
				ypos--;
				goto move;
			case KEY_DOWN:
				ypos++;
				goto move;

			break;
			//TODO switch to asciiwin on tab
			case 'q':
				return;
		}
move:
		//TODO find better solution for highlighting byte (?)
/*		if (bytepos == 4){
			xpos += 2;
			bytepos = 0;
		}
		else if (bytepos == -1){
			xpos -=2;
			bytepos = 4;
		}
*/
		if (xpos % 2 == 0){
			if (oldy == ypos)
				mvwchgat(activewin, oldy, oldx-1, 2, A_NORMAL, 0, NULL);
			else
				mvwchgat(activewin, oldy, oldx, 2, A_NORMAL, 0, NULL);
			mvwchgat(activewin, ypos, xpos, 2, A_STANDOUT, 0, NULL);
		}
		else{
			if (oldy == ypos){
				mvwchgat(activewin, oldy, oldx, 2, A_NORMAL, 0, NULL);
				mvwchgat(activewin, ypos, xpos-1, 2, A_STANDOUT, 0, NULL);
				wmove(activewin, ypos, xpos);
			}
			else{ 
				mvwchgat(activewin, oldy, oldx-1, 2, A_NORMAL, 0, NULL);
				mvwchgat(activewin, ypos, xpos-1, 2, A_STANDOUT, 0, NULL);
				wmove(activewin, ypos, xpos);
			}
		}
		wrefresh(activewin);
/*		wchgat(activewin, 1, A_NORMAL, 0, NULL);
		wchgat(activewin, 1, A_STANDOUT, 0, NULL);
*/
	}
}

