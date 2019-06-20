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

/*static struct edit_entry *edit_contains(int pos, struct edits *ed){
	int c;
	for (c = 0; c < ed->size; c++){
		if (ed->vals[c].pos == pos)
			return &(ed->vals[c]);
	}
	return NULL;
}*/

void init_edits(struct file_buffer *f){
	struct edits*mem;
	ALLOCMEM(mem, (sizeof(struct edits)));
	f->edits = mem;
	mem->root = NULL;
	/*ALLOCMEM(mem, (sizeof(struct edit_entry)*DEFAULT_SIZE));
	f->edits->root = mem;
	mem->left = NULL;
	mem->right = NULL;*/
	return;
};

static void insert_into_edits(char k, char old, int p, struct edits *ed){
	/*if (ed->root == NULL){
		struct edit_entry *mem;
		ALLOCMEM(mem, sizeof(struct edit_entry));
		mem->left = mem->right = NULL;
		mem->val = k;
		mem->old = old;
		mem->pos = p;
		ed->root = mem;
	}
	else {
		if (ed->root->val > k){
			; //insert to right
		}
		else { ; //insert to left
		}
	}*/
	return;
}

int handle_general(char key, int pos, struct file_buffer *f, int which){
	if (!which && ISVALIDHEX(key)){
		if (key >= 'A' || key <= 'F')
			key = TOLOWER(key);
		/*	insert_into_edits((f->buf[pos] & 0xF0)|(key-97), 
				pos, f->edits);*/
	} else {
		//wasn't valid edit key for nibble
		return 0;
	}
	//asciiwin -- we entered a byte
	if (which){
		insert_into_edits(key, pos, f->buf[pos], f->edits);
	} else { //we entered a nibble
	}
	return -1;

}
