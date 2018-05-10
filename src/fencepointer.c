#include "fencepointer.h"

fencepointer *create_fencepointer(KEY_TYPE *keys, int size) {
	printf("creating fencepointer\n");
	fencepointer *fp = malloc(sizeof(fencepointer));

	// calculate number of fencepointers and keys per fencepointer
	int keys_per_page = PAGE_NUM_BYTES / sizeof(KEY_TYPE);
	fp->keys_per_fence = keys_per_page * M_FENCES;

	int num_pages = size / keys_per_page;
	if (num_pages * keys_per_page < size) {
		// round up
		num_pages++;
	}

	fp->num_fences = size / fp->keys_per_fence;
	if (fp->num_fences * fp->keys_per_fence < size) {
		// round up
		fp->num_fences++;
	}

	if (fp->num_fences > 0) {
		// if there's at least some data
		fp->mins = calloc(fp->num_fences, sizeof(KEY_TYPE));

		// populate fencepointers
		int keys_idx;
		for (int p = 0; p < fp->num_fences; p++) {
			keys_idx = p * fp->keys_per_fence;
			fp->mins[p] = keys[keys_idx];
		}
	}

	printf("size: %d, keys_per_page: %d, num_pages: %d, n_fences: %d, keys_per_fence: %d\n", size, keys_per_page, num_pages, fp->num_fences, fp->keys_per_fence);

	return fp;
}

int *query_fencepointer(fencepointer *fp, KEY_TYPE key) {
	// NULL => before start of run
	// i => at fence i or after
	int *fence_num = NULL;

	// printf("query_fencepointer:\n");
	// for (int i = 0; i < fp->num_fences; i++) {
	// 	printf("i: %d, ", fp->mins[i]);
	// }
	// printf("\n");

	if (key >= fp->mins[0]) {
		// not before start of run
		fence_num = malloc(sizeof(int));
		*fence_num = 1;
		// find max fence less than key
		for (int i = 2; i <= fp->num_fences; i++) {
			if (key >= fp->mins[i - 1]) {
				// before start of run
				*fence_num = i;
			}
			else {
				break;
			}
		}
	}

	return fence_num;
}

void free_fencepointer(fencepointer *fp) {
	if (fp->num_fences > 0) {
		free(fp->mins); 
	}

	free(fp);
}
