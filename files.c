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

//TODO check malloc return values
struct file_buffer *alloc_file_buff(char *fn){
	struct file_buffer *f = malloc(sizeof(struct file_buffer));
	f->fname = fn;
	f->file = fopen(fn, "r");
	if (f->file == NULL){ return NULL; }
	fseek(f->file, 0L, SEEK_END);
	f->size = ftell(f->file);
	//TODO probably an issue with large files
	f->buf = malloc(f->size);
	rewind(f->file);
	return f;
}
void full_dealloc_file_buff(struct file_buffer *fb){
	if (fb == NULL)
		return;
	dealloc_file_buff(fb);
	free(fb);
}

void dealloc_file_buff(struct file_buffer* fb){
		fclose(fb->file);
		if (fb->buf != NULL)
			free(fb->buf);
		return;
}

int set_file_pos_from_start(struct file_buffer *fb, long pos){
	return fseek(fb->file, pos, SEEK_SET);
}
