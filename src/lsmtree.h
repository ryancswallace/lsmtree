#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int32_t KEY_TYPE; 
typedef int32_t VAL_TYPE; 

typedef struct buffer {
	KEY_TYPE *keys;
	VAL_TYPE *vals;

	size_t capacity;
} buffer;

typedef struct lsmtree {
	// data
	buffer *buff;

	// parameters
	size_t L1_capacity;
	size_t ratio;

	// dynamic statistics
	int num_pairs;
	int *pairs_per_level;
} lsmtree;

lsmtree *init_lsmtree(void);
int load_lsmtree(lsmtree *tree);
void empty_lsmtree(lsmtree *tree, size_t capacity);
void free_lsmtree(lsmtree *tree);
void serialize_lsmtree(lsmtree *tree);
void flush_lsmtree(lsmtree *tree);

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val);
VAL_TYPE get(lsmtree *tree, KEY_TYPE key);
KEY_TYPE *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop);
void delete(lsmtree *tree, KEY_TYPE key);
void load(lsmtree *tree, char *filename);
void print_stats(lsmtree *tree) ;