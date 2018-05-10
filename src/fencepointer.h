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
int *query_fencepointer(fencepointer *fp, KEY_TYPE key);
void free_fencepointer(fencepointer *fp);

#endif
