#include "bloom.h"

bloomfilter *create_bloomfilter(KEY_TYPE *keys, int size) {
	bloomfilter *bf = malloc(sizeof(bf));
	
	return bf;
}

void free_bloomfilter(bloomfilter *bf) {

	free(bf);
}
