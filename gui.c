#ifndef LOCAL_GUI_HEADER
#define LOCAL_GUI_HEADER 1
#include "gui.h"
#endif

#include <signal.h>
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
	win->mainwin = newwin(LINES-1, (scalar*10)+(left*2)-2, 0, 8);
	win->asciiwin = newwin(LINES-1, scalar*4+left-1, 0, 8+(scalar*10)+(left*2));
	//TODO this needs to be more dynamic...can make it smaller
	//based on filesize
	win->offsetwin = newwin(LINES-1, 8, 0, 0);
	win->cmdwin = newwin(1, COLS, LINES-1, 0);
	win->pos_s = malloc(sizeof(struct positions));
	win->pos_s->line_length = 0;
	win->pos_s->last_segment_len = 0;
	win->pos_s->fbyte_pos = 0;
	win->pos_s->printed_chars = 0;
	win->pos_s->lines_into_file = 0;
	win->pos_s->at_end = 0;
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
	print_file(win, f, 0);
	input_loop(win, f);
	//TODO if we're here we should probably clean up malloc'd stuff
	//before returning and exiting
	return;
}
//dump the contents of the file's buffer (starting from print_start_pos relative
//to beginning of file) to the screen. updates 'positions' struct with important
//information relating to where we are in the file
void
print_file(struct editor win, struct file_buffer *file, int print_start_pos){
	int x, y, c, d;
	int num = 0;
	win.pos_s->at_beginning = !print_start_pos;
	getmaxyx(win.asciiwin, y, x);
	win.pos_s->line_length = x;
	//check if the (rest of the) file is larger than the terminal window...
	//TODO doesn't work when scrolling
	int toprint = (file->size)-(print_start_pos) > (x*y) ?
					 x*y : (file->size)-(print_start_pos);
	win.pos_s->at_end = toprint < x*y? 1 : 0;
	win.pos_s->printed_chars = toprint;
	//seek to the position provided
	//TODO error handling on -1 return val
	fseek(file->file, print_start_pos, SEEK_SET);
	//TODO error handling
	fread(file->buf, 1, file->size, file->file);
	wmove(win.asciiwin, 0, 0);
	wmove(win.offsetwin, 0, 0);
	wmove(win.asciiwin, 0, 0);
	for (d = 0; d*x < toprint; d++){
		wmove(win.offsetwin, d, 0);
		wprintw(win.offsetwin, "%7x", (d*x)+print_start_pos);
		wmove(win.asciiwin, d, 0);
		wmove(win.mainwin, d, 0);
		for(c = 0; d*x+c < toprint; c++){
			wprintw(win.mainwin, "%02x", file->buf[d*x+c] & 0xFF);
			wprintw(win.asciiwin, "%c", file->buf[d*x+c] < 32
										  || file->buf[d*x+c] == 127 ?
										  '.' : file->buf[d*x+c]
			);
			num++;
			if (num == 4){
				wprintw(win.mainwin, "  ");
				num = 0;
			}
		}
		num = 0;
	}
	if (win.pos_s->at_end){
		wprintw(win.mainwin, "%*c", ATOM(x-c), ' ');
		wprintw(win.asciiwin, "%*c", x-c, ' ');
	}
	win.pos_s->last_y_pos = d;
	c = ATOM(c-1);
	if ((c%2) == 1){ c--; }
	c++;
	win.pos_s->last_x_pos = c;
	wrefresh(win.offsetwin);
	wrefresh(win.mainwin);
	wrefresh(win.asciiwin);
	return;
}

void
input_loop(struct editor win, struct file_buffer* currfile){
	WINDOW *activewin = win.mainwin;
	WINDOW *passivewin = win.asciiwin;
	//TODO update these properly on screen scroll
	int xmax,ymax = win.pos_s->last_y_pos;
	//TODO off by one
	ymax--;
	xmax = getmaxx(activewin);

	//TODO again, off by one?
	xmax--;
	wmove(activewin, 0, 0);
	wrefresh(activewin);
	
	int xpos = 0, ypos = 0, key;
	int oldx, oldy;
//	mvwchgat(activewin, ypos, xpos, 2, A_STANDOUT, 0, NULL);
	while(true){
		int finalx = win.pos_s->last_x_pos;
		key = wgetch(activewin);
		oldx=xpos;
		oldy=ypos;
		switch(key){
			case KEY_RIGHT:
				xpos++;
				break;
			case KEY_LEFT:
				xpos--;
				break;
			case KEY_UP:
				ypos--;
				break;
			case KEY_DOWN:
				ypos++;
				break;
			case KEY_HOME:
				xpos = 0;
				break;
			case KEY_END:
				xpos = xmax;
				break;
			case '\t':
				if (activewin == win.mainwin){
					activewin = win.asciiwin;
					//account for the spaces in our position mapping
					xpos = MTOA(xpos);
					//update maxim
					xmax = MTOA(xmax);
					finalx = MTOA(finalx);
				} else {
					activewin = win.mainwin;
					//simply inverse of above
					xpos = ATOM(xpos);
					//TODO mapping aint perfect, fix
					if ((xpos %2) == 1){xpos--;}
					xmax = ATOM(xmax);
					finalx = ATOM(finalx);
					if ((finalx%2)==0)
						finalx++;
				}
				break;
			case 'q':
				return;
		}
		//skip the spacing in the main window
		if (activewin == win.mainwin
			&& SPACE_CHECK_BOOL(xpos)){
			if (oldx < xpos)
				xpos += 2;
			else xpos -= 2;
		}
		
		if (xpos > xmax){
			if (ypos != ymax || !win.pos_s->at_end){
				ypos++;
				xpos = 0;
			} else {
				xpos = oldx; 
			}
		}
		else if (xpos < 0){
			if (ypos != 0 || !win.pos_s->at_beginning){
				ypos--;
				xpos = xmax;
			} else { xpos = 0; }
		}
		//TODO check logic for scrolling
		if (ypos == ymax
			&& xpos > finalx){
			if (key == KEY_END)
				xpos = finalx;
			else if (oldx > finalx) 
				xpos = finalx;
			else xpos = oldx;
		} else if (ypos < 0){
			if (win.pos_s->lines_into_file != 0){
				win.pos_s->lines_into_file--;
				print_file(win, currfile, 
					GET_NEXT_FILE_OFFSET(win)
				);

			}
			ypos = oldy;
		} else if (ypos > ymax && !win.pos_s->at_end){
			if (win.pos_s->printed_chars != currfile->size){
				win.pos_s->lines_into_file++;
				print_file(win, currfile, 
					GET_NEXT_FILE_OFFSET(win)
				);
			}
			finalx = win.pos_s->last_x_pos;
			ypos = oldy;
			if (xpos > finalx) xpos = finalx;
		} else if (ypos > ymax && win.pos_s->at_end){
			ypos = oldy;
		}
		wmove(activewin, ypos, xpos);
		wmove(win.cmdwin, 0, 0);
		wprintw(win.cmdwin, "printed_chars: %d, lastseglen: %d fbpos: %d  finalx: %d, pos: %d, %d bounds: %d, %d LINES: %d COLS: %d at_end: %s at_beginning: %s     ",
			  win.pos_s->printed_chars, win.pos_s->last_segment_len, win.pos_s->fbyte_pos, finalx, xpos, ypos, xmax, ymax, LINES, COLS, win.pos_s->at_end ? "true" : "false", win.pos_s->at_beginning? "true" : "false");
		wrefresh(win.cmdwin);
		wrefresh(activewin);
	}
}
