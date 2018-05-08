#ifndef LSMTREE_H
#define LSMTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#include "fencepointer.h"
#include "bloom.h"
#include "util.h"
#include "params.h"

typedef struct buffer {
	KEY_TYPE *keys;
	VAL_TYPE *vals;
	bool *dels;
} buffer;

typedef struct run {
	int *num;
	buffer *buff; // NULL for runs on disk

	int *size; // physical

	fencepointer *fences;
	bloomfilter *filter;
} run;

typedef struct level {
	int *num_runs;
	run **runs;
} level;

typedef struct lsmtree {
	// name as saved on disk
	char name[MAX_DIR_LEN];

	// data
	char data_dir[MAX_DIR_LEN];
	buffer *buff;
	level **levels; // L1, L2... (levels on disk)

	// dynamic statistics
	int *num_levels; // including buffer

	int *num_pairs; // logical
	int *pairs_per_level; // physical

	int *run_ctr; // total number of runs ever created
} lsmtree;

lsmtree *new_lsmtree(void);
int load_lsmtree(lsmtree *tree);
int empty_lsmtree(lsmtree *tree, char *name);
void free_lsmtree(lsmtree *tree);
void serialize_lsmtree(lsmtree *tree);
char *run_filepath(lsmtree *tree, run *r, bool keys, bool dels);
void write_run(lsmtree *tree, run *new_run);
void read_run(lsmtree *tree, run *r, buffer *buff);
int sort_buff(buffer *buff);
run *sort_merge(lsmtree *tree, level *l); 
void flush_lsmtree(lsmtree *tree);
void merge_lsmtree(lsmtree *tree, int level_num);

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val, bool del);
VAL_TYPE get(lsmtree *tree, KEY_TYPE key);
KEY_TYPE *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop);
void delete(lsmtree *tree, KEY_TYPE key);
void load(lsmtree *tree, char *filename);
void print_stats(lsmtree *tree);

#endif
