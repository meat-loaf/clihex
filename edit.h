#define LOCAL_EDIT_HEADER

#ifndef LOCAL_FILES_HEADER
#include "files.h"
#endif

#ifndef LOCAL_GENERAL_HEADER
#include "general.h"
#endif
#define ISVALIDHEX(X) (((X)>='0' && (X)<='9') || ((X)>='A' && (X)<='F') || ((X)>='a' && (X)<='f'))

#define TOLOWER(X) ((X)-(97-65))
struct edit_entry {
	//new value
	char val;
	//value originally in file
	char orig;
	//position in file (i.e. 'asciiwin' position)
	int pos;
	struct edit_entry *left, *right;
};
//TODO this should probably be a tree;
//we need to search to see if a certain byte
//has been changed already...
struct edits {
	struct edit_entry *root;
};

int handle_general(char, int, struct file_buffer *, int);
void init_edits(struct file_buffer* );
