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

int empty_lsmtree(lsmtree *tree, char *name) {
	/* 
	Creates new empty LSM tree.
	*/
	// initialize buffer
	tree->buff = malloc(sizeof(buffer));
	tree->buff->capacity = BUFF_CAPACITY;
	tree->buff->keys = calloc(tree->buff->capacity, sizeof(KEY_TYPE));
	tree->buff->vals = calloc(tree->buff->capacity, sizeof(VAL_TYPE));

	// initialize tree
	tree->pairs_per_level = calloc(16, sizeof(size_t)); // 16 levels, dynamic
	tree->num_pairs = malloc(sizeof(int));
	tree->run_ctr = malloc(sizeof(int));

	*(tree->num_pairs) = 0;
	*(tree->run_ctr) = 0;

	// create directory for data
	char dir_name[MAX_DIR_LEN] = DATA_DIR;
	strcat(dir_name, name);

	struct stat status = {0};
	if (stat(dir_name, &status) == 0) {
		// directory already exist
		printf("LSM tree with given name already exists.\n");
		return 1;
	} 
    mkdir(dir_name, 0700);
    strcpy(tree->data_dir, dir_name);

	return 0;
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

int write_run(lsmtree *tree, run *new_run, KEY_TYPE *keys, VAL_TYPE *vals) {
	// writes a run to disk
	// first create file paths
	char keys_filepath[MAX_DIR_LEN];
	char vals_filepath[MAX_DIR_LEN];

	printf("num: %d\n", *(new_run->num));

	strcpy(keys_filepath, tree->data_dir);
	strcpy(vals_filepath, tree->data_dir);
	strcat(keys_filepath, "/run");
	strcat(vals_filepath, "/run");
	strcat(keys_filepath, new_run->num);
	strcat(vals_filepath, new_run->num);
	strcat(keys_filepath, "_keys");
	strcat(vals_filepath, "_vals");

	printf("%s\n", keys_filepath);

	return 0;
}

void flush_lsmtree(lsmtree *tree) {
	/* 
	Flushes buffer to L1.
	*/
	// create new run
	run *new_run = malloc(sizeof(run));

	new_run->num = malloc(1);
	*(new_run->num) = *(tree->run_ctr) + 1;

	// sort by key
	sort(tree->buff->keys, tree->buff->vals);

	// TODO: construct bloom filter and fence pointers

	// write run to disk
	write_run(tree, new_run, tree->buff->keys, tree->buff->vals);

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
		// put(tree, key, val);
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


