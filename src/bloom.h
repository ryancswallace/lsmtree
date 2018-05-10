#ifndef BLOOM_H
#define BLOOM_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "params.h"
#include "util.h"

typedef struct hashtable {
	int type;
	int len;

	bool *table; // TODO: pack bits
} hashtable;

typedef struct bloomfilter {
	int num_hts;
	hashtable **hts;
} bloomfilter;

bloomfilter *create_bloomfilter(KEY_TYPE *keys, int size);
void set_bloomfilter(bloomfilter *bf, KEY_TYPE key);

unsigned int hash(KEY_TYPE key, int type, int len);

void set_hashtable(hashtable *ht, KEY_TYPE key);
void set_bloomfilter(bloomfilter *bf, KEY_TYPE key);

bool query_hashtable(hashtable *ht, KEY_TYPE key);
bool query_bloomfilter(bloomfilter *bf, KEY_TYPE key);

int opt_table_size(int run_len, int level_num, int N);

void free_bloomfilter(bloomfilter *bf);

#endif
