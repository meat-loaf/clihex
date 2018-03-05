#ifndef STDLIB_DEFS
#define STDLIB_DEFS 1
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifndef LOCAL_FILES_HEADER
#define LOCAL_FILES_HEADER 1
#include "files.h"
#endif

#ifndef _LOCAL_LIB_NCURSES_DEF
#define _LOCAL_LIB_NCURSES_DEF 1
#include <curses.h>
#endif

struct positions {
	//number of bytes printed per line
	int line_length;
	//number of bytes in last 'segment'
	int last_segment_len
}
struct editor {
	WINDOW *mainwin;
	WINDOW *asciiwin;
	WINDOW *offsetwin;
	WINDOW *cmdwin;
	struct positions;
};

void editor_entry(char *);
void input_loop(struct editor, struct file_buffer*);
void print_file(struct editor, struct file_buffer*);
