#include "lsmtree.h"

/*
SECTION 1: functions for initializing, creating, loading, maintaining, and saving the LSM tree.
*/

lsmtree *init_lsmtree(void) {
	/* 
	Initializes empty LSM tree.
	*/
	lsmtree *tree = malloc(sizeof(lsmtree));
	return tree;
}

void empty_lsmtree(lsmtree *tree, size_t capacity) {
	/* 
	Creates new empty LSM tree.
	*/
	// initialize buffer
	tree->buff = malloc(sizeof(buffer));
	tree->buff->capacity = capacity;
	tree->buff->keys = calloc(tree->buff->capacity, sizeof(KEY_TYPE));
	tree->buff->vals = calloc(tree->buff->capacity, sizeof(VAL_TYPE));

	// initialize tree
	tree->pairs_per_level = calloc(16, sizeof(size_t)); // 16 levels, dynamic
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

void flush_lsmtree(lsmtree *tree) {
	/* 
	Flushes buffer to L1.
	*/
}


/*
SECTION 2: functions for querying LSM tree.
*/

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val) {
	if (tree->pairs_per_level[0] < tree->buff->capacity) {
		tree->buff->keys[tree->pairs_per_level[0]] = key;
		tree->buff->vals[tree->pairs_per_level[0]] = val;

		tree->pairs_per_level[0]++;
	}
	else {
		// flush buffer to L1
		flush_lsmtree(tree);

		// now space in buffer; call put again
		put(tree, key, val);
	}
}

VAL_TYPE get(lsmtree *tree, KEY_TYPE key) {
	return tree->buff->vals[key];
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


