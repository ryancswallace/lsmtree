#ifndef FENCEPOINTER_H
#define FENCEPOINTER_H

#include <stdio.h>
#include <stdlib.h>

#include "params.h"

typedef struct fencepointer {
	int num_fences;
	int keys_per_fence;

	KEY_TYPE *mins;
} fencepointer;

fencepointer *create_fencepointer(KEY_TYPE *keys, int size);
void query_fencepointer(fencepointer *fp, KEY_TYPE key, int *idx_range_start, int *idx_range_stop);
void free_fencepointer(fencepointer *fp);

#endif
