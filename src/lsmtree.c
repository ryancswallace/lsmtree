#include "lsmtree.h"

/*
SECTION 1: functions for initializing, creating, loading, and saving the LSM tree.
*/

lsmtree *init_lsmtree(void) {
	/* 
	Initializes empty LSM tree.
	*/
	lsmtree *tree = malloc(sizeof(lsmtree));
	return tree;
}

void empty_lsmtree(lsmtree *tree) {
	/* 
	Creates new empty LSM tree.
	*/
}

int load_lsmtree(lsmtree *tree) {
	/* 
	Loads existing serialized LSM tree from disk.
	*/
	return 0;
}

void free_lsmtree(lsmtree *tree) {
	/* 
	Cleans up after LSM tree, freeing all data from memory.
	*/
}

void serialize_lsmtree(lsmtree *tree) {
	/* 
	Serializes memory resident LSM tree data to disk.
	*/
}


/*
SECTION 2: functions for querying LSM tree.
*/

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val) {

}

VAL_TYPE get(lsmtree *tree, KEY_TYPE key) {
	return 0;
}

KEY_TYPE *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop) {
	return 0;
}

void delete(lsmtree *tree, KEY_TYPE key) {

}

void load(lsmtree *tree, char *filename) {

}

void print_stats(lsmtree *tree) {

}


