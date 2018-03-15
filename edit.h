#define ISVALIDHEX(X) (((X)>='0' && (X)<='9') || ((X)>='A' && (X)<='F') || ((X)>='a' && (X)<='f'))

#define TOLOWER(X) ((X)-(97-65))
struct bst_node {
	char val;
	struct bst_node *left, *right;
};

struct bst_node_w {
	struct bst_node* entry;
}
void init_edits();
void handle_general(char, struct editor, struct file_buffer);
