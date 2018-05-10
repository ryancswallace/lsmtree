#ifndef BLOOM_H
#define BLOOM_H

#include <stdio.h>
#include <stdlib.h>

#include "params.h"

typedef struct bloomfilter {

} bloomfilter;

bloomfilter *create_bloomfilter(KEY_TYPE *keys, int size);
void free_bloomfilter(bloomfilter *bf);

#endif
