//STDLIB Includes
#include <signal.h>

//Local File Includes
#ifndef LOCAL_GUI_HEADER
#define LOCAL_GUI_HEADER
#include "gui.h"
#endif

#ifndef LOCAL_EDIT_HEADER
#include "edit.h"
#endif

#ifndef LOCAL_GENERAL_HEADER
#include "general.h"
#endif

//TODO make drawing 
static int winch_refresh_scr = 0;
void winch_handler(int sig){
	winch_refresh_scr = 1;	
}

void
init_editor_struct_byref(struct editor *win){
	//we print in sets of 8 chars + 2 spaces in main win
	//ascii win is just raw data
	//so 10 chars in mainwin equals 4 chars in asciiwin
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
	win->offsetwin = newwin(LINES-1, 8, 0, 0);
	win->cmdwin = newwin(1, COLS, LINES-1, 0);
	win->pos_s = malloc(sizeof(struct positions));
	win->pos_s->line_length = 0;
	win->pos_s->last_segment_len = 0;
	win->pos_s->printed_chars = 0;
	win->pos_s->lines_into_file = 0;
	win->pos_s->at_end = 0;
	return;
}
void 
editor_entry(char *filename){
	//init_edits();
	struct editor win;
	//TODO want input handling on separate thread for this to work
	signal(SIGWINCH, winch_handler);
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
	init_edits(f);
	//couldn't open file
	if (!f) return;
	//TODO win should be passed as pointer
	print_file(win, f, 0);
	input_loop(win, f);
	//cleanup on exit
	full_dealloc_file_buff(f);
	if (win.pos_s)
		free(win.pos_s);
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
	int toprint = (file->size)-(print_start_pos) > (x*y) ?
					 x*y : (file->size)-(print_start_pos);
	win.pos_s->at_end = toprint < x*y? 1 : 0;
	win.pos_s->printed_chars = toprint;
	//seek to the position provided
	//TODO error handling on -1 return val (file becomes invalid?)
	fseek(file->file, print_start_pos, SEEK_SET);
	//TODO error handling...
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
			//TODO add space here and fix maths
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
				//xpos = finalx;
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
			//ESC or ALT
			case 27:
				return;
			default:
				wmove(win.cmdwin, 0, 0);
				wprintw(win.cmdwin, "calling handle_general with value '%c'", key);
				if (activewin == win.mainwin){
					wprintw(win.cmdwin, "; returned %d, inserted at position %x ", handle_general(key, MTOA(xpos)+(MTOA(ypos)*win.pos_s->line_length), currfile,
					activewin == win.mainwin? 0 : 1), MTOA(xpos)+MTOA(ypos)*win.pos_s->line_length);
				}
				else {
					wprintw(win.cmdwin, "; returned %d, inserted at position %x ", handle_general(key, MTOA(xpos)+(MTOA(ypos)*win.pos_s->line_length), currfile,
					activewin == win.mainwin? 0 : 1), xpos+(ypos*win.pos_s->line_length));
				}
				wmove(activewin, ypos, xpos);
				break;

		}
		ensure_new_position_valid(win,
			&xpos, &ypos, oldx, oldy,
			activewin == win.mainwin ? 1 : 0, finalx,
			xmax, ymax, key, currfile
		);
		//skip the spacing in the main window
		wmove(activewin, ypos, xpos);
		//TODO finish highlighting logic
		/*if (activewin == win.mainwin){
			unsigned int moved = 0;
			if (xpos & 1){
				moved = 1;
				xpos--;
				wmove(activewin, ypos, xpos);
			}
			if (oldx != xpos){
				wmove(activewin, oldy, oldx & 1 ? oldx-1 : oldx);
				wchgat(activewin, 2, A_NORMAL, 0, NULL);
				wmove(activewin, ypos, xpos);
			}
			wchgat(activewin, 2, A_REVERSE, 0, NULL);
			//wmove(win.asciiwin, MTOA(xpos), MTOA(ypos));
			wmove(win.asciiwin, MTOA(ypos), MTOA(xpos));
			wchgat(win.asciiwin, 1, A_REVERSE, 0, NULL);
			if (moved){
				xpos++;
				wmove(activewin, ypos, xpos);
			}
		}
		else if (activewin == win.asciiwin){
			wchgat(win.mainwin, 2, A_REVERSE, ATOM(xpos), NULL);
		}*/
		wrefresh(win.cmdwin);
		//wrefresh(activewin == win.mainwin ? win.asciiwin : win.mainwin);
		wrefresh(activewin);
	}
}

//TODO should return a coord_pair with valid new positions instead
void
ensure_new_position_valid(
	struct editor win,
	int* xpos, int* ypos,
	int oldx, int oldy,
	bool skipspace, int finalx,
	int xmax, int ymax, char key, struct file_buffer *currfile){
	if (skipspace && SPACE_CHECK_BOOL(*xpos)){
		if (oldx < *xpos)
			*xpos += 2;
		else *xpos -= 2;
	}

	if (*xpos > xmax){
		if ((*ypos != ymax || !win.pos_s->at_end)){
			*ypos++;
			*xpos = 0;
		} else {
			*xpos = oldx; 
		}
	}
	else if (*xpos < 0){
		if (*ypos != 0 || !win.pos_s->at_beginning){
			*ypos--;
			*xpos = xmax;
		} else { *xpos = 0; }
	}
	//TODO check logic for scrolling
	if (*ypos == ymax
		&& *xpos > finalx){
		if (key == KEY_END)
			*xpos = finalx;
		else if (oldx > finalx) 
			*xpos = finalx;
		else *xpos = finalx;
		/*if (*xpos > finalx){
			*xpos = finalx;
		}*/
		//else *xpos = finalx;
	} else if (*ypos < 0){
		if (win.pos_s->lines_into_file != 0){
			win.pos_s->lines_into_file--;
			print_file(win, currfile, 
				GET_NEXT_FILE_OFFSET(win)
			);
			}
		*ypos = oldy;
	} else if (*ypos > ymax && !win.pos_s->at_end){
		if (win.pos_s->printed_chars != currfile->size){
			win.pos_s->lines_into_file++;
			print_file(win, currfile, 
				GET_NEXT_FILE_OFFSET(win)
			);
		}
		finalx = win.pos_s->last_x_pos;
		*ypos = oldy;
		if (*xpos > finalx) *xpos = finalx;
	} else if (*ypos > ymax && win.pos_s->at_end){
		*ypos = oldy;
	}
}

/*void
move_cursor (WINDOW *activewin, struct editor *wins, int x, int y, int oldx, int oldy){
	return;
}*/
