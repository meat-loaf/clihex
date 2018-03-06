#ifndef LOCAL_GUI_HEADER
#define LOCAL_GUI_HEADER 1
#include "gui.h"
#endif
void
init_editor_struct_byref(struct editor *win){
	//we print in sets of 8 bytes + 2 spaces in main window
	//ascii win is just raw
	//so 10 characters in mainwin equals 4 bytes in asciiwin
	//(COLS-8) = 10x+4x
	//		   = 14x
	//(COLS-8)/14) = x
	int pspace = COLS-8;
	int scalar = pspace/14;
	//the rest is 2char mainwin per char asciiwin
	// remainder = 2x+x
	// 			 = 3x
	// remainder/3 = x
	int left = (pspace%14)/3;
	win->mainwin = newwin(LINES-1, (scalar*10)+(left*2), 0, 8);
	win->asciiwin = newwin(LINES-1, scalar*4+left+1, 0, 1+8+(scalar*10)+(left*2));
//	win->mainwin = newwin(LINES-1, (COLS-8)*3/4, 0, 8);
//	win->asciiwin = newwin(LINES-1, ((COLS-8)*1/4), 0, ((COLS-8)*3/4)+8);	
	win->offsetwin = newwin(LINES-1, 8, 0, 0);
	win->cmdwin = newwin(1, COLS, LINES-1, 0);
	win->pos_s = malloc(sizeof(struct positions));
	win->pos_s->line_length = 0;
	win->pos_s->last_segment_len = 0;
	win->pos_s->fbyte_pos = 0;
	return;
}
void 
editor_entry(char *filename){
	struct editor win;
	init_editor_struct_byref(&win);
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
	print_file(win, f);
	input_loop(win, f);
	//TODO if we're here we should probably clean up malloc'd stuff
	//before returning and exiting
	return;
}
void
print_file(struct editor win, struct file_buffer *file){
	int x, y;
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
				win.pos_s->line_length += p;
			}
			p = 0;
		}
		if ((pos + 2) > x){
			//if not line 1 and no cutoff 
			if (q == 0 && p != 0){
				win.pos_s->line_length += p;
				win.pos_s->last_segment_len = p;
				win.pos_s->last_x_pos = pos-1;
			}
			pos = 0;
			p = 0;
			q++;
			wmove(mwin, q, 0);
		}
	}
	win.pos_s->fbyte_pos = pos;
	win.pos_s->last_y_pos = q;
	wrefresh(win.offsetwin);
	wrefresh(mwin);
	wrefresh(win.asciiwin);
	return;
}

void
input_loop(struct editor win, struct file_buffer* currfile){
	WINDOW *activewin = win.mainwin;
	WINDOW *passivewin = win.asciiwin;
	int xmax = win.pos_s->last_x_pos,
		ymax = win.pos_s->last_y_pos;
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
			case KEY_HOME:
				xpos = 0;
				space = 0;
				goto move;
			case KEY_END:
				xpos = xmax;
//				space = (win.pos_s->last_segment_len*2)-1;
				goto move;
			case '\t':
				//TODO its broken
				if (activewin == win.mainwin){
					activewin = win.asciiwin;
					//account for the spaces in our position mapping
					xpos = (xpos+(xpos/10))/2;
				} else {
					activewin = win.mainwin;
					xpos = (xpos*2)-(xpos/4);
				}
				break;
			//TODO switch to asciiwin on tab
			case 'q':
				return;
		}
move:
		//skip the spacing in the main window
		if (activewin == win.mainwin &&
			(xpos % 10) > 7){
			if (oldx < xpos)
				xpos += 2;
			else xpos -= 2;
		} 
		if (xpos > xmax || xpos < 0){
			if (xpos > xmax){
				if (ypos != ymax){
					ypos++;
					xpos = 0;
				}
				else {
					xpos = oldx; 
				}
			}
			else if (xpos < 0){
				xpos = xmax;
				if (ypos != 0)
					ypos--;
			}
		}
		wmove(activewin, ypos, xpos);
		wmove(win.cmdwin, 0, 0);
		wprintw(win.cmdwin, "linelen: %d lastseglen: %d fbpos: %d lastxpos: %d  pos: %d, %d bounds: %d, %d LINES: %d COLS: %d      ",
			win.pos_s->line_length, win.pos_s->last_segment_len, win.pos_s->fbyte_pos, win.pos_s->last_x_pos, xpos, ypos, xmax, ymax, LINES, COLS);
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
end:
		wrefresh(win.cmdwin);
		wrefresh(activewin);
	}
}
