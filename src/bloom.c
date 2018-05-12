#include "bloom.h"

bloomfilter *create_bloomfilter(KEY_TYPE *keys, int len) {
	// creates a bloomfilter over the keys
	// each hash table has len bins
	bloomfilter *bf = malloc(sizeof(bf));

	// construct bloom filter
	bf->num_hts = NUM_HASHES;
	bf->hts = calloc(bf->num_hts, sizeof(hashtable *));
	for (int ht_num = 0; ht_num < bf->num_hts; ht_num++) {
		bf->hts[ht_num] = malloc(sizeof(hashtable));

		bf->hts[ht_num]->type = ht_num;
		bf->hts[ht_num]->len = len;
		bf->hts[ht_num]->table = calloc(len, sizeof(bool));
	}

	// populate bloom filter
	int num_keys = sizeof(keys) / sizeof(KEY_TYPE);
	for (int i = 0; i < num_keys; i++) {
		set_bloomfilter(bf, keys[i]);
	}

	return bf;
}

unsigned int hash(KEY_TYPE key, int type, int len) {
	// modified from https://stackoverflow.com/questions/664014/
	// hashes key using specified hash type
	uint64_t x = key;
	x += MIN_KEY;

	if (type == 0) {
		x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
	    x = x ^ (x >> 31);
	}
	else if (type == 1) {
		x = (x ^ (x >> 30)) * UINT64_C(0x94d049bb133111eb);
	    x = (x ^ (x >> 27)) * UINT64_C(0xbf58476d1ce4e5b9);
	    x = x ^ (x >> 31);
	}
	else if (type == 2) {
		x = (x ^ (x >> 30)) * UINT64_C(0x5358426d1c52e5b9);
	    x = (x ^ (x >> 27)) * UINT64_C(0xa4d349bb132222eb);
	    x = x ^ (x >> 31);
	}
	else {
		printf("Increase NUM_HASHES\n.");
		exit(EXIT_FAILURE);
	}
    unsigned int hashed = x;
	hashed = hashed % len;

	return hashed;
}

void set_hashtable(hashtable *ht, KEY_TYPE key) {
	unsigned int hashed = hash(key, ht->type, ht->len);
	ht->table[hashed] = true;

}

void set_bloomfilter(bloomfilter *bf, KEY_TYPE key) {
	// printf("setting bf for %d\n", key);
	// sets specified key in bloomfilter bf
	for (int ht_num = 0; ht_num < bf->num_hts; ht_num++) {
		// set hash table
		set_hashtable(bf->hts[ht_num], key);
	}
}

bool query_hashtable(hashtable *ht, KEY_TYPE key) {
	// printf("querying ht %d for %d\n", ht->type, key);

	unsigned int hashed = hash(key, ht->type, ht->len);
	bool found = ht->table[hashed];
	
	return found;
}

bool query_bloomfilter(bloomfilter *bf, KEY_TYPE key) {
	// printf("querying bf for %d\n", key);
	// sets specified key in bloomfilter bf
	bool found;
	for (int ht_num = 0; ht_num < bf->num_hts; ht_num++) {
		// query hash table
		found = query_hashtable(bf->hts[ht_num], key);
		if (found) {
			return true;
		}
	}

	// not found in any hash table
	return false;
}

int opt_table_size_ideal(int level_num, int N) {
	// Monkey inspired	
	float T = RATIO;
	float T_L = (float) int_pow(RATIO, level_num - 1);

	float p_i = P_1 * T_L;
	float ln_p_i = log(p_i);
	float ln_2_sq = pow(log(2), 2); 

	float len = (-1 * (float) N / T_L) * ((T - 1) / T) * (ln_p_i / ln_2_sq);

	int opt_size = (int) len;
	if (opt_size < 1) {
		opt_size = 1;
	}

	return opt_size;
}

int opt_table_size_constrained(void) {
	// calculates approximate maximum size of each hash table of the bloom
	// filter that can fit within the memory constraints after the buffer
	// is allocated
	int bytes_buff = (sizeof(KEY_TYPE) + sizeof(VAL_TYPE) + sizeof(DEL_TYPE)) * BUFF_CAPACITY;
	int bytes_bf = BUFF_PLUS_BF_CAPACITY - bytes_buff;
	int bytes_per_hash = bytes_bf / NUM_HASHES;

	int max_runs = RATIO * EST_NUM_LAYERS;
	int bytes_per_hash_per_run = bytes_per_hash / max_runs;

	if (bytes_per_hash_per_run < 0) {
		bytes_per_hash_per_run = 0;
	}
	
	return bytes_per_hash_per_run;
}

void free_bloomfilter(bloomfilter *bf) {
	for (int ht_num = 0; ht_num < bf->num_hts; ht_num++) {
		free(bf->hts[ht_num]->table);
		free(bf->hts[ht_num]);
	}

	free(bf->hts);
	free(bf);
}
