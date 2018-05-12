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
	// initialize names
	tree->name = malloc(MAX_DIR_LEN);
	tree->data_dir = malloc(MAX_DIR_LEN);
	
	// initialize tree variables
	tree->num_levels = malloc(sizeof(int));
	tree->num_pairs = malloc(sizeof(int));
	tree->pairs_per_level = calloc(MAX_NUM_LEVELS, sizeof(size_t));
	tree->run_ctr = malloc(sizeof(int));

	*tree->num_levels = 2; // buffer and L1
	*tree->num_pairs = 0;
	*tree->run_ctr = 0;

	// initialize buffer
	tree->buff = malloc(sizeof(buffer));

	tree->buff->size = malloc(sizeof(int));

	tree->buff->keys = calloc(BUFF_CAPACITY, sizeof(KEY_TYPE));
	tree->buff->vals = calloc(BUFF_CAPACITY, sizeof(VAL_TYPE));
	tree->buff->dels = calloc(BUFF_CAPACITY, sizeof(DEL_TYPE));

	// initialize levels and first level
	tree->levels = calloc(MAX_NUM_LEVELS, sizeof(level *));

	tree->levels[0] = malloc(sizeof(level));

	tree->levels[0]->num_runs = malloc(sizeof(int));
	*tree->levels[0]->num_runs = 0;

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
    strcpy(tree->name, name);

	return 0;
}

