#ifndef STDLIB_DEFS
#define STDLIB_DEFS 1
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#endif

//file buffer  structure
//fname	-- user-provided filename
//buf	-- data buffer for data display
//file	-- file obj from opened stream
//size	-- size of file
struct file_buffer {
	char *fname;
	char *buf;
	FILE *file;
	long size;
};
//dynamically allocate file buffer structure
struct file_buffer *alloc_file_buff(char *);
//dealloc only the contents of the file buffer struct,
//but not the struct itself
void dealloc_file_buff(struct file_buffer*);
//dealloc the entire struct's contents, and then itself
void full_dealloc_file_buff(struct file_buffer *);
