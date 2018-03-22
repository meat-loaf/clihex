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
	char e;
	//position in file (i.e. 'asciiwin' position)
	int pos;
};
//TODO this should probably be a tree;
//we need to search to see if a certain byte
//has been changed already...
struct edits {
	int size;
	int maxsize;
	struct edit_entry *vals;
};

int handle_general(char, int, struct file_buffer *, int);
void init_edits(struct file_buffer* );
