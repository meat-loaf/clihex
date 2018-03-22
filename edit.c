#include <ncurses.h>

#ifndef LOCAL_EDIT_HEADER
#include "edit.h"
#endif

#define DEFAULT_SIZE 32
//which defines if we edited in a raw character or a nibble
static void resize_edits();

static void resize_edits(struct edits *e){
	NCURSES_EXIT(1);
}

static struct edit_entry *edit_contains(int pos, struct edits *ed){
	int c;
	for (c = 0; c < ed->size; c++){
		if (ed->vals[c].pos == pos)
			return &(ed->vals[c]);
	}
	return NULL;
}

void init_edits(struct file_buffer *f){
	void *mem;
	ALLOCMEM(mem, (sizeof(struct edits)));
	f->edits = mem;
	ALLOCMEM(mem, (sizeof(struct edit_entry)*DEFAULT_SIZE));
	f->edits->vals = mem;
	f->edits->maxsize = DEFAULT_SIZE;
	f->edits->size = 0;
	return;
};

static void insert_into_edits(char k, int p, struct edits *ed){
	struct edit_entry *ee = edit_contains(p, ed);
	if (ee){
		ee->e = k;	
	} else {
		int s = ++(ed->size);
		if (ed->size > ed->maxsize)
			resize_edits(ed);
		ed->vals[s].e = k;
		ed->vals[s].pos = p;
	}
	return;
}

int handle_general(char key, int pos, struct file_buffer *f, int which){
	if (!which && ISVALIDHEX(key)){
		if (key >= 'A' || key <= 'F')
			key = TOLOWER(key);
	} else {
		//wasn't valid edit key for nibble
	}
	//asciiwin -- we entered a byte
	if (which){
		insert_into_edits(key, pos, f->edits);
	} else { //we entered a nibble
	}
	return f->edits->size;
}
