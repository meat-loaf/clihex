#ifndef STDLIB_DEFS
#define STDLIB_DEFS 1
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifndef LOCAL_FILES_HEADER
#include "files.h"
#endif

#ifndef _LOCAL_LIB_NCURSES_DEF
#define _LOCAL_LIB_NCURSES_DEF 1
#include <curses.h>
#endif

#define MTOA(X) (((X)/2)-((X)/10))
#define ATOM(X) (((X)*10)/4)
#define SPACE_CHECK_BOOL(X) (((X)%10)>7)
#define GET_NEXT_FILE_OFFSET(WIN) ((WIN.pos_s->line_length)*(WIN.pos_s->lines_into_file))
//TODO clean me up...
struct positions {
	//number of bytes we actually printed on the screen
	//good to know when file is smaller than screen
	int printed_chars;
	//number of bytes printed per line
	int line_length;
	//how many 'lines deep' we are into the file (line being
	//a line on the screen
	int lines_into_file;
	//number of bytes in last 'segment', if applicable
	int last_segment_len;
	//position in window of last nibble
	int last_x_pos;
	//how many lines we ended up printing
	//(if file is small enough to not fill window)
	int last_y_pos;
	//position in last line of final byte
	//(relative to window printing)
	int at_end;
	int at_beginning;
};
//TODO could use some dealloc functions
struct editor {
	WINDOW *mainwin;
	WINDOW *asciiwin;
	WINDOW *offsetwin;
	WINDOW *cmdwin;
	struct positions *pos_s;
};

void init_editor_struct_byref(struct editor *);
void editor_entry(char *);
void input_loop(struct editor, struct file_buffer*);
void print_file(struct editor, struct file_buffer*, int);
