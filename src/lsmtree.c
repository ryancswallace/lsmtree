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
	// initialize buffer
	tree->buff = malloc(sizeof(buffer));
	tree->buff->keys = calloc(BUFF_CAPACITY, sizeof(KEY_TYPE));
	tree->buff->vals = calloc(BUFF_CAPACITY, sizeof(VAL_TYPE));
	tree->buff->dels = calloc(BUFF_CAPACITY, sizeof(bool));

	// initialize levels
	tree->levels = calloc(MAX_LEVELS, sizeof(level *));
	for (int level = 0; level < MAX_LEVELS; level++) {
		tree->levels[level] = malloc(sizeof(level));
		tree->levels[level]->num_runs = malloc(sizeof(int));
		*(tree->levels[level]->num_runs) = 0;
	}

	tree->levels[0]->runs = calloc(RATIO, sizeof(run *));

	// initialize tree variables
	tree->num_levels = malloc(sizeof(int));
	tree->num_pairs = malloc(sizeof(int));
	tree->pairs_per_level = calloc(MAX_LEVELS, sizeof(size_t));
	tree->run_ctr = malloc(sizeof(int));

	*(tree->num_levels) = 2; // buffer and L1
	*(tree->num_pairs) = 0;
	*(tree->run_ctr) = 0;

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

void write_run(lsmtree *tree, run *new_run, buffer *buff) {
	// writes a run to disk
	char *keys_filepath = run_filepath(tree, new_run, true, false);
	char *vals_filepath = run_filepath(tree, new_run, false, false);
	char *dels_filepath = run_filepath(tree, new_run, false, true);

	FILE *f_keys = fopen(keys_filepath, "wb");
	if (f_keys) {
		fwrite(buff->keys, sizeof(KEY_TYPE), tree->pairs_per_level[0], f_keys);
		fclose (f_keys);
	}
	else {
		printf("Unable to write to disk.\n");
	}

	FILE *f_vals = fopen(vals_filepath, "wb");
	if (f_vals) {
		fwrite(buff->vals, sizeof(VAL_TYPE), tree->pairs_per_level[0], f_vals);
		fclose (f_vals);
	}
	else {
		printf("Unable to write to disk.\n");
	}

	// TODO: pack bits
	FILE *f_dels = fopen(dels_filepath, "wb");
	if (f_dels) {
		fwrite(buff->dels, sizeof(bool), tree->pairs_per_level[0], f_dels);
		fclose (f_dels);
	}
	else {
		printf("Unable to write to disk.\n");
	}

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

void read_run(lsmtree *tree, run *r, KEY_TYPE *keys_buff, VAL_TYPE *vals_buff, bool *dels_buff) {
	// reads keys, vals, and del flags
	char *keys_filepath = run_filepath(tree, r, true, false);
	char *vals_filepath = run_filepath(tree, r, false, false);
	char *dels_filepath = run_filepath(tree, r, false, true);

	// TODO: read

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

void flush_lsmtree(lsmtree *tree) {
	/* 
	Flushes buffer to L1 and resets metadata. Does NOT zero buffer.
	*/
	// create and insert new run
	run *new_run = malloc(sizeof(run));

	new_run->num = malloc(sizeof(int));
	tree->run_ctr++;
	*(new_run->num) = *(tree->run_ctr);

	new_run->size = malloc(sizeof(int));
	*(new_run->size) = BUFF_CAPACITY * RATIO;

	tree->levels[0]->runs[*(tree->levels[0]->num_runs)] = new_run;
	
	tree->levels[0]->num_runs++;

	// sort by key
	sort(tree->buff->keys, tree->buff->vals);

	// TODO: construct bloom filter and fence pointers

	// write run to disk
	write_run(tree, new_run, tree->buff);
	
	// reset buffer
	tree->pairs_per_level[0] = 0;
}

void merge_lsmtree(lsmtree *tree) {
	// uses tiering. checks for any required merges. at termination, no merges 
	// remain.

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
		tree->num_pairs++;
	}
	else {
		// flush buffer to L1
		flush_lsmtree(tree);

		// merge if necessary
		merge_lsmtree(tree);

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
	printf("Total pairs: %d\n", *(tree->num_pairs));

	for (int level = 0; level < *(tree->num_levels); level++) {
		printf("LVL%d: %d\n", level, tree->pairs_per_level[level]);
	}

	printf("\n\nContents:\n\n");
	for (int level = 0; level < *(tree->num_levels); level++) {
		if (level == 0) {
			// buffer
			for (int idx = 0; idx < tree->pairs_per_level[0]; idx++) {
				if (!tree->buff->dels[idx]) {
					printf("%d:%d:L%d", tree->buff->keys[idx], 
						tree->buff->vals[idx], 0);
				}
			}
		}
		else {
			// minimize clutter
			run *r = malloc(sizeof(run *));
			// on disk
			for (int level = 1; level < *(tree->num_levels); level++) {
				for (int run = 0; run < *(tree->levels[level-1]->num_runs); 
					run++) {
					r = tree->levels[level-1]->runs[run];

					KEY_TYPE *keys_buff = calloc(*(r->size), sizeof(KEY_TYPE));
					VAL_TYPE *vals_buff = calloc(*(r->size), sizeof(VAL_TYPE));
					bool *dels_buff = calloc(*(r->size), sizeof(bool));

					read_run(tree, tree->levels[level-1]->runs[run], keys_buff,vals_buff, dels_buff);

					for (int idx = 0; idx < *(r->size); idx++) {
						if (!dels_buff[idx]) {
							printf("%d:%d:L%d", keys_buff[idx], vals_buff[idx], level);
						}
					}

					free(keys_buff);
					free(vals_buff);
					free(dels_buff);
				}
			free(r);
			}
		}
		printf("\n\n");
	}
}