char *run_filepath(lsmtree *tree, run *r, bool keys, bool dels) {
	// create string containing file path of run r on disk
	char *filepath = malloc(MAX_DIR_LEN);

	char str[MAX_DIR_LEN];
	sprintf(str, "%d", *r->num);

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
		fwrite(new_run->buff->keys, sizeof(KEY_TYPE), *new_run->buff->size, f_keys);
		fclose (f_keys);
	}
	else {
		printf("Unable to write to disk.\n");
		exit(EXIT_FAILURE);
	}

	FILE *f_vals = fopen(vals_filepath, "wb");
	if (f_vals) {
		fwrite(new_run->buff->vals, sizeof(VAL_TYPE), *new_run->buff->size, f_vals);
		fclose (f_vals);
	}
	else {
		printf("Unable to write to disk.\n");
		exit(EXIT_FAILURE);
	}

	// TODO: pack bits
	FILE *f_dels = fopen(dels_filepath, "wb");
	if (f_dels) {
		fwrite(new_run->buff->dels, sizeof(DEL_TYPE), *new_run->buff->size, f_dels);
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

void read_keys(lsmtree *tree, run *r, buffer *buff, int idx_start, int idx_stop) {
	// reads keys of specified run between idx_start and idx_stop (inclusive) 
	// into provided buffer 
	char *keys_filepath = run_filepath(tree, r, true, false);

	// calculate where to read
	int offset = idx_start * sizeof(KEY_TYPE);
	int num_keys = idx_stop - idx_start + 1;

	// read from disk
	FILE *f_keys = fopen(keys_filepath, "rb");
	if (f_keys) {
		fseek(f_keys, offset, SEEK_SET);
		fread(buff->keys, sizeof(KEY_TYPE), num_keys, f_keys);
		fclose(f_keys);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	free(keys_filepath);
}

void read_vals_dels(lsmtree *tree, run *r, buffer *buff, int idx_start, int idx_stop) {
	// reads vals and dels of specified run between idx_start and idx_stop (
	// inclusive) into provided buffer 
	char *vals_filepath = run_filepath(tree, r, false, false);
	char *dels_filepath = run_filepath(tree, r, false, true);

	// calculate where to read
	int offset_vals = idx_start * sizeof(VAL_TYPE);
	int offset_dels = idx_start * sizeof(DEL_TYPE);
	int num_keys = idx_stop - idx_start + 1;

	// read from disk
	FILE *f_vals = fopen(vals_filepath, "rb");
	if (f_vals) {
		fseek(f_vals, offset_vals, SEEK_SET);
		fread(buff->vals, sizeof(VAL_TYPE), num_keys, f_vals);
		fclose(f_vals);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	FILE *f_dels = fopen(dels_filepath, "rb");
	if (f_dels) {
		fseek(f_dels, offset_dels, SEEK_SET);
		fread(buff->dels, sizeof(DEL_TYPE), num_keys, f_dels);
		fclose(f_dels);
	}
	else {
		printf("Unable to read from disk.\n");
		exit(EXIT_FAILURE);
	}

	free(vals_filepath);
	free(dels_filepath);
}

void read_run(lsmtree *tree, run *r, buffer *buff) {
	// reads all keys, vals, and del flags of the run into provided buffer
	
	char *keys_filepath = run_filepath(tree, r, true, false);
	char *vals_filepath = run_filepath(tree, r, false, false);
	char *dels_filepath = run_filepath(tree, r, false, true);
	// printf("read run\n");

	// read from disk
	FILE *f_keys = fopen(keys_filepath, "rb");

	if (f_keys) {
		fread(buff->keys, sizeof(KEY_TYPE), *r->buff->size, f_keys);
		
		fclose (f_keys);
	}
	else {
	}

	FILE *f_vals = fopen(vals_filepath, "rb");
	if (f_vals) {
		fread(buff->vals, sizeof(VAL_TYPE), *r->buff->size, f_vals);
		fclose (f_vals);
	}
	else {
	}

	FILE *f_dels = fopen(dels_filepath, "rb");
	if (f_dels) {	
		fread(buff->dels, sizeof(DEL_TYPE), *r->buff->size, f_dels);
		fclose (f_dels);
	}
	else {
		exit(EXIT_FAILURE);
	}

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

void erase_run(lsmtree *tree, run *r) {
	// from disk
	char *keys_filepath = run_filepath(tree, r, true, false);
	char *vals_filepath = run_filepath(tree, r, false, false);
	char *dels_filepath = run_filepath(tree, r, false, true);

	remove(keys_filepath);
	remove(vals_filepath);
	remove(dels_filepath);

	free(keys_filepath);
	free(vals_filepath);
	free(dels_filepath);
}

level *read_level(lsmtree *tree, int level_num) {
	// reads a level (>= 1) into memory from disk
	level *l = malloc(sizeof(level));

	l->num_runs = malloc(sizeof(int));
	*l->num_runs = *tree->levels[level_num - 1]->num_runs;

	l->runs = calloc(*(l->num_runs), sizeof(run *));

	for (int run_num = 0; run_num < *(l->num_runs); run_num++) {
		l->runs[run_num] = malloc(sizeof(run));

		l->runs[run_num]->num = malloc(sizeof(int));
		*l->runs[run_num]->num = *tree->levels[level_num - 1]->runs[run_num]->num;

		l->runs[run_num]->fp = malloc(sizeof(fencepointer));
		l->runs[run_num]->fp = tree->levels[level_num - 1]->runs[run_num]->fp;

		l->runs[run_num]->bf = malloc(sizeof(bloomfilter));
		l->runs[run_num]->bf = tree->levels[level_num - 1]->runs[run_num]->bf;

		l->runs[run_num]->buff = malloc(sizeof(buffer));

		l->runs[run_num]->buff->size = malloc(sizeof(int));
		*l->runs[run_num]->buff->size = *tree->levels[level_num - 1]->runs[run_num]->buff->size;

		l->runs[run_num]->buff->keys = calloc(*l->runs[run_num]->buff->size, sizeof(KEY_TYPE));
		l->runs[run_num]->buff->vals = calloc(*l->runs[run_num]->buff->size, sizeof(VAL_TYPE));
		l->runs[run_num]->buff->dels = calloc(*l->runs[run_num]->buff->size, sizeof(DEL_TYPE));

		read_run(tree, tree->levels[level_num - 1]->runs[run_num], l->runs[run_num]->buff);
	}

	return l;
}

void sort(lsmtree *tree, buffer *buff) {
	// sorts buffer in place
	// stably accounts for updates and deletes, updating buff->size
	if (*buff->size == 0) {
		return;
	}

	// insertion sort--replace with merge sort
	KEY_TYPE temp_key;
	VAL_TYPE temp_val;
	DEL_TYPE temp_del;

	int j;
	int i = 1;
	while (i < *buff->size) {
		j = i - 1;

		temp_key = buff->keys[i];
		temp_val = buff->vals[i];
		temp_del = buff->dels[i];

		while (j >= 0 && buff->keys[j] > temp_key) {
			buff->keys[j+1] = buff->keys[j];
			buff->vals[j+1] = buff->vals[j];
			buff->dels[j+1] = buff->dels[j];

			j--;
		}
		i++;

		buff->keys[j+1] = temp_key;
		buff->vals[j+1] = temp_val;
		buff->dels[j+1] = temp_del;
	}

	// account for updates and deletes
	// int repeats = 0;
	// int num_insert_repeats = 0;
	// int insert_idx = 0;
	// KEY_TYPE curr;

	// for (int i = 0; i < *buff->size - 1; i++) {
	// 	curr = buff->keys[i];
	// 	while (buff->keys[i + 1] == curr && i < *buff->size - 1) {
	// 		i++;
	// 		repeats++;
	// 		if (!buff->dels[i + 1]) {
	// 			num_insert_repeats++;
	// 		}
	// 	}
	// 	insert_idx++;
	// }

	// (*buff->size) -= repeats;

	// (*tree->num_pairs) -= num_insert_repeats;
}

void merge(lsmtree *tree, int level_num, run *new_run) {
	// merges sorted runs on level_num (>= 1)
	// updates new_run argument with sorted buffer and sets buff->size

	// read level into memory
	level *l = read_level(tree, level_num);

	// calculate parameters of new run
	int num_runs = *l->num_runs;
	int sum_sizes = 0;
	for (int run_num = 0; run_num < *l->num_runs; run_num++) {
		sum_sizes += (*l->runs[run_num]->buff->size);
	}
	*new_run->buff->size = sum_sizes;

	// create new buffer for merged runs
	new_run->buff->keys = calloc(sum_sizes, sizeof(KEY_TYPE));
	new_run->buff->vals = calloc(sum_sizes, sizeof(VAL_TYPE));
	new_run->buff->dels = calloc(sum_sizes, sizeof(DEL_TYPE));

	// perform merge, counting duplicate keys; inefficient--should be nlog(n)
	int head_idxs[num_runs];
	for (int i = 0; i < num_runs; i++) {
		head_idxs[i] = 0;
	}

	long min_key = MAX_KEY;
	int min_run_num;
	int min_run_idx;
	
	int insert_idx = 0;
	int repeats = 0;
	int num_insert_repeats = 0;
	KEY_TYPE last_key;

	for (int i = 0; i < sum_sizes; i++) {
		for (int run = num_runs - 1; run >= 0; run--) {
			if (head_idxs[run] < *l->runs[run]->buff->size) {
				// run not emptied
				if (l->runs[run]->buff->keys[head_idxs[run]] <= min_key) {
					min_key = l->runs[run]->buff->keys[head_idxs[run]];
					min_run_num = run;
					min_run_idx = head_idxs[run];
				}
			}
		}
		new_run->buff->keys[insert_idx] = l->runs[min_run_num]->buff->keys[min_run_idx];
		new_run->buff->vals[insert_idx] = l->runs[min_run_num]->buff->vals[min_run_idx];
		new_run->buff->dels[insert_idx] = l->runs[min_run_num]->buff->dels[min_run_idx];
		insert_idx++;

		// de-duplicate
		// if (i == 0) {
		// 	new_run->buff->keys[insert_idx] = l->runs[min_run_num]->buff->keys[min_run_idx];
		// 	new_run->buff->vals[insert_idx] = l->runs[min_run_num]->buff->vals[min_run_idx];
		// 	new_run->buff->dels[insert_idx] = l->runs[min_run_num]->buff->dels[min_run_idx];
		// }
		// else {
		// 	if (l->runs[min_run_num]->buff->keys[min_run_idx] != last_key) {
		// 		// don't overwrite
		// 		insert_idx++;
		// 	}
		// 	else {
		// 		// overwrite
		// 		repeats++;
		// 		if (!l->runs[min_run_num]->buff->dels[min_run_idx]) {
		// 			num_insert_repeats++;
		// 		}
		// 	}

		// 	new_run->buff->keys[insert_idx] = l->runs[min_run_num]->buff->keys[min_run_idx];
		// 	new_run->buff->vals[insert_idx] = l->runs[min_run_num]->buff->vals[min_run_idx];
		// 	new_run->buff->dels[insert_idx] = l->runs[min_run_num]->buff->dels[min_run_idx];
		// }

		// last_key = new_run->buff->keys[insert_idx];
	
		// printf("min was %ld=%d at run %d, idx %d\n", min_key, l->runs[min_run_num]->buff->keys[min_run_idx], min_run_num, min_run_idx);

		head_idxs[min_run_num]++;

		min_key = MAX_KEY;
	}

	// (*new_run->buff->size) -= repeats;
	// (*tree->num_pairs) -= num_insert_repeats;

	free_level_data(l);
}

run *merge_level(lsmtree *tree, int level_num) {
	// sort merges level and returns pointer to newly created run
	run *new_run = malloc(sizeof(run));

	new_run->num = malloc(sizeof(int));
	(*tree->run_ctr)++;
	*new_run->num = *tree->run_ctr;

	// create new buffer
	new_run->buff = malloc(sizeof(buffer));
	new_run->buff->size = malloc(sizeof(int));

	if(level_num == 0) {
		// point buffer to CO array, sort buffer
		new_run->buff->keys = tree->buff->keys;
		new_run->buff->vals = tree->buff->vals;
		new_run->buff->dels = tree->buff->dels;

		*new_run->buff->size = *tree->buff->size;

		sort(tree, new_run->buff);

	}
	else {
		// merge, populating new run
		merge(tree, level_num, new_run);
	}
	
	// construct bloom filter and fence pointers
	new_run->fp = create_fencepointer(new_run->buff->keys, *new_run->buff->size);

	int len = opt_table_size_constrained();
	new_run->bf = create_bloomfilter(new_run->buff->keys, len);

	return new_run;
}

void merge_lsmtree(lsmtree *tree, int level_num) {
	// merges at specified level_num of lsm tree, if necessary
	// uses tiering
	// at termination, no merges remain
	int level_capacity = RATIO;
	
	run *new_run;
	
	if(level_num == 0) {
		// only called when L0 is full, must merge
		new_run = merge_level(tree, level_num); 		
	}
	else {
		// check if level at capacity
		if (*tree->levels[level_num - 1]->num_runs == level_capacity) {
			// maintain level count
			if (level_num >= MAX_NUM_LEVELS) {
				// printf("Increase MAX_NUM_LEVELS.\n");
				exit(EXIT_FAILURE);
			}

			// add level if necessary
			if (level_num == *tree->num_levels - 1) {
				(*tree->num_levels)++;
				tree->levels[level_num] = malloc(sizeof(level));

				tree->levels[level_num]->num_runs = malloc(sizeof(int));
				*tree->levels[level_num]->num_runs = 0;

				tree->levels[level_num]->runs = calloc(RATIO * level_capacity, sizeof(run *));
			}

			// sort merge runs
			new_run = merge_level(tree, level_num);
		} 
		else {
			// level not full
			return;
		}
	}

	// add run to level_num + 1
	tree->levels[level_num]->runs[*tree->levels[level_num]->num_runs] = new_run;

	// write run to disk from memory; erase old level from disk
	write_run(tree, new_run);

	if (level_num != 0) {
		// free_run_data(new_run);
		erase_level(tree, level_num);
	}

	// add count of pairs to next level
	tree->pairs_per_level[level_num + 1] += *new_run->buff->size;

	// clear runs from level_num
	tree->pairs_per_level[level_num] = 0;
	if(level_num != 0) {
		*tree->levels[level_num - 1]->num_runs = 0;
	}
	else {
		*tree->buff->size = 0;
	}
	(*tree->levels[level_num]->num_runs) += 1;

	// merge next level
	merge_lsmtree(tree, level_num + 1);
}

void probe_run(lsmtree *tree, run *r, KEY_TYPE key, VAL_TYPE **res, DEL_TYPE **del) {
	// probes run r for specified key. If found, allocates memory and 
	// populates pointers to res and del

	// NULL => before start of run
	// i => after fence i
	// printf("probe_run\n");

	if (*r->buff->size == 0) {
		return;
	}

	// first, probe bloom filter

	bool found = query_bloomfilter(r->bf, key);
	if (!found) {
		return;
	}

	
	// if there are no fence pointers, simply query run
	if (r->fp->num_fences == 0) {
		buffer *buff = malloc(sizeof(buff));
		buff->size = malloc(sizeof(int));
		*buff->size = *r->buff->size;
		buff->keys = calloc(*r->buff->size, sizeof(KEY_TYPE));
		buff->vals = calloc(*r->buff->size, sizeof(VAL_TYPE));
		buff->dels = calloc(*r->buff->size, sizeof(DEL_TYPE));

		read_run(tree, r, buff);

		for (int i = *r->buff->size - 1; i >= 0; i--) {
			if (buff->keys[i] == key) {
				*res = malloc(sizeof(VAL_TYPE));
				*del = malloc(sizeof(DEL_TYPE));

				**res = buff->vals[i];
				**del = buff->dels[i];

				free(buff->vals);
				free(buff->dels);

				break;
			}
		}

		free(buff->size);
		free(buff->keys);
		free(buff);
	}

	// if there are fence pointers, use them
	else {
		int *fence_num = query_fencepointer(r->fp, key);

		if (fence_num) {

			// find range of key indices
			int idx_start = (*fence_num - 1) * r->fp->keys_per_fence;
			int idx_stop = ((*fence_num) * r->fp->keys_per_fence) - 1;
			if (idx_stop > *r->buff->size - 1) {
				idx_stop = *r->buff->size - 1;
			}
			int num_keys = idx_stop - idx_start + 1;

			buffer *buff = malloc(sizeof(buff));
			buff->size = malloc(sizeof(int));
			buff->keys = calloc(num_keys, sizeof(KEY_TYPE));

			// // read portion of run from disk
			read_keys(tree, r, buff, idx_start, idx_stop);

			// check for key in portion
			for (int i = num_keys - 1; i >= 0; i--) {
				if (buff->keys[i] == key) {
					buff->vals = calloc(num_keys, sizeof(VAL_TYPE));
					buff->dels = calloc(num_keys, sizeof(DEL_TYPE));

					read_vals_dels(tree, r, buff, idx_start, idx_stop);

					*res = malloc(sizeof(VAL_TYPE));
					*del = malloc(sizeof(DEL_TYPE));

					**res = buff->vals[i];
					**del = buff->dels[i];

					free(buff->vals);
					free(buff->dels);

					break;
				}
			}

			free(buff->size);
			free(buff->keys);
			free(buff);

		}
		else {
			// key not in run
			return;
		}
	}
}

/*
SECTION 2: functions for querying LSM tree.
*/

void put(lsmtree *tree, KEY_TYPE key, VAL_TYPE val, DEL_TYPE del) {
	if (tree->pairs_per_level[0] < BUFF_CAPACITY) {
		tree->buff->keys[tree->pairs_per_level[0]] = key;
		tree->buff->vals[tree->pairs_per_level[0]] = val;
		tree->buff->dels[tree->pairs_per_level[0]] = del;

		tree->pairs_per_level[0]++;
		(*tree->buff->size)++;
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
		// flush buffer to L1 and merge if necessary
		merge_lsmtree(tree, 0);

		// now space in buffer; call put again
		put(tree, key, val, del);
	}
}

VAL_TYPE *get(lsmtree *tree, KEY_TYPE key) {
	// returns pointer to val if found, otherwise NULL
	// caller must free return value if not NULL
	// printf("getting\n");
	VAL_TYPE *res = NULL;

	// scan L0 backwards
	for (int i = *tree->buff->size - 1; i >= 0; i--) {
		if (tree->buff->keys[i] == key) {
			if (!tree->buff->dels[i]) {
				// not deleted
				res = malloc(sizeof(VAL_TYPE));
				*res = tree->buff->vals[i];
			}
			return res; // still NULL if deleted
		}
	}
	// probe disk
	for (int level_num = 1; level_num < *tree->num_levels; level_num++) {
		// printf("level_num: %d\n", level_num);

		level *l = tree->levels[level_num - 1];
		for (int run_num = *l->num_runs - 1; run_num >= 0; run_num--) {
			// printf("run_num: %d\n", run_num);

			VAL_TYPE *res = NULL;
			DEL_TYPE *del = NULL;

			run *r = l->runs[run_num];
			
			probe_run(tree, r, key, &res, &del);
			
			if (res) {
				// key found
				if (*del) {
					// deleted
					res = NULL;
				}
				free(del);

				return res;
			}
		}
	}

	// not found in tree
	return res; 
}

buffer *range(lsmtree *tree, KEY_TYPE key_start, KEY_TYPE key_stop) {
	return 0;
}

void delete(lsmtree *tree, KEY_TYPE key) {
	put(tree, key, 0, true);
}

void load(lsmtree *tree, char *filepath) {
	// loads all key-value pairs in specified binary file. Reads from disk in 
	// chunks to trade off between memory overhead and I/O
	long num_pairs_read;
	long idx;

	KEY_TYPE key;
	VAL_TYPE val;

	KEY_TYPE chunk[2 * LOAD_NUM_PAIRS];

	FILE *f = fopen(filepath, "rb");
	if (f) {
		while(!feof(f)) {
			num_pairs_read = fread(chunk, sizeof(KEY_TYPE), 2 * LOAD_NUM_PAIRS, f) / 2;

			if (num_pairs_read > 0) {
				for (int i = 0; i < num_pairs_read; i++) {
					idx = 2 * i;
					key = chunk[idx];
					val = chunk[idx + 1];
					
					put(tree, key, val, false);
				}
			}
		}
	}
	else {
		printf("Load file file not found.\n");
	}
}

void print_stats(lsmtree *tree) {
	printf("Total pairs: %d\n\n", *tree->num_pairs);

	for (int level_num = 0; level_num < *tree->num_levels; level_num++) {
		printf("LVL%d: %d\n", level_num, tree->pairs_per_level[level_num]);
	}

	printf("\nContents:\n\n");
	for (int level_num = 0; level_num < *tree->num_levels; level_num++) {
		if (level_num == 0) {
			// buffer
			for (int idx = 0; idx < tree->pairs_per_level[0]; idx++) {
				if (!tree->buff->dels[idx]) {
					printf("%d:%d:L%d ", tree->buff->keys[idx], 
						tree->buff->vals[idx], 0);
				}
			}
			printf("\n\n");
		}
		else {
			// on disk
			for (int run = 0; run < *tree->levels[level_num-1]->num_runs;
				run++) {
				buffer *buff = malloc(sizeof(buffer));
				buff->keys = calloc(*tree->levels[level_num-1]->runs[run]->buff->size, sizeof(KEY_TYPE));
				buff->vals = calloc(*tree->levels[level_num-1]->runs[run]->buff->size, sizeof(VAL_TYPE));
				buff->dels = calloc(*tree->levels[level_num-1]->runs[run]->buff->size, sizeof(DEL_TYPE));

				read_run(tree, tree->levels[level_num-1]->runs[run], buff);

				for (int idx = 0; idx < *tree->levels[level_num-1]->runs[run]->buff->size; idx++) {
					if (!(buff->dels[idx])) {
						printf("%d:%d:L%d ", buff->keys[idx], buff->vals[idx], level_num);
					}
				}
				free_buffer_data(buff);
			}
			printf("\n\n");
		}
	}
}

/*
SECTION 3: functions cleaning up, freeing memory.
The _data functions free the data arrays allocated to each contained buffer,
not the structs themselves.
*/

void erase_level(lsmtree *tree, int level_num) {
	// from disk
	for (int run_num = 0; run_num < *tree->levels[level_num - 1]->num_runs; run_num++) {
		erase_run(tree, tree->levels[level_num - 1]->runs[run_num]);
	}
}

void free_buffer_data(buffer *buff) {
	free(buff->keys);
	free(buff->vals);
	free(buff->dels);
}

void free_run_data(run *r) {
	free_buffer_data(r->buff);
}

void free_level_data(level *l) {
	for (int run_num = 0; run_num < *l->num_runs; run_num++) {
		free_run_data(l->runs[run_num]);
	}
}

void free_lsmtree_data(lsmtree *tree) {
	free(tree->buff);
	for (int level_num = 0; level_num < *tree->num_levels; level_num++) {
		free_level_data(tree->levels[level_num]);
	}
}

void free_buffer(buffer *buff) {
	free_buffer_data(buff);
	
	free(buff->size);
}

void free_run(run *r) {
	free_run_data(r);
	free_fencepointer(r->fp);

	if (*r->buff->size > RUN_MIN) {
		free_bloomfilter(r->bf);
	}

	free(r->num);
}

void free_level(level *l) {
	for (int run_num = 0; run_num < *l->num_runs; run_num++) {
		free_run(l->runs[run_num]);
	}

	free(l->runs);
	free(l->num_runs);
}

void free_lsmtree(lsmtree *tree) {
	free_lsmtree_data(tree);
	
	free(tree->num_levels);
	free(tree->num_pairs);
	free(tree->pairs_per_level);
	free(tree->run_ctr);	

	free(tree->name);
	free(tree->data_dir);
}

int load_lsmtree(lsmtree *tree) {
	/* 
	Loads existing serialized LSM tree from disk.
	*/
	return 0;
}

void serialize_lsmtree(lsmtree *tree) {
	/* 
	Serializes memory resident LSM tree data to disk.
	*/
}

