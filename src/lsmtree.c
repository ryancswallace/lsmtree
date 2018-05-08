#include "lsmtree.h"

/*
SECTION 1: functions for initializing, creating, loading, maintaining, and saving the LSM tree.
*/

lsmtree *new_lsmtree(void) {
	/* 
	Initializes new LSM tree.
	*/
	lsmtree *tree = malloc(sizeof(lsmtree));
	return tree;
}

int empty_lsmtree(lsmtree *tree, char *name) {
	/* 
	Creates new empty LSM tree.
	*/
	// initialize tree variables
	tree->num_levels = malloc(sizeof(int));
	tree->num_pairs = malloc(sizeof(int));
	tree->pairs_per_level = calloc(MAX_NUM_LEVELS, sizeof(size_t));
	tree->run_ctr = malloc(sizeof(int));

	*(tree->num_levels) = 2; // buffer and L1
	*(tree->num_pairs) = 0;
	*(tree->run_ctr) = 0;

	// initialize buffer
	tree->buff = malloc(sizeof(buffer));

	tree->buff->keys = calloc(BUFF_CAPACITY, sizeof(KEY_TYPE));
	tree->buff->vals = calloc(BUFF_CAPACITY, sizeof(VAL_TYPE));
	tree->buff->dels = calloc(BUFF_CAPACITY, sizeof(bool));

	// initialize levels and first level
	tree->levels = calloc(MAX_NUM_LEVELS, sizeof(level *));

	tree->levels[0] = malloc(sizeof(level));

	tree->levels[0]->num_runs = malloc(sizeof(int));
	*(tree->levels[0]->num_runs) = 0;

	tree->levels[0]->runs = calloc(RATIO, sizeof(run *));

	// create directory for data
	char dir_name[MAX_DIR_LEN] = DATA_DIR;
	strcat(dir_name, name);

	struct stat status = {0};
	if (stat(dir_name, &status) == 0) {
		// directory already exist
		printf("LSM tree with given name already exists.\n");
		return 1;
	} 
    mkdir(dir_name, 0700);
    strcpy(tree->data_dir, dir_name);

	return 0;
}

int load_lsmtree(lsmtree *tree) {
	/* 
	Loads existing serialized LSM tree from disk.
	*/
	return 0;
}

void free_lsmtree(lsmtree *tree) {
	/* 
	Cleans up after LSM tree, freeing all data from memory.
	*/
}

void serialize_lsmtree(lsmtree *tree) {
	/* 
	Serializes memory resident LSM tree data to disk.
	*/
}

char *run_filepath(lsmtree *tree, run *r, bool keys, bool dels) {
	// first create file paths
	char *filepath = malloc(MAX_DIR_LEN);

	char str[sizeof(int)];
	sprintf(str, "%d", *(r->num));

	strcpy(filepath, tree->data_dir);
	strcat(filepath, "/run_");
	strcat(filepath, str);

	if(dels) {
		strcat(filepath, "_dels");
	}
	else if(keys) {
		strcat(filepath, "_keys");
	}
	else {
		strcat(filepath, "_vals");
	}

	return filepath;
}

