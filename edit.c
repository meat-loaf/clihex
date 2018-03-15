#ifndef LOCAL_EDIT_HEADER
#define LOCAL_EDIT_HEADER
#include "edit.h"
#endif
#ifndef LOCAL_FILES_HEADER
#define LOCAL_FILES_HEADER
#include "files.h"
#endif
//which defines if we edited in a raw character or a nibble
static struct bst_node_w edits;

void init_edits(){
	edits->entry = NULL;
};

void handle_general(char key, struct editor win, struct file_buffer *f, int which){
	if (ISVALIDHEX(key)){
		if (key >= 'A' || key <= 'F')
			key = TOLOWER(key)
	} else {
		//wasn't valid edit key
	}	
}
