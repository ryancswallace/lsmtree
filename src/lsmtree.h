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
	int *size; // physical

	KEY_TYPE *keys;
	VAL_TYPE *vals;
	DEL_TYPE *dels;
} buffer;

typedef struct run {
	int *num;
	buffer *buff; // NULL for runs on disk

	fencepointer *fp;
	bloomfilter *bf;
} run;

typedef struct level {
	int *num_runs;
	run **runs;
} level;

typedef struct lsmtree {
	// name as saved on disk
	char *name;

	// data
	char *data_dir;
	buffer *buff;
	level **levels; // L1, L2... (levels on disk)

	// dynamic statistics
	int *num_levels; // including buffer

	int *num_pairs; // logical
	int *pairs_per_level; // physical, includes L0

	int *run_ctr; // total number of runs ever created
} lsmtree;

lsmtree *new_lsmtree(void);
int load_lsmtree(lsmtree *tree);
int empty_lsmtree(lsmtree *tree, char *name);
void free_lsmtree(lsmtree *tree);
void serialize_lsmtree(lsmtree *tree);
char *run_filepath(lsmtree *tree, run *r, bool keys, bool dels);
void write_run(lsmtree *tree, run *new_run);
void read_vals_dels(lsmtree *tree, run *r, buffer *buff, int idx_start, int idx_stop);
void read_keys(lsmtree *tree, run *r, buffer *buff, int idx_start, int idx_stop);
void read_run(lsmtree *tree, run *r, buffer *buff);
void erase_run(lsmtree *tree, run *r);
level *read_level(lsmtree *tree, int level_num);
void sort(lsmtree *tree, buffer *buff);
void merge(lsmtree *tree, int level_num, run *new_run);
run *merge_level(lsmtree *tree, int level_num); 
void merge_lsmtree(lsmtree *tree, int level_num);

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val, DEL_TYPE del);
void probe_run(lsmtree *tree, run *r, KEY_TYPE key, VAL_TYPE **res, DEL_TYPE **del);
VAL_TYPE *get(lsmtree *tree, KEY_TYPE key);
buffer *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop);
void delete(lsmtree *tree, KEY_TYPE key);
void load(lsmtree *tree, char *filepath);
void print_stats(lsmtree *tree);

void free_buffer_data(buffer *buff);
void free_run_data(run *r);
void free_level_data(level *l);
void free_lsmtree_data(lsmtree *tree);
void free_buffer(buffer *buff);
void free_run(run *r);
void free_level(level *l);
void free_lsmtree(lsmtree *tree);
void erase_level(lsmtree *tree, int level_num);

#endif