void write_run(lsmtree *tree, run *new_run) {
	// writes a run to disk
	char *keys_filepath = run_filepath(tree, new_run, true, false);
	char *vals_filepath = run_filepath(tree, new_run, false, false);
	char *dels_filepath = run_filepath(tree, new_run, false, true);

	FILE *f_keys = fopen(keys_filepath, "wb");
	if (f_keys) {
		fwrite(new_run->buff->keys, sizeof(KEY_TYPE), *(new_run->size), f_keys);
		fclose (f_keys);
	}
	else {
		printf("Unable to write to disk.\n");
		exit(EXIT_FAILURE);
	}

	FILE *f_vals = fopen(vals_filepath, "wb");
	if (f_vals) {
		fwrite(new_run->buff->vals, sizeof(VAL_TYPE), *(new_run->size), f_vals);
		fclose (f_vals);
	}
	else {
		printf("Unable to write to disk.\n");
		exit(EXIT_FAILURE);
	}

	// TODO: pack bits
	FILE *f_dels = fopen(dels_filepath, "wb");
	if (f_dels) {
		fwrite(new_run->buff->dels, sizeof(bool), *(new_run->size), f_dels);
		fclose (f_dels);
	}
	else {
		printf("Unable to write to disk.\n");
		exit(EXIT_FAILURE);
	}

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

void read_run(lsmtree *tree, run *r, buffer *buff) {
	// reads keys, vals, and del flags into provided buffer
	char *keys_filepath = run_filepath(tree, r, true, false);
	char *vals_filepath = run_filepath(tree, r, false, false);
	char *dels_filepath = run_filepath(tree, r, false, true);

	// read from disk
	FILE *f_keys = fopen(keys_filepath, "rb");
	if (f_keys) {
		fread(buff->keys, sizeof(KEY_TYPE), *(r->size), f_keys);
		fclose (f_keys);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	FILE *f_vals = fopen(vals_filepath, "rb");
	if (f_vals) {
		fread(buff->vals, sizeof(VAL_TYPE), *(r->size), f_vals);
		fclose (f_vals);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	FILE *f_dels = fopen(dels_filepath, "rb");
	if (f_dels) {
		fread(buff->dels, sizeof(bool), *(r->size), f_dels);
		fclose (f_dels);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

int sort_buff(buffer *buff) {
	// sorts buffer in place, removing duplicate keys and perfoming deletes
	// returns number of logical elements sorted buffer
	
	return 2;
}

run *sort_merge(lsmtree *tree, level *l) {
	// sort merges lvl and returns pointer to newly created run
	run *new_run = malloc(sizeof(run));

	new_run->num = malloc(sizeof(int));
	(*tree->run_ctr)++;
	*(new_run->num) = *(tree->run_ctr);

	// new_run->buff = malloc(sizeof(buffer *));
	// new_run->buff = tree->buff;

	// new_run->size = malloc(sizeof(int));
	// *(new_run->size) = ;
	
	// TODO: construct bloom filter and fence pointers

	return new_run;
}

void flush_lsmtree(lsmtree *tree) {
	/* 
	Flushes buffer to L1 and resets metadata. Does NOT zero buffer.
	*/
	// sort by key
	int num_pairs = sort_buff(tree->buff);

	// create and insert new run
	run *new_run = malloc(sizeof(run));

	new_run->num = malloc(sizeof(int));
	(*tree->run_ctr)++;
	*(new_run->num) = *(tree->run_ctr);

	new_run->buff = malloc(sizeof(buffer *));
	new_run->buff = tree->buff;

	new_run->size = malloc(sizeof(int));
	*(new_run->size) = num_pairs;

	tree->levels[0]->runs[*(tree->levels[0]->num_runs)] = new_run;
	
	(*tree->levels[0]->num_runs)++;
	tree->pairs_per_level[1] += num_pairs;

	// TODO: construct bloom filter and fence pointers


	// write run to disk
	write_run(tree, new_run);
	
	// reset buffer
	tree->pairs_per_level[0] = 0;
}

void merge_lsmtree(lsmtree *tree, int level_num) {
	// TODO: fix segfault, update counters, variables on merge, free functions for each struct (esp free runs written to disk).

	// merges at specified level_num of lsm tree, if necessary
	// uses tiering
	// at termination, no merges remain
	int level_capacity = int_pow(RATIO, level_num);
	if (*(tree->levels[level_num - 1]->num_runs) == level_capacity) {
		// maintain level count
		if (level_num >= MAX_NUM_LEVELS) {
			printf("Increase MAX_NUM_LEVELS.\n");
			exit(EXIT_FAILURE);
		}

		// add level if necessary
		if (level_num == *(tree->num_levels) - 1) {
			(*tree->num_levels)++;

			tree->levels[level_num] = malloc(sizeof(level));

			tree->levels[level_num]->num_runs = malloc(sizeof(int));
			*(tree->levels[level_num]->num_runs) = 0;

			tree->levels[level_num]->runs = calloc(RATIO * level_capacity, sizeof(run *));
		}

		// sort merge runs, add run to level_num + 1, write run to disk
		run *new_run = sort_merge(tree, tree->levels[level_num - 1]); 
		tree->levels[level_num]->runs[*(tree->levels[level_num]->num_runs)] = new_run;
		// write_run(tree, new_run);

		// clear runs from level_num
		tree->pairs_per_level[level_num - 1] = 0;
		tree->pairs_per_level[level_num] += *(new_run->size);

		*(tree->levels[level_num - 1]->num_runs) = 0;
		*(tree->levels[level_num]->num_runs) += 1;

		// merge next level
		merge_lsmtree(tree, level_num + 1);
	}
}

/*
SECTION 2: functions for querying LSM tree.
*/

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val, bool del) {
	if (tree->pairs_per_level[0] < BUFF_CAPACITY) {
		tree->buff->keys[tree->pairs_per_level[0]] = key;
		tree->buff->vals[tree->pairs_per_level[0]] = val;
		tree->buff->dels[tree->pairs_per_level[0]] = del;

		tree->pairs_per_level[0]++;
		if (del) {
			// assume key already in tree to avoid cost of maintaining true 
			// count
			(*tree->num_pairs)--;
		}
		else {
			(*tree->num_pairs)++;
		}
	}
	else {
		// flush buffer to L1
		flush_lsmtree(tree);

		// merge if necessary
		merge_lsmtree(tree, 1);

		// now space in buffer; call put again
		put(tree, key, val, del);
	}
}

VAL_TYPE get(lsmtree *tree, KEY_TYPE key) {
	return tree->buff->vals[key];
}

KEY_TYPE *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop) {
	return 0;
}

void delete(lsmtree *tree, KEY_TYPE key) {
	put(tree, key, 0, true);
}

void load(lsmtree *tree, char *filename) {

}

void print_stats(lsmtree *tree) {
	printf("Total pairs: %d\n\n", *(tree->num_pairs));

	for (int level_num = 0; level_num < *(tree->num_levels); level_num++) {
		printf("LVL%d: %d\n", level_num, tree->pairs_per_level[level_num]);
	}

	printf("\nContents:\n\n");
	for (int level_num = 0; level_num < *(tree->num_levels); level_num++) {
		if (level_num == 0) {
			// buffer
			for (int idx = 0; idx < tree->pairs_per_level[0]; idx++) {
				if (!tree->buff->dels[idx]) {
					printf("%d:%d:L%d ", tree->buff->keys[idx], 
						tree->buff->vals[idx], 0);
				}
			}
		}
		else {
			// on disk
			for (int level_num = 1; level_num < *(tree->num_levels); level_num++) {
				for (int run = 0; run < *(tree->levels[level_num-1]->num_runs); 
					run++) {
					buffer *buff = malloc(sizeof(buffer));
					buff->keys = calloc(*(tree->levels[level_num-1]->runs[run]->size), sizeof(KEY_TYPE));
					buff->vals = calloc(*(tree->levels[level_num-1]->runs[run]->size), sizeof(VAL_TYPE));
					buff->dels = calloc(*(tree->levels[level_num-1]->runs[run]->size), sizeof(bool));

					read_run(tree, tree->levels[level_num-1]->runs[run], buff);

					for (int idx = 0; idx < *(tree->levels[level_num-1]->runs[run]->size); idx++) {
						if (!(buff->dels[idx])) {
							printf("%d:%d:L%d ", buff->keys[idx], buff->vals[idx], level_num);
						}
					}

					free(buff);
				}
			}
		}
		printf("\n\n");
	}
}


