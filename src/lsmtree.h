#ifndef LSMTREE_H
#define LSMTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fencepointer.h"
#include "bloom.h"
#include "util.h"

#define MAX_DIR_LEN 2048

// LSM tree parameters
#define DATA_DIR "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/data/"
#define BUFF_CAPACITY 2

typedef int32_t KEY_TYPE; 
typedef int32_t VAL_TYPE; 

typedef struct buffer {
	KEY_TYPE *keys;
	VAL_TYPE *vals;

	size_t capacity;
} buffer;

typedef struct run {
	char *num;
	fencepointer *fences;
	bloomfilter *filter;
} run;

typedef struct level {
	size_t level_num;
	run *runs;
} level;

typedef struct lsmtree {
	// name as saved on disk
	char name[MAX_DIR_LEN];

	// data
	buffer *buff;
	level *levels;
	char data_dir[MAX_DIR_LEN];

	// parameters
	size_t L1_capacity;
	size_t ratio;

	// dynamic statistics
	int *num_pairs;
	int *pairs_per_level;
	int *run_ctr; // total number of runs ever created
} lsmtree;

lsmtree *init_lsmtree(void);
int load_lsmtree(lsmtree *tree);
int empty_lsmtree(lsmtree *tree, char *name);
void free_lsmtree(lsmtree *tree);
void serialize_lsmtree(lsmtree *tree);
int write_run(lsmtree *tree, run *new_run, KEY_TYPE *keys, VAL_TYPE *vals);
void flush_lsmtree(lsmtree *tree);

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val);
VAL_TYPE get(lsmtree *tree, KEY_TYPE key);
KEY_TYPE *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop);
void delete(lsmtree *tree, KEY_TYPE key);
void load(lsmtree *tree, char *filename);
void print_stats(lsmtree *tree);

#endif
