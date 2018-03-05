#ifndef LOCAL_GUI_HEADER
#define LOCAL_GUI_HEADER 1
#include "gui.h"
#endif

void 
editor_entry(char *filename){
	struct editor win;
	//TODO make this mess nicer (?)
	win.mainwin = newwin(LINES-1, (COLS-8)*3/4, 0, 8);
	win.line_length = 0;
	win.asciiwin = newwin(LINES-1, ((COLS-8)*1/4), 0, ((COLS-8)*3/4)+8);
	win.offsetwin = newwin(LINES-1, 8, 0, 0);
	win.cmdwin = newwin(1, COLS, LINES-1, 0);
	curs_set(1);
	keypad(win.mainwin, TRUE);
	keypad(win.asciiwin, TRUE);
	keypad(win.offsetwin, TRUE);
	keypad(win.cmdwin, TRUE);
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
	wrefresh(win.cmdwin);
	wrefresh(win.offsetwin);
	
	struct file_buffer *f = alloc_file_buff(filename);
	win.line_length = print_file(win, f);
	input_loop(win, f);
	//TODO if we're here we should probably clean up malloc'd stuff
	//before returning and exiting
}
/* returns number of characters printed in a single line */
void
print_file(struct editor win, struct file_buffer *file){
	int x, y;
	int ll = 0;
	WINDOW *mwin = win.mainwin;
	getmaxyx(mwin, y, x);
	int r = file->size > (x*y) ? x*y : file->size;
	fread(file->buf, 1, file->size, file->file);
	int c, l=1,q=0, pos = 0, p=0;
	wmove(mwin, 0, 0);
	wmove(win.offsetwin, 0, 0);
	wmove(win.asciiwin, 0, 0);
	for (c = 0; c < r; c++){
		if (pos == 0){
			wprintw(win.offsetwin, "%7x ", c);
			wmove(win.offsetwin, l, 0);
			l++;
		}

		wprintw(mwin, "%02x", file->buf[c]);
		wprintw(win.asciiwin, "%c", 
			file->buf[c] < 32 || file->buf[c] == 127 ? '.' : file->buf[c]);
		p++;
		pos += 2;
		if (p == 4){
			wprintw(mwin, "  ");
			pos += 2;
			//when we move to the next line, stop computing length	
			if (q == 0){
				ll += p;
				//wprintw(win.cmdwin, "ll: %d ", win.line_length);
				//wrefresh(win.cmdwin);
			}
			p = 0;
		}
		if ((pos + 2) > x){ 
			if (q == 0){
				ll += p;
			}
			pos = 0;
			p = 0;
			q++;
			wmove(mwin, q, 0);
		}
	}
	wrefresh(win.offsetwin);
	wrefresh(mwin);
	wrefresh(win.asciiwin);
	return ll;
}

void input_loop(struct editor win, struct file_buffer* currfile){
	WINDOW *activewin = win.mainwin;
	int xmax, ymax;
	getmaxyx(activewin, ymax, xmax);
	int xpos = 0, ypos = 0, key;
	int oldx, oldy;
	int space = 0;
//	mvwchgat(activewin, ypos, xpos, 2, A_STANDOUT, 0, NULL);
	while(true){
		key = wgetch(activewin);
		oldx=xpos;
		oldy=ypos;
		switch(key){
			case KEY_RIGHT:
				space++;
				xpos++;
				goto move;
			case KEY_LEFT:
				space--;
				xpos--;
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
		//TODO this is broken aaaaaaaaah
		if (xpos > xmax || xpos < 0){
			if (xpos > xmax){
				xpos = 0;
				if (ypos != ymax){
					space = 0;
					ypos++;
				}
			}
			else if (xpos < 0){
				xpos = xmax;
				if (ypos != 0)
					space = 7;
					ypos--;
			}
			//figure out which bound was violated
			//TODO want to hop to the next line if available
			/*xpos = xpos > xmax ? xmax
			: xpos < 0 ? 0
			: xpos;
			ypos = ypos > ymax ? ymax
			: ypos < 0 ? 0
			: ypos;*/
		}
		else if (space > 7 || space < 0){
			if (space > 7){ space = 0; xpos += 2; }
			else {space = 7; xpos -=2;}
		}
		wmove(activewin, ypos, xpos);
		wmove(win.cmdwin, 0, 0);
		wprintw(win.cmdwin, "linelen: %d pos: %d, %d bounds: %d, %d",
			win.line_length, xpos, ypos, xmax, ymax);
		//dont care about this if we violated a bound
/*		else {
			if (currfile->buf[xpos*+(ymax*ypos)] == ' '){
				//can't encounter a spacer vertically
				xpos = xpos > oldx ? xpos + 2 : xpos - 2;
			}
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
		}*/
		wrefresh(win.cmdwin);
		wrefresh(activewin);
	}
}
